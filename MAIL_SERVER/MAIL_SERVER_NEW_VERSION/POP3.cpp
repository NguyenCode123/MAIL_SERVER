#include"POP3.h"

string PrintListELemnt(string num, string Folder)
{
    string filePath = Folder + num + "." + ".txt";
    ifstream input;
    input.open(filePath);

    string Byte;
    string temp;
    input >> temp;
    input >> temp;
    input >> Byte;

    while (!input.eof())
    {
        input >> temp;
    }

    string result;
    result = result + num + " " + Byte;
    if (temp == "YES")
    {
        result = result + " READ";
    }
    input.close();

    return result;
}

vector<string> ListPrint(string recipient)
{
    CreateDirectoryA(recipient.c_str(), NULL);
    int ListCheckNumber = 0;

    string Folder = recipient + "\\";

    vector<string> List;
    ifstream input;
    input.open(Folder + "NumberMail.txt");

    string num;
    while (input >> num)
    {
        List.push_back(num);
    }
    input.close();

    vector<string> result;
    string temp;
    // Lướt từng file
    for (int i = 0; i < List.size(); i++)
    {
        temp = PrintListELemnt(List[i], Folder);
        result.push_back(temp);
    }
    return result;
}

void CreateServerPOP3(string IP, WSADATA& wsaData, SOCKET& serverSocket)
{
    struct sockaddr_in serverAddr;

    // Khởi tạo Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Error in WSAStartup" << std::endl;
        return;
    }

    // Tạo socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Error in socket creation: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Thiết lập cấu trúc sockaddr_in cho máy chủ
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY(IP);

    // Ràng buộc socket với địa chỉ và cổng
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Error in binding: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Error in listening: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    cout << "Server listening on port " << PORT << std::endl;
}

int HaveDataSQL(string EmailAccount[], string Password[], string Download[])
{
    // Kết nối với SQL
    String^ connectionString = "Data Source=DESKTOP-PB2C200;Initial Catalog=EMAIL_CLIENT;Integrated Security=True";
    SqlConnection^ connection = gcnew SqlConnection(connectionString);
    connection->Open();

    SqlCommand^ command = gcnew SqlCommand("SELECT * FROM USERS", connection);
    SqlDataReader^ reader = command->ExecuteReader();

    int count = 0;
    while (reader->Read()) {
        String^ email = safe_cast<String^>(reader["EmailAccount"]); // Thay "ID" bằng tên cột thật sự trong bảng
        String^ password = safe_cast<String^>(reader["Password"]); // Thay "Name" bằng tên cột thật sự trong bảng
        String^ download = safe_cast<String^>(reader["Download"]);
        
        EmailAccount[count] = marshal_as<string>(email);
        Password[count] = marshal_as<string>(password);
        Download[count]= marshal_as<string>(download);
        
        istringstream in1(EmailAccount[count]);
        getline(in1, EmailAccount[count], ' ');

        istringstream in2(Password[count]);
        getline(in2, Password[count], ' ');

        istringstream in3(Download[count]);
        getline(in3, Download[count], ' ');

        count++;
    }

    connection->Close();

    return count;
}

void DeleteRowTable(string recipient, int num)
{
    String^ connectionString = "Data Source=DESKTOP-PB2C200;Initial Catalog=EMAIL_CLIENT;Integrated Security=True";
    SqlConnection^ connection = gcnew SqlConnection(connectionString);
    connection->Open();

    string data = "DELETE FROM MailBox WHERE EmailAccount='" + recipient + "' AND NumberMail=" + to_string(num);

    String^ commandText = msclr::interop::marshal_as<String^>(data);
    SqlCommand^ command = gcnew SqlCommand(commandText, connection);

    command->ExecuteNonQuery();

    connection->Close();
}

void UpdateTableAfterDelete(string recipient, int Num_RM, int Last_Num)
{
    String^ connectionString = "Data Source=DESKTOP-PB2C200;Initial Catalog=EMAIL_CLIENT;Integrated Security=True";
    SqlConnection^ connection = gcnew SqlConnection(connectionString);
    connection->Open();

    for (int i = Num_RM; i < Last_Num; i++)
    {
        string data = "UPDATE MailBox SET NumberMail=" + to_string(i) + " WHERE EmailAccount='" + recipient + "' AND NumberMail=" + to_string(i + 1);
        String^ commandText = msclr::interop::marshal_as<String^>(data);
        SqlCommand^ command = gcnew SqlCommand(commandText, connection);

        command->ExecuteNonQuery();
    }

    connection->Close();
}

