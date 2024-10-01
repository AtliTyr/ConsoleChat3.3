#include "Functions.h"

bool Registration(string& command)
{
	cout << "Register" << endl;
	char ch = ' ';

	/*do
	{*/
	cout << "To exit, enter '0': ";
	//ch = getch();
	(cin >> ch).ignore();
	if (ch == '0')
	{
		command = "";
		return 0;
	}
	cout << "\033[2J\033[1;1H";

	string second_name, name, email, password;
	cout << "Second_name: (Available symbols: A-Z, a-z, '-', '.', ' ')"; cin >> second_name;
	cout << "Name: (Available symbols: A-Z, a-z, '-', '.', ' ')"; cin >> name;
	cout << "Email: (Available symbols: A-Z, a-z, '0-9', '.', '@')"; cin >> email;
	cout << "Password: (Min length: 1, Max length - 16)"; cin >> password;
	

	if(second_name.length() <= 0 || second_name.length() >= 200)
	{
		cout << "Registration failed(Incorrect second name length)\n";
		return false;
	}
	if (name.length() <= 0 || name.length() >= 200)
	{
		cout << "Registration failed(Incorrect name length)\n";
		return false;
	}
	if (email.length() <= 0 || email.length() >= 200)
	{
		cout << "Registration failed(Incorrect email length)\n";
		return false;
	}
	if (password.length() <= 0 || password.length() >= 17)
	{
		cout << "Registration failed(Incorrect password length)\n";
		return false;
	}

	for (std::string::iterator it = second_name.begin(); it != second_name.end(); it++)
	{
		if (*it < 'A' || (*it > 'Z' && *it < 'a') || *it > 'z')
			if (!(*it == '-' || *it == '.' || *it == ' '))
			{
				cout << "Registration failed(Incorrect second name)\n";
				return false;
			}
	}
	for (std::string::iterator it = name.begin(); it != name.end(); it++)
	{
		if (*it < 'A' || (*it > 'Z' && *it < 'a') || *it > 'z')
			if (!(*it == '-' || *it == '.' || *it == ' '))
			{
				cout << "Registration failed(Incorrect name)\n";
				return false;
			}
	}
	for (std::string::iterator it = second_name.begin(); it != second_name.end(); it++)
	{
		if( !( (*it >= '@' && *it <= 'Z') || (*it >= '0' && *it <= '9') || (*it >= 'a' && *it <= 'z') || (*it == '.') || (*it == '@') ) )
		{
			cout << "Registration failed(Incorrect email)\n";
			return false;
		}
	}

	char* h = new char[password.length() + 1];
	std::strcpy(h, password.c_str());

	uint* pwd = sha1(h, password.length());

	command = command + " " + second_name + " " + name + " " + email + " ";

	for (int i = 0; i < 5; i++)
		command = command + std::to_string(pwd[i]) + " ";

	return 1;
}
bool Authorization(string& command, string& email)
{
	std::string password;
	//std::string login;
	char ch;

	std::cout << "Log in" << endl;
	std::cout << "To exit, enter '0': ";

	(std::cin >> ch).ignore();
	if (ch == '0')
	{
		command = "";
		return 0;
	}

	cout << "\033[2J\033[1;1H";

	std::cout << "Enter email: ";
	std::cin >> email;
	std::cout << "Enter password: ";
	std::cin >> password;

	if (email.length() <= 0 || email.length() >= 200)
	{
		cout << "Authorization failed(Incorrect email length)\n";
		return false;
	}
	if (password.length() <= 0 || password.length() >= 17)
	{
		cout << "Authorization failed(Incorrect password length)\n";
		return false;
	}

	char* pwd = new char[password.length() + 1];
	std::strcpy(pwd, password.c_str());

	uint* pass = sha1(pwd, password.length());

	command = command + " " + email + " ";

	for (int i = 0; i < 5; i++)
	{
		command = command + std::to_string(pass[i]) + " ";
	}



	return 1;
}

void DisplayValidUsers()
{
	cout << "Available users: " << endl
		<< "login: admin		login: A" << endl
		<< "password: admin		password: A" << endl;
}
std::string EmailRequest()
{
	string email;
	cout << "Recipient's email: ";
	getline(cin, email);
	return email;
}

void Menu1()
{

	cout << "#####################" << endl
		<< "1: Register" << endl
		<< "2: Enter the chat" << endl
		<< "3: Exit" << endl
		<< "#####################" << endl
		<< ">";

}
void Menu2()
{
	cout << "\n 1 - Write a message to the general chat " << endl
		<< " 2 - Write a private message " << endl
		<< " 3 - Read general messages " << endl
		<< " 4 - Read private messages " << endl
		<< " 5 - View all users " << endl
		<< " 6 - View all users online " << endl
		<< " 7 - Delete account " << endl
		<< " 8 - Leave chat " << endl
		<< " >";
}

void printSystemInfo()
{
#if defined _WIN32 || _WIN64
	system("chcp 1251 > nul & systeminfo > systeminfo.txt");
	std::fstream OCinfo("systeminfo.txt", std::ios::in);

	string OCname, OCversion;

	getline(OCinfo, OCname); // Just for moving file pointer
	getline(OCinfo, OCname);

	getline(OCinfo, OCname);
	getline(OCinfo, OCversion);

	cout << OCname << endl
		<< OCversion << endl;

	OCinfo.close();
#elif defined __linux__
	struct utsname utsname; // объект для структуры типа utsname

	uname(&utsname); // передаем объект по ссылке

	// распечатаем на экране информацию об операционной системе
	// эту информацию нам вернуло ядро Linux в объекте utsname
	cout << "OS name: " << utsname.sysname << endl;
	cout << "Host name: " << utsname.nodename << endl;
	cout << "OS release: " << utsname.release << endl;
	cout << "OS version: " << utsname.version << endl;
	cout << "Architecture: " << utsname.machine << endl;
#elif
	cout << "OS is not identified" << endl;
#endif
}