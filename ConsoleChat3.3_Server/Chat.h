#pragma once
#if defined _WIN32 || _WIN64
//#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#elif __linux__
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#endif

#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>

#define MESSAGE_LENGTH 1024


//#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <fstream>

#include "Logger.h"
#include <shared_mutex>
#include <sstream>
using namespace std;

class Chat
{
private:
	Logger logs;
public:
	Chat();
	~Chat();

	std::string getLastLogInput();

	bool SetDataBaseConnection();
	void CloseDataBaseConnection();

	bool createDatabase();
	bool creatingDBTables();
	bool creatingDBProcedures();
	bool creatingDBTriggers();
	bool creatingDBViews();
	void deleteDatabase();

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	int Identification(std::string Email);
	bool Registration(std::string second_name, std::string name, std::string email, std::string pass);
	bool Authentification(std::string email, std::string pass);
	bool Authorization(std::string email, std::string pass);
	void QuitServer(std::string email);
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	bool WriteMessage(std::string sender_email, std::string recepient_email, std::string message);
	bool DeleteFromChat(std::string email);
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	void ReadGeneralMessages(std::string email, SOCKET& clientSocket);
	void ReadPrivateMessages(std::string email, SOCKET& clientSocket);
	void ShowAllUsers(SOCKET& clientSocket);
	void ShowUsersOnline(SOCKET& clientSocket);
};