void ChangeInNumberMail(string recipient, int num)
{
    ifstream input;
    input.open(recipient + "\\" + "NumberMail.txt");
    int x = 0;
    while (!input.eof())
    {
        input >> x;
    }

    x = x - 1;
    input.close();

    ofstream output;
    output.open(recipient + "\\" + "NumberMail.txt");
    bool check = false;
    for (int i = 1; i <= x; i++)
    {
        if (i == num)
        {
            check = true;
            continue;
        }
        if (check == true)
        {
            output << i - 1 << endl;
            continue;
        }
        output << i << endl;
    }

    output.close();
}

string base64_decode(const string& input) {
    // Sử dụng iterator để decode dữ liệu
    auto base64Decode = [](char c) -> unsigned char 
    {
        const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        auto result = find(begin(base64Chars), end(base64Chars), c);
        return static_cast<unsigned char>(distance(begin(base64Chars), result));
    };

    string decodedData;
    auto inputBegin = input.begin();
    auto inputEnd = input.end();

    while (inputBegin != inputEnd) 
    {
        // Lấy 4 ký tự Base64
        char char1 = *(inputBegin++);
        char char2 = (inputBegin != inputEnd) ? *(inputBegin++) : 0;
        char char3 = (inputBegin != inputEnd) ? *(inputBegin++) : 0;
        char char4 = (inputBegin != inputEnd) ? *(inputBegin++) : 0;

        // Chia 4 ký tự thành 3 byte và decode
        unsigned char byte1 = (base64Decode(char1) << 2) | (base64Decode(char2) >> 4);
        unsigned char byte2 = ((base64Decode(char2) & 0x0F) << 4) | (base64Decode(char3) >> 2);
        unsigned char byte3 = ((base64Decode(char3) & 0x03) << 6) | base64Decode(char4);

        // Thêm 3 byte đã giải mã vào dữ liệu kết quả
        decodedData += byte1;

        if (char3 != '=' && char3 != 0)
            decodedData += byte2;

        if (char4 != '=' && char4 != 0)
            decodedData += byte3;
    }

    return decodedData;
}

void write_binary_file(const string& filename, const string& data) {
    ofstream file;
    file.open(filename, ios::binary);

    if (file) 
    {
        file.write(data.c_str(), data.size());
    }

    file.close();
}

void DownloadFileOnAdrress(string Adrress, string fileName, string fileEncode)
{
    string decodedFileName = Adrress + "\\";

    decodedFileName = decodedFileName + fileName;
    string decodedData = base64_decode(fileEncode);

    write_binary_file(decodedFileName, decodedData);
}

