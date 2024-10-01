#include "Chat.h"

//int iSendResult;
//SOCKET ClientSocket = INVALID_SOCKET;


constexpr auto SQL_RESULT_LEN = 240;
constexpr auto SQL_RETURN_CODE_LEN = 1024;

SQLHANDLE sqlConnHandle{ nullptr }; // дескриптор для соединения с базой данных
SQLHANDLE sqlEnvHandle{ nullptr }; // дескриптор окружения базы данных
SQLHANDLE sqlStmtHandle{ nullptr };  // дескриптор для выполнения запросов к базе данных
SQLWCHAR retconstring[SQL_RETURN_CODE_LEN]{}; // строка для кода возврата из функций API ODBC


Chat::Chat() = default;
Chat::~Chat() = default;

std::string DBname = "chat2";

//#elif __linux__
//void Chat::SendUsersList(int con)
//{
//	char message[MESSAGE_LENGTH];
//	bzero(message, sizeof(message));
//
//	strcpy(message, "$");
//
//	ssize_t bytes = write(con, message, sizeof(message));
//
//	for (int i{ 0 }; i < base.getUsersCount(); i++)
//	{
//		strcpy(message, base.getUsersNameNLoginByIndex(i).c_str());
//		ssize_t bytes = write(con, message, sizeof(message));
//	}
//
//	strcpy(message, "$");
//	write(con, message, sizeof(message));
//
//}
//
//void Chat::SendGeneralMessages(int con)
//{
//	char message[MESSAGE_LENGTH];
//	bzero(message, sizeof(message));
//
//	Message tmp;
//
//	strcpy(message, "$");
//	ssize_t bytes = write(con, message, sizeof(message));
//
//	for (int i{ 0 }; i < m_base.getMessageCount(); i++)
//	{
//		tmp = m_base.getMessageByIndex(i);
//		if (tmp.getRecepientOfMessage() == "")
//		{
//			string t = "<" + tmp.getSenderOfMessage() + ">" + tmp.getMessage();
//			strcpy(message, t.c_str());
//			ssize_t bytes = write(con, message, sizeof(message));
//		}
//	}
//	strcpy(message, "$");
//	write(con, message, sizeof(message));
//
//}
//
//void Chat::SendPrivateMessages(int con, string login)
//{
//	char message[MESSAGE_LENGTH];
//	bzero(message, sizeof(message));
//
//	Message tmp;
//
//	strcpy(message, "$");
//	ssize_t bytes = write(con, message, sizeof(message));
//
//	for (int i{ 0 }; i < m_base.getMessageCount(); i++)
//	{
//		tmp = m_base.getMessageByIndex(i);
//		if ((tmp.getSenderOfMessage() == login && tmp.getRecepientOfMessage() != "") || (tmp.getRecepientOfMessage() == login))
//		{
//			string t = "<" + tmp.getSenderOfMessage() + "> <" + tmp.getRecepientOfMessage() + "> " + tmp.getMessage();
//			strcpy(message, t.c_str());
//			ssize_t bytes = write(con, message, sizeof(message));
//		}
//	}
//
//	strcpy(message, "$");
//	write(con, message, sizeof(message));
//}
//#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Chat::SetDataBaseConnection()
{
	cout << "Getting DBConnection settings from file...\n";
	std::fstream conf("DatabaseConnectionSettings.txt", std::ios_base::in);

	std::string config;
	getline(conf, config);
	cout << "Done\n";

	std::wstring c(config.begin(), config.end());

	conf.close();

	cout << "SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle): ";
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
		goto COMPLETED;
	cout << "sucess\nSQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0): ";
	if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
		goto COMPLETED;
	cout << "success\nSQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle): ";
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
		goto COMPLETED;
	cout << "sucess\n";

	cout << "Attempting connection to SQL Server...\n";

	// Устанавливаем соединение с сервером  
	switch (SQLDriverConnect(sqlConnHandle,
		GetDesktopWindow(),
		(SQLWCHAR*)c.c_str(),
		SQL_NTS,
		retconstring,
		1024,
		NULL,
		SQL_DRIVER_COMPLETE)) {


	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		cout << "Successfully connected to SQL Server\n";
		break;

	case SQL_INVALID_HANDLE:
	case SQL_ERROR:
		cout << "Could not connect to SQL Server\n";
		goto COMPLETED;

	default:
		break;
	}

	cout << "SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle): ";
	// Если соединение не установлено, то выходим из программы
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))
		goto COMPLETED;
	cout << "success\n";

	{
		string q = "use " + DBname;
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(q.begin(), q.end()).c_str(), SQL_NTS))
		{
			cout << "Database \"" << DBname << "\" doesn't exist.Creating DB and setting the environment...\n";
			createDatabase();
			cout << "Database successfully created are ready for work\n";
		}
		else
			cout << "Database \"" << DBname << "\" already exists\n";

		cout << "///////////////////////////////////////////////////////\n";
	}

	return true;
	// Закрываем соединение и выходим из программы
