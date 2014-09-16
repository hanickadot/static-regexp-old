#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRX;

int main (int argc, char const *argv[])
{
	RegularExpression< Begin, OneCatch<1,Plus<CRange<'a','z'>>>, ReCatch<1>, End > regexp;
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
