#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRegExp2;

int main ()
{
	RegularExpression<Begin, Plus<CSet<'a','z'>>, Str<':','/','/'>, CSet<'a','z'>, Plus<CSet<'a','z','0','9'>>, Star<Chr<'.'>, CSet<'a','z'>, Plus<CSet<'a','z','0','9'>> >, Chr<'/'> > regexp;
	if (regexp("https://github.com/hanickadot/static-regexp"))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
