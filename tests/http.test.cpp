#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRegExp;

int main ()
{
	StaticRegExp< Seq<Begin,Plus<CSet<'a','z'>>,Str<0x3a,0x2f,0x2f>,CSet<'a','z'>,Plus<CSet<0x30,0x39,'a','z'>, Star<Seq<Chr<0x2e>,CSet<'a','z'>,Plus<CSet<0x30,0x39,'a','z'>>>, Chr<0x2f>>>> > regexp;
	if (regexp("https://github.com/hanickadot/static-regexp"))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
