#include "regexp/regexp2.hpp"
#include <cstdio>
#include <iostream>
#include <fstream> 

using namespace SRegExp2;

#define INSERT_HERE Any

int main (int argc, char ** argv)
{
	RegularExpression< INSERT_HERE > regexp;
	
	if (argc >= 2)
	{
	
		std::ifstream ifs (std::string(argv[1]), std::ifstream::in);
	
		char buffer[1024];
		//std::string line;
	
		while (ifs.getline(buffer,1024))
		{
			if (regexp(buffer))
			{
				std::cout << buffer << '\n';
			}
		}
	}
	else
	{
		std::string line;
	
		while (getline(std::cin,line))
		{
			if (regexp(line))
			{
				std::cout << line << '\n';
			}
		}
	}
	
	return 0;
}
