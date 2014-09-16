#include <regex>
#include <cstdio>
#include <iostream>
#include <fstream> 

//using namespace SRX;

#define INSERT_HERE Any

int main (int argc, char ** argv)
{
	auto regexp = std::regex("ABCDE|DEFGH|EFGHI|AAAA+");
	
	//RegularExpression< Str<'A','B','C','D'> > regexp;
	
	if (argc >= 2)
	{
	
		std::ifstream ifs (std::string(argv[1]), std::ifstream::in);
	
		char buffer[1024];
		//std::string line;
	
		while (ifs.getline(buffer,1024))
		{
			if (std::regex_search(buffer, regexp))
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
			if (std::regex_search(line, regexp))
			{
				std::cout << line << '\n';
			}
		}
	}
	
	return 0;
}
