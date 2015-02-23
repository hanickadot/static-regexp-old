#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRX;

int main ()
{
	RegularExpression< Begin,Str<'a','b','c'>,End > regexp;
	if (regexp("abc")) return 0;
	else return 1;
}
