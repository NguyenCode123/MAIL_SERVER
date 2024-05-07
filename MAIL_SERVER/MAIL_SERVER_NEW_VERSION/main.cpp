#include"StartServer.h"

int main()
{

    thread t1(SMTPWorks);
    t1.detach();

    thread t2(POP3Works);
    t2.detach();

    string Command = "";
    while (true)
    {
        cin >> Command;
        if (Command == "Quit");
        break;
    }
}