#include <iostream>
#include <functional>

#include "translator/translate.hpp"

int main (int argc, char const *argv[])
{
	std::string line;
	
	bool ns{false};
	if (argc >= 2 && std::string(argv[1]) == "ns")
	{
		ns = true;
	}
	
	while (getline(std::cin,line))
	{
		std::cout << RegExpCompiler::translate(line,ns) << '\n';
	}
	
	return 0;
}
