#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRX;

int main (int argc, char const *argv[])
{
	RegularExpression<Begin, Sel< Seq<Id<1,1>, Chr<'a'>,Chr<'b'>,Chr<'c'>>, Seq<Id<1,2>, Chr<'d'>,Chr<'e'>,Chr<'f'>> > > regexp;
	if (argc >= 2)
	{
		if (regexp(argv[1]))
		{
			printf("yes: %d\n", regexp.getIdentifier<1>());
		}
		else
		{
			printf("no\n");
		}
	}
	return 0;
}