COMPLETED:
		cout << "failed\n";
		SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
		SQLDisconnect(sqlConnHandle);
		SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
		SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
		return false;
}
void Chat::CloseDataBaseConnection()
{

	//std::cout << getLastLogInput() << std::endl;

	SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
	SQLDisconnect(sqlConnHandle);
	SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
	SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
	cout << "Connection to Database closed" << endl;
}

bool Chat::createDatabase()
{
	string q = "create database " + DBname + ";";
	if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(q.begin(), q.end()).c_str(), SQL_NTS))
	{
		cout << "Couldn't create database\n";
		return false;
	}
	else
		cout << "Database \"" << DBname << "\" created successfully" << endl;

	q = "use " + DBname + ";";
	if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(q.begin(), q.end()).c_str(), SQL_NTS))
	{
		cout << "Couldn't use database\n";
		return false;
	}
	else
		cout << "Database \"" << DBname << "\" used successfully" << endl;
	
	if (creatingDBTables() == false)
	{
		cout << "Error while creating tables\n";
		return false;
	}
	else
		cout << "All tables created sucessfully\n";

	if (creatingDBProcedures() == false)
	{
		cout << "Error while creating procedures\n";
		return false;
	}
	else
		cout << "All procedures created sucessfully\n";

	if (creatingDBTriggers() == false)
	{
		cout << "Error while creating triggers\n";
		return false;
	}
	else
		cout << "All triggers created sucessfully\n";

	if (creatingDBViews() == false)
	{
		cout << "Error while creating views\n";
		return false;
	}
	else
		cout << "All views created sucessfully\n";

	SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)L"insert into users(second_name, name, email) values ('Deleted', 'Deleted', 'Deleted')", SQL_NTS);
	
}
bool Chat::creatingDBTables()
{
	// Table "users" 
	stringstream q;
	string tmp;
	{
		q << "create table users(id serial primary key, " <<
			"second_name varchar(200) not null, " <<
			"name varchar(200) not null, " <<
			"email varchar(200) not null, " <<
			"status varchar(7) not null default 'offline');";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating table \"users\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// Table "passwords"
	{
		q << "create table passwords(user_id integer references users(id), " <<
			"hash text);";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating table \"passwords\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// Table "messages"
	{
		q << "create table messages(id serial primary key, " <<
			"sender_id integer references users(id), " <<
			"recepient_id integer references users(id), " <<
			"text text, " <<
			"sent_at date, " <<
			"status integer not null default 0);";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating table \"messages\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	return true;
}
bool Chat::creatingDBProcedures()
{
	stringstream q;
	string tmp;
	// Procedure "adding_to_messages"
	{
		q << "create procedure adding_to_messages(s_id integer, r_id integer, _message text) " <<
		"begin " <<
		"insert into messages(sender_id, recepient_id, text, sent_at, status) values (s_id, r_id, _message, current_date(), 0); " <<
		"set @id = LAST_INSERT_ID(); " <<
		"set @stat = (select status from users where id = (select recepient_id from messages where id = @id)); " <<
		"set @comp := (select count(users.id) from users where (users.id <> s_id and users.status = 'online')); " <<
		"update messages " <<
		"set status = 1 " <<
		"where id = @id and @stat = 'online'; " <<
		"update messages " <<
		"set status = 1 " <<
		"where id = @id and recepient_id = 0 and @comp > 0; " <<
		"end;";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating procedure \"adding_to_messages\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// Procedure "adding_new_user"
	{
		q << "create procedure adding_new_user(_second_name varchar(200), _name varchar(200), _email varchar(200), _hash text) " <<
			"begin " <<
			"insert into users(second_name, name, email) values(_second_name, _name, _email); " <<
			"update passwords " <<
			"set hash = _hash " <<
			"where user_id = (select id from users where email = _email); " <<
			"end;";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating procedure \"adding_new_user\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// Procedure "check_private_messages"
	{
		q << "create procedure check_private_messages(c_id integer) " <<
			"begin " <<
			"select u1.email, u2.email, m.text, m.sent_at, m.status " <<
			"from messages as m " <<
			"join users as u1 on u1.id = m.sender_id " <<
			"join users as u2 on u2.id = m.recepient_id " <<
			"where m.recepient_id <> 0 and (u1.id = c_id or u2.id = c_id); " <<
			"update messages " <<
			"set status = 2 " <<
			"where recepient_id = c_id and status <> 2; " <<
			"end;";

		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating procedure \"check_private_messages\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// Procedure "check_general_messages"
	{
		q << "create procedure check_general_messages(c_id integer) " <<
			"begin " <<
			"select u.email, m.text, m.sent_at " <<
			"from messages as m " <<
			"join users as u on u.id = m.sender_id " <<
			"where m.recepient_id = 0; " <<
			"update messages " <<
			"set status = 2 " <<
			"where recepient_id = 0 and status <> 2 and sender_id <> c_id; " <<
			"end;";

		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating procedure \"check_general_messages\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// Procedure "users_authorization"
	{
		q << "create procedure users_authorization(_email varchar(200)) " <<
			"begin " <<
			"update users " <<
			"set status = 'online' " <<
			"where email = _email; " <<
			"update messages " <<
			"set status = 1 " <<
			"where ( ( recepient_id = (select id from users where email = _email) ) or (recepient_id = 0 and ( sender_id <> (select id from users where email = _email) ) ) and status = 0 ); " <<
			"end;";

		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating procedure \"users_authorization\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	return true;
}
bool Chat::creatingDBTriggers()
{
	stringstream q;
	string tmp;
	// Trigger "after_insert_on_users"
	{
		q << "create trigger after_insert_on_users after insert on users " <<
			"for each row begin " <<
			"insert into passwords(user_id) values (new.id); " <<
			"end;";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating trigger \"after_insert_on_users\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// Trigger "after_delete_on_users"
	{
		q << "create trigger after_delete_on_users before delete on users " <<
			"for each row begin " <<
			"delete from passwords where user_id = old.id; " <<
			"end;";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating trigger \"after_delete_on_users\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// Trigger "after_delete_on_users2"
	{
		q << "create trigger after_delete_on_users2 after delete on users " <<
			"for each row begin " <<
			"update messages " <<
			"set sender_id = 1 " <<
			"where sender_id = old.id; " <<
			"update messages " <<
			"set recepient_id = 1 " <<
			"where recepient_id = old.id; " <<
			"delete from messages where sender_id = 1 and recepient_id = 1; " <<
			"end;";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating trigger \"after_delete_on_users2\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	return true;
}
bool Chat::creatingDBViews()
{
	stringstream q;
	string tmp;
	// View "check_users_list"
	{
		q << "create view check_users_list as " <<
			"select u.second_name, u.name, u.email, u.status " <<
			"from users as u " <<
			"where u.id <> 1;";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating view \"check_users_list\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	// View "check_users_online_list"
	{
		q << "create view check_users_online_list as " <<
			"select u.second_name, u.name, u.email, u.status " <<
			"from users as u " <<
			"where u.id <> 1 and u.status = 'online';";
		tmp = q.str();
		q.str(std::string());
		q.clear();
		cout << "Creating view \"check_users_online_list\": ";
		if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(tmp.begin(), tmp.end()).c_str(), SQL_NTS))
		{
			cout << "fail\n";
			return false;
		}
		else
			cout << "success\n";
	}
	return true;
}
void Chat::deleteDatabase()
{
	string q = "drop database ";
	q += DBname;
	q += ";";
	if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wstring(q.begin(), q.end()).c_str(), SQL_NTS))
	{
		cout << "Couldn't delete database\n";
		return;
	}
	cout << "Database \"" << DBname << "\" deleted successfully" << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Chat::Identification(std::string Email)
{
	std::string q = "select id from users where email='" + Email + "';";
	std::wstring query(q.begin(), q.end());

	SQLCloseCursor(sqlStmtHandle);

	SQLExecDirect(sqlStmtHandle,(SQLWCHAR*)query.c_str(), SQL_NTS);

	SQLINTEGER id;

	SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &id, sizeof(id), nullptr);

	if (SQLFetch(sqlStmtHandle) != SQL_NO_DATA)
		return id;
	return -1;
}

bool Chat::Registration(std::string second_name, std::string name, std::string email, std::string pass)
{
	if (Identification(email) == -1)
	{
		//std::string hash_to_text = "";
		//for (int i = 0; i < 5; i++)
		//	hash_to_text = hash_to_text + std::to_string(pass[i]) + " ";

		std::string q = "call adding_new_user('" + second_name + "', '" + name + "', '" + email + "', '" + pass + "');";
		std::wstring query(q.begin(), q.end());
		SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);

		return true;
	}
	cout << "Email - " << email << " is occupied!" << endl;
	return false;
}

bool Chat::Authentification(std::string email, std::string pass)
{

	std::string q = "select * from passwords where user_id = (select id from users where email = '" + email + "');";
	std::wstring query(q.begin(), q.end());

	SQLCloseCursor(sqlStmtHandle);

	SQLRETURN rc;
	rc = SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);

	SQLLEN sql_str_length;

	SQLINTEGER V_OD_err, V_OD_id;
	SQLCHAR V_OD_buffer[200];

	V_OD_err = SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &V_OD_id, sizeof(V_OD_id), nullptr);
	V_OD_err = SQLBindCol(sqlStmtHandle, 2, SQL_CHAR, &V_OD_buffer, SQL_RESULT_LEN, &sql_str_length);

	SQLFetch(sqlStmtHandle);

	std::string comp(reinterpret_cast<char*>(V_OD_buffer));

	if (pass == comp)
		return true;
	return false;
}

bool Chat::Authorization(std::string email, std::string pass)
{
	if (Identification(email) != -1)
	{
		if (Authentification(email, pass) == true)
		{
			//std::string q = "update users set status = 'online' where email = '" + email + "';";
			std::string q = "call users_authorization('" + email + "');";
			std::wstring query(q.begin(), q.end());
			
			SQLCloseCursor(sqlStmtHandle);

			SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);


			return true;
		}
	}
	return false;
}

void Chat::QuitServer(std::string email)
{
	std::string q = " update users set status = 'offline' where email = '" + email + "'; ";
	std::wstring query(q.begin(), q.end());
	
	SQLCloseCursor(sqlStmtHandle);
	SQLExecDirect(sqlStmtHandle,(SQLWCHAR*)query.c_str(), SQL_NTS);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Chat::WriteMessage(std::string sender_email, std::string recepient_email, std::string message)
{
	int s_id = Identification(sender_email);
	int r_id;
	if (recepient_email == "ALL")
		r_id = 0;
	else
		r_id = Identification(recepient_email);
	

	if (r_id == -1)
		return false;

	std::string q = "call adding_to_messages(" + std::to_string(s_id) + ", " + std::to_string(r_id) + ", '" + message + "');";
	std::wstring query(q.begin(), q.end());

	//////////////////////////////// Logging message
	
	SYSTEMTIME time;
	GetLocalTime(&time);

	std::stringstream t;
	t << time.wYear << "-" << time.wMonth << "-" << time.wDay << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond;
	
	std::string s = "[" + t.str() + "] (" + std::to_string(s_id) + " -> " + std::to_string(r_id) + ") { " + message + " }";
	logs.inputToLogs(s);

	////////////////////////////////

	SQLCloseCursor(sqlStmtHandle);
	SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);

	return true;
}

bool Chat::DeleteFromChat(std::string email)
{
	int id = Identification(email);

	if (id > 1)
	{
		std::string q = "delete from users where id = " + std::to_string(id) + ";";
		std::wstring query(q.begin(), q.end());

		SQLCloseCursor(sqlStmtHandle);
		SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);
	
		return true;
	}
	return false;
}	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Chat::ReadGeneralMessages(std::string _email, SOCKET& clientSocket)
{
	std::string q = "call check_general_messages(" + std::to_string(Identification(_email)) + ");";
	std::wstring query(q.begin(), q.end());

	SQLCloseCursor(sqlStmtHandle);
	SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);

	SQLSMALLINT status;
	SQLCHAR email[200], msg[200], date[200];

	SQLBindCol(sqlStmtHandle, 1, SQL_CHAR, &email, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 2, SQL_CHAR, &msg, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 3, SQL_CHAR, &date, SQL_RESULT_LEN, nullptr);
	

	int iSendResult;
	char message[MESSAGE_LENGTH];

	memset(message, 0, sizeof(message));

	strcpy(message, "$");

	iSendResult = send(clientSocket, message, sizeof(message), 0);

	//std::string tmp;
	while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA)
	{
		stringstream tmp;
		tmp << "| " << setw(30) << reinterpret_cast<char*>(email) << " | " << setw(50) << reinterpret_cast<char*>(msg) << " | " << setw(10) << reinterpret_cast<char*>(date) << " |";

		strcpy(message, tmp.str().c_str());
		iSendResult = send(clientSocket, message, sizeof(message), 0);
	}

	strcpy(message, "$");
	iSendResult = send(clientSocket, message, sizeof(message), 0);

}

void Chat::ReadPrivateMessages(std::string email, SOCKET& clientSocket)
{
	std::string q = "call check_private_messages(" + std::to_string(Identification(email)) + ");";
	/*string ID = std::to_string(Identification(email));
	std::string q = "select u1.email, u2.email, m.text, m.sent_at, m.status from messages as m join users as u1 on u1.id = m.sender_id join users as u2 on u2.id = m.recepient_id where m.recepient_id != 0 and (u1.id = " + ID + " or u2.id = " + ID + ");";
	*/
	std::wstring query(q.begin(), q.end());

	SQLCloseCursor(sqlStmtHandle);
	SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);

	SQLCHAR sender_email[200], recepient_email[200], text[200], date[200];
	SQLSMALLINT status;

	SQLBindCol(sqlStmtHandle, 1, SQL_CHAR, &sender_email, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 2, SQL_CHAR, &recepient_email, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 3, SQL_CHAR, &text, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 4, SQL_CHAR, &date, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 5, SQL_SMALLINT, &status, sizeof(status), nullptr);

	int iSendResult;
	char message[MESSAGE_LENGTH];

	memset(message, 0, sizeof(message));
	strcpy(message, "$");

	iSendResult = send(clientSocket, message, sizeof(message), 0);

	while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA)
	{
		stringstream tmp;
		tmp << "| " << setw(30) << reinterpret_cast<char*>(sender_email) << " | " << setw(30) << reinterpret_cast<char*>(recepient_email) << " | " << setw(30) << reinterpret_cast<char*>(text) << " | " << setw(10) << reinterpret_cast<char*>(date) << " | " << status << " |";
		
		strcpy(message, tmp.str().c_str());
		iSendResult = send(clientSocket, message, sizeof(message), 0);
	}

	strcpy(message, "$");
	iSendResult = send(clientSocket, message, sizeof(message), 0);
}

