#pragma once
#include <iostream>
//#ifdef _WIN32 || _WIN64
//#include  "lExceptions/ExceptionHandling.h"
//#elif __linux__
//#include "ExceptionHandling.h"	
//#endif
#include "Chat.h"
#include <iostream>
#include <string>

extern Chat chat1;

using namespace std;

void prefixDetection(string& pref, string& tmp);

#if defined _WIN32 || _WIN64
void commandProcessing(string pref, string tmp, bool& success, SOCKET ClientSocket);
#elif defined __linux__
void commandProcessing(string pref, string tmp, bool& success, int connection);
#endif