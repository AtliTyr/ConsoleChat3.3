#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <shared_mutex>

class Logger
{
private:
	std::fstream logs;
	std::shared_mutex m;
public:
	Logger();
	~Logger();

	void inputToLogs(std::string text);
	std::string getLastLogString();
};

