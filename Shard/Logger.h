#pragma once

#include <iostream>
#include <Windows.h>

namespace Shard
{
    class Logger
    {
    public:
        static void Log(std::string message)
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, 11);
            std::cout << "[Shard] ";
            SetConsoleTextAttribute(hConsole, 15);
            std::cout << message << std::endl;
        }
    };
}
