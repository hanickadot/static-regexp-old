#include "regexp/regexp.hpp"
#include <cstdio>

using namespace SRX;

int main (int argc, char const *argv[])
{
    // you can combine sub-regexp into one
    using Protocol = Plus<CRange<'a','z'>>;
    using Domain = Seq<CRange<'a','z'>, Plus<CRange<'a','z','0','9'>>>;
    RegularExpression<Begin, Protocol, Str<':','/','/'>, Domain, Star<Chr<'.'>, Domain >, Chr<'/'> > regexp;
    if (argc >= 2)
    {
        if (regexp(argv[1]))
        {
            printf("yes\n");
        }
        else
        {
            printf("no\n");
        }
    }
    return 0;
}