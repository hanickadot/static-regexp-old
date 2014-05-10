#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRegExp;

bool plus()
{
	StaticRegExp< Seq<Begin,Plus<CSet<'a','z'>, Seq<Chr<0x30>,End>>> > regexp;
	if (!regexp("aa0")) return false;
	if (!regexp("a0")) return false;
	if (!regexp("azz0")) return false;
	if (!regexp("0")) return true;
	return false;
}

bool star()
{
	StaticRegExp< Seq<Begin,Star<CSet<'a','z'>, Seq<Chr<0x30>,End>>> > regexp;
	if (!regexp("aa0")) return false;
	if (!regexp("a0")) return false;
	if (!regexp("azz0")) return false;
	if (!regexp("0")) return false;
	return true;
}

int main ()
{
	if (plus() && star()) return 0;
	return 1;
}
