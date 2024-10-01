#if defined _WIN32 || _WIN64
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


//#define DEFAULT_PORT "27015"

#define IPaddr "127.0.0.1"

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo* result = 
	NULL,
	* ptr = NULL,
	hints;
int iResult;

#define PORT "7777"

#elif defined __linux__
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 7777 // Будем использовать этот номер порта

int socket_file_descriptor, connection;
struct sockaddr_in serveraddress, client;

#endif

#include <iostream>
#include <string>
#include <fstream>
#include "Functions.h"
//#include <sstream>

#define MESSAGE_LENGTH 1024

char message[MESSAGE_LENGTH];

string authUserLogin;
string command;

using namespace std;

void sendMessage(string command)
{
	memset(message, 0, MESSAGE_LENGTH);
	strcpy(message, command.c_str());

#if defined __linux__
	write(socket_file_descriptor, message, sizeof(message));
#elif defined _WIN32 || _WIN64
	iResult = send(ConnectSocket, message, (int)strlen(message), 0);
#endif
}
void getMessage(string& ans)
{
#if defined __linux__
	read(socket_file_descriptor, message, sizeof(message));
#elif defined _WIN32 || _WIN64
	iResult = recv(ConnectSocket, message, MESSAGE_LENGTH, 0);
#endif

	ans = message;
}

