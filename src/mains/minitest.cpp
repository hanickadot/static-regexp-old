#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRX;

int main (int argc, char const *argv[])
{
	RegularExpression<Begin, Plus<CRange<'a','z'>>, Str<':','/','/'>, CRange<'a','z'>, Plus<CRange<'a','z','0','9'>>, Star<Chr<'.'>, CRange<'a','z'>, Plus<CRange<'a','z','0','9'>> >, Chr<'/'> > regexp;
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