void Chat::ShowAllUsers(SOCKET& clientSocket)
{
	std::string q = "select * from check_users_list;";
	std::wstring query(q.begin(), q.end());

	SQLCloseCursor(sqlStmtHandle);
	SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);

	SQLCHAR second_name[200], name[200], email[200], status[10];
	SQLBindCol(sqlStmtHandle, 1, SQL_CHAR, &second_name, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 2, SQL_CHAR, &name, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 3, SQL_CHAR, &email, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 4, SQL_CHAR, &status, SQL_RESULT_LEN, nullptr);

	int iSendResult;
	char message[MESSAGE_LENGTH];

	memset(message, 0, sizeof(message));
	strcpy(message, "$");

	iSendResult = send(clientSocket, message, sizeof(message), 0);

	while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA)
	{
		stringstream tmp;
		tmp << "| " << setw(30) << reinterpret_cast<char*>(second_name) << " | " << setw(30) << reinterpret_cast<char*>(name) << " | " << setw(30) << reinterpret_cast<char*>(email) << " | " << setw(7) << reinterpret_cast<char*>(status) << " |";

		strcpy(message, tmp.str().c_str());
		iSendResult = send(clientSocket, message, sizeof(message), 0);
	}

	strcpy(message, "$");
	iSendResult = send(clientSocket, message, sizeof(message), 0);

}

