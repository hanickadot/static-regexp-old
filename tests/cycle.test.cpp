#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRegExp2;

bool plus()
{
	RegularExpression< Begin,Plus<CRange<'a','z'>>,Chr<'0'>,End> regexp;
	if (!regexp("aa0")) return false;
	if (!regexp("a0")) return false;
	if (!regexp("azz0")) return false;
	if (!regexp("0")) return true;
	return false;
}

bool star()
{
	RegularExpression< Begin,Star<CRange<'a','z'>>,Chr<'0'>,End> regexp;
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
