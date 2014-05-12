#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRegExp2;

int main (int argc, char const *argv[])
{
	using Protocol = OneCatch<1, Plus<CSet<'a','z'>> >;
	using Domain = OneCatch<2, Plus<CSet<'a','z'>> >;
	using NextDomains = Star<Seq<Chr<0x2e>>, StaticCatch<3,8, Plus<CSet<'a','z'>>>>;
	using Path = OneCatch<4, Star<Any>>;
	using HTTPCatch = RegularExpression<Begin, Protocol, Str<0x3a,0x2f,0x2f>, Domain, NextDomains, Chr<0x2f>, Path, End>;
	
	//RegularExpression< Begin, OneCatch<1, Plus<CSet<'a','z'>> >, Str<0x3a,0x2f,0x2f>, OneCatch<2, Plus<CSet<'a','z'>> >, Star<Seq<Chr<0x2e>>>, DynamicCatch<3, Plus<CSet<'a','z'>>>, Chr<0x2f>, OneCatch<4, Star<Any>>, End> regexp;
	
	HTTPCatch regexp;
	
	if (argc >= 2)
	{
		if (regexp(argv[1]))
		{
			printf("yes:\n");
			unsigned int id{0};
			for (auto tmp: regexp.get<0>()) printf("0.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.get<1>()) printf("1.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.get<2>()) printf("2.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.get<3>()) printf("3.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.get<4>()) printf("4.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
		}
		else
		{
			printf("no\n");
		}
	}
	return 0;
}
