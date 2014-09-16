// The MIT License (MIT)
// 
// Copyright (c) 2014 Hana Dusíková (hanicka@hanicka.net)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef __REGEXP__ABSTRACTION__HPP__
#define __REGEXP__ABSTRACTION__HPP__

namespace SRX {

template <typename BaseType, typename CharType, bool (*compare)(CharType, CharType, CharType)> struct StringAbstraction;

template <typename CharType> inline bool charactersAreEqual(CharType a, CharType min, CharType max)
{
	return a >= min && a <= max;
}

inline bool caseSensitive(const char a, const char min, const char max)
{
	return a >= min && a <= max;
}

inline bool caseInsensitive(const char a, const char min, const char max)
{
	char pom = (a >= 'a' && a <= 'z') ? a - 0x20 : a;	
	return (pom >= ((min >= 'a' && min <= 'z') ? min - 0x20 : min) && pom <= ((max >= 'a' && max <= 'z') ? max - 0x20 : max)); 
}

template <typename CharType, bool (*equalFnc)(CharType,CharType,CharType) = charactersAreEqual> struct CharacterAbstraction
{
	//const size_t cpos;
	const CharType * str;
	const CharType * original;
	inline CharacterAbstraction(const CharType * lstr, const CharType * lstrorig): str{lstr}, original{lstrorig} {}
	inline CharacterAbstraction(const CharType * lstr): str{lstr}, original{lstr} {}
	inline bool exists(const size_t pos) const {
		return *(str+pos);
	}
	inline bool exists() const {
		return *(str);
	}
	inline CharacterAbstraction add(size_t c) const {
		//printf("add: %p, %p\n", str+c,original);
		return CharacterAbstraction{str+c,original};
	}
	template <typename CharTypeInner> inline bool equal(const CharTypeInner c) const {
		return equalFnc(*str,c,c);
	}
	inline bool equalToOriginal(const size_t pos, const size_t vec) const {
		return equalFnc(*(original+pos),*(str+vec),*(str+vec));
	}
	template <typename CharTypeInner> inline bool charIsBetween(const CharTypeInner a, const CharTypeInner b) const {
		return (*str >= a) && (*str <= b);
	}
	inline bool isBegin() const {
		return getPosition() == 0;
	}
	inline bool isEnd() const {
		return !*str;
	}
	inline size_t getPosition() const {
		return str-original;
	}
};

template <bool (*compare)(const char, const char, const char)> struct StringAbstraction<const char *, const char, compare>: public CharacterAbstraction<char, compare> {
	inline StringAbstraction(const CharacterAbstraction<char, compare> && orig): CharacterAbstraction<char, compare>{orig} { }
	inline StringAbstraction(const char * lstr): CharacterAbstraction<char, compare>{lstr} { }
	inline StringAbstraction add(size_t c) const {
		return static_cast<StringAbstraction>(CharacterAbstraction<char, compare>::add(c));
	}
};
template <bool (*compare)(const char, const char, const char)> struct StringAbstraction<char *, const char, compare>: public CharacterAbstraction<char, compare> {
	inline StringAbstraction(const CharacterAbstraction<char, compare> && orig): CharacterAbstraction<char, compare>{orig} { }
	inline StringAbstraction(const char * lstr): CharacterAbstraction<char, compare>{lstr} { }
	inline StringAbstraction add(size_t c) const {
		return static_cast<StringAbstraction>(CharacterAbstraction<char, compare>::add(c));
	}
};
template <bool (*compare)(const wchar_t, const wchar_t, const wchar_t)> struct StringAbstraction<const wchar_t *, const wchar_t, compare>: public CharacterAbstraction<wchar_t, compare> {
	inline StringAbstraction(const CharacterAbstraction<wchar_t, compare> && orig): CharacterAbstraction<wchar_t, compare>{orig} { }
	inline StringAbstraction(const wchar_t * lstr): CharacterAbstraction<wchar_t, compare>{lstr} { }
	inline StringAbstraction add(size_t c) const {
		return static_cast<StringAbstraction>(CharacterAbstraction<wchar_t, compare>::add(c));
	}
};
template <bool (*compare)(const wchar_t, const wchar_t, const wchar_t)> struct StringAbstraction<wchar_t *, const wchar_t, compare>: public CharacterAbstraction<wchar_t, compare> {
	inline StringAbstraction(const CharacterAbstraction<wchar_t, compare> && orig): CharacterAbstraction<wchar_t, compare>{orig} { }
	inline StringAbstraction(const wchar_t * lstr): CharacterAbstraction<wchar_t, compare>{lstr} { }
	inline StringAbstraction add(size_t c) const {
		return static_cast<StringAbstraction>(CharacterAbstraction<wchar_t, compare>::add(c));
	}
};

}

#endif
