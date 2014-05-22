#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRegExp2;

int main (int argc, char const *argv[])
{
	using Protocol = OneCatch<1, Plus<CRange<'a','z'>> >;
	using Domain = OneCatch<2, Plus<CRange<'a','z'>> >;
	using NextDomains = Star<Seq<Chr<0x2e>>, StaticCatch<3, 8, Plus<CRange<'a','z'>>>>;
	using Path = OneCatch<4, Star<Any>>;
	using HTTPCatch = RegularExpression<Begin, Protocol, Str<0x3a,0x2f,0x2f>, Domain, NextDomains, Chr<0x2f>, Path, End>;
	
	//RegularExpression< Begin, OneCatch<1, Plus<CRange<'a','z'>> >, Str<0x3a,0x2f,0x2f>, OneCatch<2, Plus<CRange<'a','z'>> >, Star<Seq<Chr<0x2e>>>, DynamicCatch<3, Plus<CRange<'a','z'>>>, Chr<0x2f>, OneCatch<4, Star<Any>>, End> regexp;
	
	class Blabla
	{
	};
	
	HTTPCatch regexp;
	
	//printf("sizeof(HTTPCatch) = %zu\n",sizeof(HTTPCatch));
	//printf("sizeof(Begin) = %zu\n",sizeof(Begin));
	//printf("sizeof(Sequence<Begin,End>) = %zu\n",sizeof(Sequence<Begin,End>));
	//printf("sizeof(Protocol) = %zu\n",sizeof(Protocol));
	//printf("sizeof(Plus<CRange<'a','z'>>) = %zu\n",sizeof(Plus<CRange<'a','z'>>));
	//printf("sizeof(NextDomains) = %zu\n",sizeof(NextDomains));
	//printf("sizeof(Str<0x3a,0x2f,0x2f>) = %zu\n",sizeof(Str<0x3a,0x2f,0x2f>));
	//printf("sizeof(Domain) = %zu\n",sizeof(Domain));
	if (argc >= 2)
	{
		if (regexp(argv[1]))
		{
			printf("yes:\n");
			unsigned int id{0};
			for (auto tmp: regexp.getCatch<0>()) printf("0.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<1>()) printf("1.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<2>()) printf("2.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<3>()) printf("3.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
			id = 0;
			for (auto tmp: regexp.getCatch<4>()) printf("4.%u: '%.*s'\n",id++,(int)tmp.length,argv[1]+tmp.begin);
		}
		else
		{
			printf("no\n");
		}
	}
	return 0;
}
