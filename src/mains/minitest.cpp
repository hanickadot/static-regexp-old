#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRegExp;

#define INSERT_HERE Any

int main (int argc, char const *argv[])
{
	StaticRegExp< INSERT_HERE > regexp;
	if (argc >= 2)
	{
		if (regexp(argv[1]))
		{
			printf("yes\n");
		}
		else
		{
			printf("no\n");
		}
	}
	return 0;
}
