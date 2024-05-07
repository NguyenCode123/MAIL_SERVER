#pragma once
#include"Library.h"
#include"DataMail.h"

#define PORT 110

string PrintListELemnt(string num, string Folder);
vector<string> ListPrint(string recipient);
void CreateServerPOP3(string IP, WSADATA& wsaData, SOCKET& serverSocket);
int HaveDataSQL(string EmailAccount[], string Password[], string Download[]);
void ChangeInNumberMail(string recipient, int num);
int RecieveMail(string sender, string recipient, string dataMail, WSADATA wsaData, SOCKET serverSocket, SOCKET clientSocket, sockaddr_in clientAddr, int clientLen);