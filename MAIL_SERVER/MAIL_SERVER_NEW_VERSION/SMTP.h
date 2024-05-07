#pragma once
#include"Library.h"

#define PORT 25

void CreateServerSMTP(string IP, WSADATA& wsaData, SOCKET& serverSocket);
int SendEmail(string IP, string sender, string recipient, string dataMail, WSADATA wsaData, SOCKET serverSocket, SOCKET clientSocket, sockaddr_in clientAddr, int clientLen);
