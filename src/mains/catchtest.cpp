#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRegExp;

#define INSERT_HERE Any

int main (int argc, char const *argv[])
{
	StaticRegExp< Seq<Begin,StaticCatch<1,1, Plus<CSet<'a','z'>>>,Str<0x3a,0x2f,0x2f>,StaticCatch<2,1, Plus<CSet<'a','z'>>>,Star<Seq<Chr<0x2e>,DynamicCatch<3, Plus<CSet<'a','z'>>>>, Seq<Chr<0x2f>,StaticCatch<4,1, Star<Any>>,End>>> > regexp;
	if (argc >= 2)
	{
		if (regexp.match(argv[1]))
		{
			printf("yes:\n");
			unsigned int id{0};
			for (auto tmp: regexp.get<0>()) printf("0.%u: '%.*s'\n",id++,(int)tmp.size,argv[1]+tmp.start);
			id = 0;
			for (auto tmp: regexp.get<1>()) printf("1.%u: '%.*s'\n",id++,(int)tmp.size,argv[1]+tmp.start);
			id = 0;
			for (auto tmp: regexp.get<2>()) printf("2.%u: '%.*s'\n",id++,(int)tmp.size,argv[1]+tmp.start);
			id = 0;
			for (auto tmp: regexp.get<3>()) printf("3.%u: '%.*s'\n",id++,(int)tmp.size,argv[1]+tmp.start);
			id = 0;
			for (auto tmp: regexp.get<4>()) printf("4.%u: '%.*s'\n",id++,(int)tmp.size,argv[1]+tmp.start);
		}
		else
		{
			printf("no\n");
		}
	}
	return 0;
}
