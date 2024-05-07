#include"DataMail.h"

int FindNumMail(string recipient)
{
	string LinkNumber = "";
	CreateDirectoryA(recipient.c_str(), NULL);
	LinkNumber = recipient + "\\" + "NumberMail.txt";

	ofstream Open;
	Open.open(LinkNumber, ios::app);
	Open.close();

	ifstream Input;
	Input.open(LinkNumber);
	int num = 0;
	string x = "";

	while (Input >> x)
	{
		num = num + 1;
	}
	Input.close();

	++num;
	Open.open(LinkNumber, ios::app);
	Open << num << endl;
	Open.close();

	return num;
}

void AddNewMailBoxSQL(string recipient,int num)
{
	String ^ connectionString = "Data Source=DESKTOP-PB2C200;Initial Catalog=EMAIL_CLIENT;Integrated Security=True";
	SqlConnection^ connection = gcnew SqlConnection(connectionString);
	connection->Open();

	string data = "INSERT INTO MailBox (EmailAccount, NumberMail) VALUES ('" + recipient + "', " + to_string(num) + ")";
	
	// Tạo và thực thi câu lệnh SQL để chèn dữ liệu vào bảng
	String^ commandText = msclr::interop::marshal_as<String^>(data);
	SqlCommand^ command = gcnew SqlCommand(commandText, connection);
	
	command->ExecuteNonQuery();

	// Đóng kết nối
	connection->Close();
}

void StoreMail(string sender, string recipient, string dataMail, vector<string> LinkFile)
{
	string LinkMail = "";
	CreateDirectoryA(recipient.c_str(), NULL);

	int num = FindNumMail(recipient);
	AddNewMailBoxSQL(recipient, num);
	LinkMail = recipient + "\\" + to_string(num) + "." + ".txt";

	ofstream Open;

	Open.open(LinkMail);
	string ReadStatus = "NO";

	//*TO DO: Load Data Mail in this .txt file
	Open << sender << endl;
	Open << recipient << endl;
	Open << dataMail << endl;

	if (LinkFile.size() != 0)
	{
		Open << "BASE 64: FILE" << endl;
		for (int i = 0; i < LinkFile.size(); i++)
		{
			Open << LinkFile[i];
		}
	}

	Open << ReadStatus;

	Open.close();
}