int RecieveMail(string sender, string recipient, string dataMail, WSADATA wsaData, SOCKET serverSocket, SOCKET clientSocket, sockaddr_in clientAddr, int clientLen)
{
    char buffer[1024];

    //Mở lời với client POP3
    string response = "+OK Test Mail Server\r\n";
    send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

    string EmailAccount[100];
    string Password[100];
    string Download[100];
    string UseDownload = "";
    int count_User = 0;
    //Kết nối SQL
    count_User = HaveDataSQL(EmailAccount, Password, Download);

    while (true)
    {
        string receivedData = "";
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (strcmp(buffer, "\r\n") == 0)
            {
                break;
            }
            receivedData.append(buffer, bytesReceived);
        }

        cout << "Received message: " << receivedData << std::endl;

        //Phản hồi lại client
        if (strstr(receivedData.c_str(), "USER") != nullptr)
        {
            recipient = receivedData.substr(5, receivedData.size() - 5);
            response = "+OK\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

            continue;
        }

        if (strstr(receivedData.c_str(), "PASS") != nullptr)
        {
            string PASS = receivedData.substr(5, receivedData.size() - 5);
            
            bool check = false;
            //TO DO: Kiểm tra User và PASS có đăng nhập hợp lệ ko theo hai biến string EmailAccount[100] và string Password[100];
            for (int i = 0; i < count_User; i++)
            {
                if (PASS == Password[i] && recipient == EmailAccount[i])
                {
                    UseDownload = Download[i];
                    check = true;
                    break;
                }
            }

            if (check == true)
            {
                response = "+OK\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            }

            else
            {
                response = "+Wrong User Or Pass\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            }
            continue;
        }

        if (strstr(receivedData.c_str(), "LIST") != nullptr && recipient != "")
        {
            response = "+OK\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            vector<string> ListData = ListPrint(recipient); // In ra các List
            for (int i = 0; i < ListData.size(); i++)
            {
                response = ListData[i] + "\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            }
            continue;
        }

        if (strstr(receivedData.c_str(), "RETR") != nullptr)
        {
            string Folder = recipient + "\\";
            string FilePath = Folder + receivedData.substr(5, receivedData.size() - 5) + "." + ".txt";

            ifstream input;
            input.open(FilePath);
            if (!input.is_open())
            {
                response = "+WRONG NUMBER\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
                input.close();
            }
            else
            {
                response = "+OK\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

                string _sender = "";
                string _recipient = "";
                string _dataMail = "";
                string temp = "";

                getline(input, _sender, '\n');
                response = "From: " + _sender + "\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

                getline(input, _recipient, '\n');
                response = "To: " + _recipient + "\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

                getline(input, temp, '\n'); //Lấy số byte, loại đi

                getline(input, _dataMail, '\n');

                response = _dataMail + "\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

                _dataMail = _dataMail + "\n\n";

                bool YN = false;
                bool checkFileExist = false;
                while (getline(input, temp, '\n'))
                {
                    if (temp != "BASE 64: FILE" && (temp != "NO" && temp != "YES"))
                    {
                        _dataMail = _dataMail + temp + "\n";
                        response = temp + "\r\n";
                        send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
                    }
                    
                    if (temp == "BASE 64: FILE")
                    {
                        checkFileExist = true;
                        break;
                    }

                    if (temp == "YES")
                    {
                        YN = true;
                    }
                }

                if (checkFileExist == true)
                {
                    string FILE_DATA[100];
                    string FILE_NAME[100];

                    int count = 0;
                    while (true)
                    {
                        getline(input, temp, '\n');

                        if (temp == "FILENAME:")
                        {
                            string _fileName = "";
                            string _fileEncode = "";
                            
                            getline(input, temp, '\n');
                            _fileName = temp;

                            response = "File Attach: " + _fileName + "\r\n";
                            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

                            while (getline(input, temp, '\n'))
                            {
                                if (temp == "")
                                {
                                    break;
                                }
                                _fileEncode = _fileEncode + temp;
                            }
                            FILE_DATA[count] = _fileEncode;
                            FILE_NAME[count] = _fileName;
                            count++;
                        }
                        else if (temp == "NO")
                        {
                            break;
                        }
                    }
                    
                    response = "+ FOUND " + to_string(count) + " FILE(S). DO YOU WANT TO DOWNLOAD IT? (YES/NO)\r\n";
                    send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

                    receivedData = "";
                    while (true)
                    {
                        memset(buffer, 0, sizeof(buffer));
                        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

                        if (strcmp(buffer, "\r\n") == 0)
                        {
                            if (receivedData == "YES")
                            {
                                for (int i = 0; i < count; i++)
                                {
                                    DownloadFileOnAdrress(UseDownload, FILE_NAME[i], FILE_DATA[i]);
                                }

                                response = "+ DOWNLOAD SUCCESS ON PATH: " + UseDownload + "\r\n";
                                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
                                break;
                            }
                            else if (receivedData == "NO")
                            {
                                receivedData = "";
                                response = "+OK\r\n";
                                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
                                break;
                            }
                            else
                            {
                                receivedData = "";
                                response = "+ ERROR\r\n";
                                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
                                continue;
                            }
                        }

                        receivedData.append(buffer);
                    }
                    
                }

                input.close();

                if (YN == false)
                {
                    ofstream output;
                    output.open(FilePath, ios::app);
                    output << "\nYES";
                    output.close();
                }
            }

            continue;
        }

        if (strstr(receivedData.c_str(), "DELE") != nullptr)
        {
            string Folder = recipient + "\\";
            
            string FilePath = Folder + receivedData.substr(5, receivedData.size() - 5) + "." + ".txt";

            ifstream input;
            input.open(FilePath);
            if (!input.is_open())
            {
                response = "+WRONG NUMBER\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
                input.close();
            }
            else
            {
                input.close();
                response = "+OK\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

                int Num_RM = atoi(receivedData.substr(5, receivedData.size() - 5).c_str());

                //Xóa file
                remove(FilePath.c_str());

                int LastPos = FindNumMail(recipient) - 1;
                ChangeInNumberMail(recipient, Num_RM);

                DeleteRowTable(recipient, Num_RM);
                UpdateTableAfterDelete(recipient, Num_RM, LastPos);

                for (int i = Num_RM; i < LastPos; i++)
                {
                    string OldName = recipient + "\\" + to_string(i + 1) + "." + ".txt";
                    string NewName = recipient + "\\" + to_string(i) + "." + ".txt";
                    rename(OldName.c_str(), NewName.c_str());
                }

            }

            continue;
        }


        if (receivedData == "QUIT")
        {
            response = "+OK\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            // Đóng kết nối với client
            closesocket(clientSocket);
            break;
        }

        // +ERROR
        response = "+ERROR\r\n";
        send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
    }

    return 0;
}