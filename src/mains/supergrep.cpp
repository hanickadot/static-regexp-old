#include "regexp/regexp.hpp"
#include <cstdio>
#include <iostream>
#include <fstream> 

using namespace SRX;

#define INSERT_HERE Any

int main (int argc, char ** argv)
{
	RegularExpression< Selection<Str<'A','B','C','D','E'>,Str<'D','E','F','G','H'>,Str<'E','F','G','H','I'>,Repeat<4,0,Chr<'A'>>>
 > regexp;
	
	if (argc == 2)
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
	else if (argc >= 2)
	{
		for (int i = 1; i < argc; ++i)
		{
			std::string fname{std::string(argv[i])};
			std::ifstream ifs (fname, std::ifstream::in);
	
			char buffer[1024];
			//std::string line;
	
			while (ifs.getline(buffer,1024))
			{
				if (regexp(buffer))
				{
					std::cout << fname << ": " << buffer << '\n';
				}
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