void ChatMenu()
{
	string command = "";
	char ch = ' ';
	string msg;

	std::cout << "\n####Welcome#####\n" << endl;

	do
	{

		Menu2();

		(cin >> ch).ignore();
		system("cls");
		//cout << "\033[2J\033[1;1H";

		switch (ch)
		{
		case '1':
		{
			command = "/m";

			getline(cin, msg, '\n');

			command = command + " " + authUserLogin + " " + "ALL" + " " + msg;

			//strcpy(message, command.c_str());

			sendMessage(command);
			string msg;
			getMessage(msg);

			//string msg = message;
			if (msg == "true")
				std::cout << "message delivered" << endl;
			else
				std::cout << "error delivering message" << endl;

		}
		break;
		case '2':
		{
			command = "/m";

			string recepient_email = EmailRequest();
			if (recepient_email == "ALL")
			{
				std::cout << "Wrong email, try again" << endl;
				continue;
			}
			
			getline(cin, msg);

			command = command + " " + authUserLogin + " " + recepient_email + " " + msg;

			//strcpy(message, command.c_str());

			sendMessage(command);
			string msg;
			getMessage(msg);

			//string msg = message;
			if (msg == "true")
				std::cout << "message delivered" << endl;
			else
				std::cout << "error delivering message" << endl;

		}
		break;
		case '3':
		{
			command = "/rgm " + authUserLogin;

			//strcpy(message, command.c_str());

			sendMessage(command);
			string check;
			getMessage(check);

			//string check = message;

			if (check == "$")
			{
				getMessage(check);

				//check = message;
				while (check != "$")
				{
					std::cout << check << endl;
					getMessage(check);
					check = message;
				}
			}
		}
		break;
		case '4':
		{
			command = "/rpm " + authUserLogin;

			//strcpy(message, command.c_str());

			sendMessage(command);
			string check;
			getMessage(check);

			//string check = message;

			if (check == "$")
			{
				getMessage(check);
				//check = message;
				while (check != "$")
				{
					std::cout << check << endl;
					getMessage(check);
					//check = message;
				}
			}
		}
		break;
		case '5':
		{
			command = "/rul ";
			//strcpy(message, command.c_str());

			sendMessage(command);
			string check;
			getMessage(check);

			/*string check = message;*/

			if (check == "$")
			{
				getMessage(check);
				//check = message;
				while (check != "$")
				{
					std::cout << check << endl;
					getMessage(check);
					//check = message;
				}

			}
		}
		break;
		case '6':
		{
			command = "/ruol ";
			//strcpy(message, command.c_str());

			sendMessage(command);
			string check;
			getMessage(check);

			/*string check = message;*/

			if (check == "$")
			{
				getMessage(check);
				//check = message;
				while (check != "$")
				{
					std::cout << check << endl;
					getMessage(check);
					//check = message;
				}

			}
		}
		break;
		case '7':
		{
			command = "/del";
			command = command + " " + authUserLogin;

			//strcpy(message, command.c_str());

			sendMessage(command);
			string msg;
			getMessage(msg);

			//string msg = message;
			if (msg == "true")
			{
				std::cout << "account deleted" << endl;
				authUserLogin = "";
			}
			else
				std::cout << "account wasnt deleted" << endl;

		}
		break;
		case '8':
		{
			command = "/q " + authUserLogin;
			sendMessage(command);
			authUserLogin = "";
		}
		break;
		default:
			
			break;
		}

	} while (ch != '7' && ch != '8');
}
void MainMenu()
{
	command = "";
	char ch = 0;

	while (1) {
		memset(message, 0, sizeof(message));

		//DisplayValidUsers();
		Menu1();

		(std::cin >> ch).ignore();
		system("cls");

		switch (ch)
		{
		case '1':
		{
			char ch = ' ';
			command = "/reg";

			if (Registration(command) == 0)
			{
				continue;
			}

			//strcpy(message, command.c_str());
			
			sendMessage(command);
			string msg;
			getMessage(msg);

			//string msg = message;

			if (msg == "true")
			{
				std::cout << "Successful registration" << endl;
			}
			else
			{
				std::cout << "Registration failed" << endl;
			}
		}
		break;
		case '2':
		{
			command = "/auth";
			std::string login;

			if (Authorization(command, login) == 0)
				continue;

			//strcpy(message, command.c_str());

			sendMessage(command);

			string msg = "";
			
			getMessage(msg);
			//string msg = message;

			if (msg == "true")
			{
				authUserLogin = login;
				std::cout << "Successful authorization" << endl;
				ChatMenu();
			}
			else
			{
				std::cout << "Authorization failed" << endl;
			}
		}
		break;
		case '3':
			command = "end";
			//strcpy(message, command.c_str());

			sendMessage(command);

			std::cout << "Client Exit." << endl;
			break;
		default:
			continue;
		}

		//bzero(message, sizeof(message));
		if (command == "end")
			break;

		//ssize_t bytes = write(socket_file_descriptor, message, sizeof(message));
		// Если передали >= 0  байт, значит пересылка прошла успешно
		//if (bytes >= 0) {
		//	cout << "Data send to the server successfully.!" << endl;
		//}
		//bzero(message, sizeof(message));
		// Ждем ответа от сервера
		//read(socket_file_descriptor, message, sizeof(message));
		//cout << "Data received from server: " << message << endl;
	}

}   

bool socketInitAndBind()
{
#if defined _WIN32 || _WIN64
	 //Validate the parameters
	/*if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}*/

	 //Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	 //Resolve the server address and port
	//iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);

	iResult = getaddrinfo(IPaddr, PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	 //Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		 //Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		 //Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	return 0;
#elif defined __linux__

	socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_file_descriptor == -1) {
		cout << "Creation of Socket failed!" << endl;
		exit(1);
	}

	// Установим адрес сервера
	serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	// Зададим номер порта
	serveraddress.sin_port = htons(PORT);
	// Используем IPv4
	serveraddress.sin_family = AF_INET;
	// Установим соединение с сервером
	connection = connect(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
	if (connection == -1) {
		cout << "Connection with the server failed.!" << endl;
		exit(1);
	}
#endif
}

void closeSocket()
{
#if defined _WIN32 || _WIN64
	closesocket(ConnectSocket);
	WSACleanup();
#elif defined __linux__
	close(socket_file_descriptor);
#endif
}

int main()
{

	//printSystemInfo();
	
	if (socketInitAndBind() == 1)
	{
		std::cout << "SOCKET TROUBLE\n";
		return 1;
	}
	MainMenu();

	closeSocket();

	return 0;
}