#include"StartServer.h"

void SMTPWorks()
{
    string sender = "";
    string recipient = "";
    string dataMail = "";

    WSADATA wsaData; SOCKET serverSocket; sockaddr_in clientAddr;
    SOCKET clientSocket; int clientLen = sizeof(clientAddr);
    CreateServerSMTP("127.0.0.1", wsaData, serverSocket);

    while (true)
    {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        thread t1(SendEmail, "127.0.0.1", sender, recipient, dataMail, wsaData, serverSocket, clientSocket, clientAddr, clientLen);
        t1.detach();
    }
}

void POP3Works()
{
    string sender = "";
    string recipient = "";
    string dataMail = "";

    WSADATA wsaData; SOCKET serverSocket; sockaddr_in clientAddr;
    SOCKET clientSocket; int clientLen = sizeof(clientAddr);
    CreateServerPOP3("127.0.0.1", wsaData, serverSocket);

    while (true)
    {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        thread t2(RecieveMail, sender, recipient, dataMail, wsaData, serverSocket, clientSocket, clientAddr, clientLen);
        t2.detach();
    }
}