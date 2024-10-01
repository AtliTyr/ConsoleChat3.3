#include "Logger.h"

Logger::Logger()
{
	logs.open("log.txt", std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
}

Logger::~Logger()
{
	logs.close();
}

void Logger::inputToLogs(std::string text)
{
    m.lock();

    logs.seekg(0, std::ios_base::end);

    logs << "\n" << text;

    m.unlock();
}

std::string Logger::getLastLogString()
{
    m.lock_shared();

    logs.seekg(-1, std::ios_base::end);                

    bool keep = true;
    while (keep) {
        char ch;
        logs.get(ch);                            

        if ((int)logs.tellg() <= 1) {           
            logs.seekg(0);                       
            keep = false;               
        }
        else if (ch == '\n') {               
            keep = false;                
        }
        else {                                 
            logs.seekg(-2, std::ios_base::cur);        
        }
    }

    std::string lastLine;
    std::getline(logs, lastLine);                      

    m.unlock_shared();

	return std::string(lastLine);
}





