#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRegExp2;

int main (int argc, char const *argv[])
{
	RegularExpression<Begin, Plus<CSet<'a','z'>>, Str<':','/','/'>, CSet<'a','z'>, Plus<CSet<'a','z','0','9'>>, Star<Chr<'.'>, CSet<'a','z'>, Plus<CSet<'a','z','0','9'>> >, Chr<'/'> > regexp;
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
