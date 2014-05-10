#ifndef __REGEXP__ABSTRACTION__HPP__
#define __REGEXP__ABSTRACTION__HPP__

template <typename BaseType> struct StringAbstraction;

template <typename CharType> struct CharacterAbstraction
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
		return *str == c;
	}
	inline bool equalToOriginal(const size_t pos, const size_t vec) const {
		return *(original+pos) == *(str+vec);
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

template <> struct StringAbstraction<const char *>: public CharacterAbstraction<char> {
	inline StringAbstraction(const CharacterAbstraction && orig): CharacterAbstraction{orig} { }
	inline StringAbstraction(const char * lstr): CharacterAbstraction{lstr} { }
	inline StringAbstraction add(size_t c) const {
		return static_cast<StringAbstraction>(CharacterAbstraction::add(c));
	}
};
template <> struct StringAbstraction<char *>: public CharacterAbstraction<char> {
	inline StringAbstraction(const CharacterAbstraction && orig): CharacterAbstraction{orig} { }
	inline StringAbstraction(const char * lstr): CharacterAbstraction{lstr} { }
	inline StringAbstraction add(size_t c) const {
		return static_cast<StringAbstraction>(CharacterAbstraction::add(c));
	}
};
template <> struct StringAbstraction<const wchar_t *>: public CharacterAbstraction<wchar_t> {
	inline StringAbstraction(const CharacterAbstraction && orig): CharacterAbstraction{orig} { }
	inline StringAbstraction(const wchar_t * lstr): CharacterAbstraction{lstr} { }
	inline StringAbstraction add(size_t c) const {
		return static_cast<StringAbstraction>(CharacterAbstraction::add(c));
	}
};
template <> struct StringAbstraction<wchar_t *>: public CharacterAbstraction<wchar_t> {
	inline StringAbstraction(const CharacterAbstraction && orig): CharacterAbstraction{orig} { }
	inline StringAbstraction(const wchar_t * lstr): CharacterAbstraction{lstr} { }
	inline StringAbstraction add(size_t c) const {
		return static_cast<StringAbstraction>(CharacterAbstraction::add(c));
	}
};

#endif
