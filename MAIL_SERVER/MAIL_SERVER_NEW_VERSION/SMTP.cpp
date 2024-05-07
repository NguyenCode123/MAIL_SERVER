#include"SMTP.h"
#include"DataMail.h"

void CreateServerSMTP(string IP, WSADATA& wsaData, SOCKET& serverSocket)
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

//Mã hóa Base 64
string base64_encode(const string& input) {
    // Sử dụng iterator để encode dữ liệu
    auto base64Encode = [](unsigned char c) -> char {
        const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        return base64Chars[c & 0x3F];
    };

    string encodedData;
    auto inputBegin = input.begin();
    auto inputEnd = input.end();

    while (inputBegin != inputEnd) {
        // Lấy 3 byte từ dữ liệu đầu vào
        unsigned char byte1 = *(inputBegin++);
        unsigned char byte2 = (inputBegin != inputEnd) ? *(inputBegin++) : 0;
        unsigned char byte3 = (inputBegin != inputEnd) ? *(inputBegin++) : 0;

        // Chia 3 byte thành 4 nhóm 6 bit và mã hóa thành các ký tự Base64
        encodedData += base64Encode(byte1 >> 2);
        encodedData += base64Encode(((byte1 & 0x03) << 4) | (byte2 >> 4));
        encodedData += base64Encode(((byte2 & 0x0F) << 2) | (byte3 >> 6));
        encodedData += base64Encode(byte3 & 0x3F);
    }

    // Thêm ký tự padding nếu cần thiết
    size_t padding = input.length() % 3;
    if (padding > 0) {
        encodedData.replace(encodedData.length() - padding, padding, padding, '=');
    }

    return encodedData;
}

