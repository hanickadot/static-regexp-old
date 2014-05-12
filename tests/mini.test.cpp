#include "regexp/regexp2.hpp"
#include <cstdio>

using namespace SRegExp2;

int main ()
{
	RegularExpression< Begin,Str<'a','b','c'>,End > regexp;
	if (regexp("abc")) return 0;
	else return 1;
}
