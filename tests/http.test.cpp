#include "regexp/test.hpp"
#include <cstdio>

using namespace SRX;

int main ()
{
	RegularExpression<Begin, Plus<CRange<'a','z'>>, Str<':','/','/'>, CRange<'a','z'>, Plus<CRange<'a','z','0','9'>>, Star<Chr<'.'>, CRange<'a','z'>, Plus<CRange<'a','z','0','9'>> >, Chr<'/'> > regexp;
	if (regexp("https://github.com/hanickadot/static-regexp"))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
