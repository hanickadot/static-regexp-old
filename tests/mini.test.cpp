#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRegExp;

int main ()
{
	StaticRegExp< Seq<Begin,Str<'a','b','c'>,End> > regexp;
	if (regexp("abc")) return 0;
	else return 1;
}