string read_binary_file(const string& filename) {
    ifstream file;
    file.open(filename, ios::binary);

    if (file) {
        ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    file.close();
    return "";
}

vector<string> SendFile(SOCKET serverSocket, SOCKET clientSocket)
{
    char buffer[1024];
    vector<string> LinkFile;
    string receivedData;
    string response;

    response = "124 FILE PATH? It will be end with line containing only '.'\r\n";
    send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

    while (true)
    {
        receivedData = "";

        while (true)
        {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (strcmp(buffer, "\r\n") == 0)
            {
                break;
            }

            receivedData.append(buffer, bytesReceived);
        }

        if (strstr(receivedData.c_str(), "FILE PATH:") != nullptr)
        {
            string filePath = receivedData.substr(11, receivedData.size() - 11);
            ifstream input;
            input.open(filePath);
            if (!input.is_open())
            {
                response = "550 FILE NOT FOUND\r\n";
                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
                continue;
            }
            else
            {
                string Input_Data = read_binary_file(filePath);
                string encoded_Data = base64_encode(Input_Data);

                int found = filePath.rfind('\\');

                string filename = filePath.substr(found + 1, filePath.size() - found);

                string resultFile = "";
                string subFile = "";
                int run = 0, start = 0, end = 0;
                string responseFile = "";

                int do_dai = strlen(encoded_Data.c_str());
                while (end != strlen(encoded_Data.c_str()) - 1)
                {
                    for (run; (run != start + 71) && (run != strlen(encoded_Data.c_str()) - 1); run++);
                    end = run;
                    subFile = encoded_Data.substr(start, end - start + 1);
                    resultFile = resultFile + subFile + "\n";
                    responseFile = responseFile + subFile + "\r\n";
                    start = run + 1;
                }

                LinkFile.push_back("FILENAME:\n" + filename + "\n" + resultFile + "\n");

                response = "\r\nBase 64:\r\n" + responseFile;

                send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

                continue;

            }
            input.close();
            continue;
        }
        else if (strstr(receivedData.c_str(), ".") != nullptr)
        {
            response = "250 File accepted\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            break;
        }
        else
        {
            response = "550 ERROR ENTER FILE PATH\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
        }

    }

    return LinkFile;
}

vector<string> AskForFileSend(SOCKET serverSocket, SOCKET clientSocket)
{
    char buffer[1024];
    vector<string> LinkFile;
    string receivedData;
    string response;

    response = "123 WANT TO SEND FILE? (YES/NO)\r\n";
    send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

    while (true)
    {
        string receivedData = "";
       
        while (true)
        {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (strcmp(buffer, "\r\n") == 0)
            {
                break;
            }

            receivedData.append(buffer, bytesReceived);
        }
        if (receivedData == "YES")
        {
            LinkFile = SendFile(serverSocket, clientSocket);
            break;
        }
        else if (receivedData == "NO")
        {
            response = "250 OK\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            break;
        }
        else
        {
            response = "550 ERROR ENTER\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            continue;
        }
    }

    return LinkFile;
}

int SendEmail(string IP, string sender, string recipient, string dataMail, WSADATA wsaData, SOCKET serverSocket, SOCKET clientSocket, sockaddr_in clientAddr, int clientLen)
{
    char buffer[1024];

    //Chào Client
    string response = "220 Mail Server Ready\r\n";
    send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

    bool Write_mail = false;//Kiểm tra có đang viết nội dung mail ko
    while (true) {

        string receivedData = "";
        // Nhận thông điệp từ client

        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (strcmp(buffer, "\r\n") == 0 && Write_mail == false)
                break;

            if (strcmp(buffer, "\r\n") == 0 && Write_mail == true)
            {
                receivedData.append("\n", 1);
                memset(buffer, 0, sizeof(buffer));
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

                if (strcmp(buffer, ".") == 0)
                {
                    receivedData.append(buffer, bytesReceived);
                    memset(buffer, 0, sizeof(buffer));
                    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                    if (strcmp(buffer, "\r\n") == 0)
                    {
                        receivedData.append("\n", 1);
                        break;
                    }
                    else
                    {
                        receivedData.append(buffer, bytesReceived);
                    }
                }
                else if (strcmp(buffer, "\r\n") == 0)
                {
                    receivedData.append("\n", 1);
                    continue;
                }
                else
                {
                    receivedData.append(buffer, bytesReceived);
                    continue;
                }
            }
            receivedData.append(buffer, bytesReceived);
        }

        // Phản hồi lại client
        if (receivedData == "HELO " + IP)
        {
            response = "250 OK\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            continue;
        }

        if (strstr(receivedData.c_str(), "MAIL FROM:") != nullptr)
        {
            sender = receivedData.substr(11, receivedData.size() - 11);
            response = "250 sender <" + sender + "> OK\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            continue;
        }

        if (strstr(receivedData.c_str(), "RCPT TO:") != nullptr)
        {
            recipient = receivedData.substr(9, receivedData.size() - 9);
            response = "250 recipient <" + recipient + "> OK\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            continue;
        }

        if (receivedData == "DATA")
        {
            response = "354 enter email, end with line containing only '.'\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            Write_mail = true;
            continue;
        }

        if (Write_mail == true)
        {
            dataMail = receivedData;
            response = "250 " + to_string(dataMail.size()) + " accepted\r\n";
            dataMail = to_string(dataMail.size()) + "\n" + dataMail;
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            vector<string> LinkFile = AskForFileSend(serverSocket, clientSocket);
            
            StoreMail(sender, recipient, dataMail,LinkFile); //Lưu trữ mail vào folder và file .txt
            
            Write_mail = false;

            continue;
        }

    

        if (receivedData == "QUIT")
        {
            response = "221 Closing connection\r\n";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            // Đóng kết nối với client
            closesocket(clientSocket);
            break;
        }

        // 550 ERROR
        response = "550 ERROR\r\n";
        send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
    }

    // Đóng socket của máy chủ
    //closesocket(serverSocket);

    // Cleanup Winsock
    //WSACleanup();
    return 0;
}