#include "Functions.h"

void prefixDetection(string& pref, string& tmp)
{
    int i = 0;
    if (tmp.at(i) == '/')
    {
        while (tmp.at(i) != ' ')
        {
            pref.push_back(tmp.at(i));
            i++;
        }
        i++;
    }
    tmp = tmp.substr(tmp.find(' ') + 1, tmp.length());
}


#if defined _WIN32 || _WIN64
void commandProcessing(string pref, string tmp, bool& success, SOCKET ClientSocket)
{
    
    int i = 0;
    if (pref == "/reg")
    {
        string second_name = "";
        string name = "";
        string email = "";
        string pass = "";

        int i = 0;
        while (tmp.at(i) != ' ')
        {
            second_name.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (tmp.at(i) != ' ')
        {
            name.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (tmp.at(i) != ' ')
        {
            email.push_back(tmp.at(i));
            i++;
        }
        i++;
        for (int k = i, j = 0; j != 5; k++)
        {
            if (tmp.at(k) == ' ')
            {
                j++;
            }
            pass.push_back(tmp.at(k));

        }

        success = chat1.Registration(second_name, name, email, pass);
        return;
    }
    else if (pref == "/auth")
    {
        string email = "";
        string pass = "";

        int i = 0;
        while (tmp.at(i) != ' ')
        {
            email.push_back(tmp.at(i));
            i++;
        }
        i++;
        for (int k = i, j = 0; j != 5; k++)
        {
            if (tmp.at(k) == ' ')
            {
                j++;
            }
            pass.push_back(tmp.at(k));

        }

        success = chat1.Authorization(email, pass);
        return;
    }
    else if (pref == "/m")
    {
        string sender_email = "";
        string recepient_email = "";
        string text = "";

        i = 0;
        while (tmp.at(i) != ' ')
        {
            sender_email.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (tmp.at(i) != ' ')
        {
            recepient_email.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (i != tmp.length())
        {
            text.push_back(tmp.at(i));
            i++;
        }

        success = chat1.WriteMessage(sender_email, recepient_email, text);
    }
    else if (pref == "/del")
    {
        success = chat1.DeleteFromChat(tmp);
    }

    else if (pref == "/ruol")
    {
        chat1.ShowUsersOnline(ClientSocket);
    }
    else if (pref == "/rul")
    {
        chat1.ShowAllUsers(ClientSocket);
    }
    else if (pref == "/rpm")
    {
        chat1.ReadPrivateMessages(tmp, ClientSocket);
    }
    else if (pref == "/rgm")
    {
        chat1.ReadGeneralMessages(tmp, ClientSocket);
    }
    else if (pref == "/q")
    {
        chat1.QuitServer(tmp);
    }
}
#elif defined __linux__
void commandProcessing(string pref, string tmp, bool& success, int connection)
{
    int i = 0;
    if (pref == "/reg")
    {
        string name = "";
        string log = "";
        string psw = "";

        i = 0;
        while (tmp.at(i) != ' ')
        {
            name.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (tmp.at(i) != ' ')
        {
            log.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (i != tmp.length())
        {
            psw.push_back(tmp.at(i));
            i++;
        }

        if (psw == " " || log == " " || name == " ")
            success = false;
        else
            success = chat1.Registration(name, log, psw);
    }
    else if (pref == "/auth")
    {
        string log = "";
        string psw = "";

        i = 0;
        while (tmp.at(i) != ' ')
        {
            log.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (i != tmp.length())
        {
            psw.push_back(tmp.at(i));
            i++;
        }

        success = chat1.authorization(log, psw);
    }
    else if (pref == "/m")
    {
        string sender = "";
        string recepient = "";
        string text = "";

        i = 0;
        while (tmp.at(i) != ' ')
        {
            sender.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (tmp.at(i) != ' ')
        {
            recepient.push_back(tmp.at(i));
            i++;
        }
        i++;
        while (i != tmp.length())
        {
            text.push_back(tmp.at(i));
            i++;
        }

        success = chat1.WriteMessage(sender, recepient, text);
    }
    else if (pref == "/del")
    {
        success = chat1.DeleteUser(tmp);
    }
    else if (pref == "/rul")
    {
        chat1.SendUsersList(connection);
    }
    else if (pref == "/rpm")
    {
        chat1.SendPrivateMessages(connection, tmp);
    }
    else if (pref == "/rgm")
    {
        chat1.SendGeneralMessages(connection);
    }
}
#endif