void Chat::ShowUsersOnline(SOCKET& clientSocket)
{
	std::string q = "select * from check_users_online_list";
	std::wstring query(q.begin(), q.end());

	SQLCloseCursor(sqlStmtHandle);
	SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS);

	SQLCHAR second_name[200], name[200], email[200], status[10];
	SQLBindCol(sqlStmtHandle, 1, SQL_CHAR, &second_name, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 2, SQL_CHAR, &name, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 3, SQL_CHAR, &email, SQL_RESULT_LEN, nullptr);
	SQLBindCol(sqlStmtHandle, 4, SQL_CHAR, &status, SQL_RESULT_LEN, nullptr);

	int iSendResult;
	char message[MESSAGE_LENGTH];

	memset(message, 0, sizeof(message));
	strcpy(message, "$");

	iSendResult = send(clientSocket, message, sizeof(message), 0);

	while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA)
	{
		stringstream tmp;
		tmp << "| " << setw(30) << reinterpret_cast<char*>(second_name) << " | " << setw(30) << reinterpret_cast<char*>(name) << " | " << setw(30) << reinterpret_cast<char*>(email) << " | " << setw(7) << reinterpret_cast<char*>(status) << " |";

		strcpy(message, tmp.str().c_str());
		iSendResult = send(clientSocket, message, sizeof(message), 0);
	}

	strcpy(message, "$");
	iSendResult = send(clientSocket, message, sizeof(message), 0);

}

std::string Chat::getLastLogInput()
{
	
	std::string tmp = logs.getLastLogString();

	return tmp;
}