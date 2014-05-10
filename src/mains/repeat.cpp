#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRegExp;

int main (int argc, char const *argv[])
{
	StaticRegExp< Seq<Begin,OneCatch<1,Plus<CSet<'a','z'>>>,Chr<0x2d>,ReCatch<1>> > regexp;
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
