#ifndef __REGEXP__REGEXP__HPP__
#define __REGEXP__REGEXP__HPP__

#include <string>

// ideally SRegExp<"ahoj|cau$">::smatch

namespace SRegExp {
template <char c> struct Chr {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str == c) {
			pos++;
			return true;
		} else return false;
	}
};

template <wchar_t c> struct WChr {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str == c) {
			pos++;
			return true;
		} else return false;
	}
};

template <char a, char b, char... rest> struct CSet {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (matchCharacter(*str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return (u >= a && u <= b) || CSet<rest...>::matchCharacter(u); }
};

template <wchar_t a, wchar_t b, wchar_t... rest> struct WSet {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (matchCharacter(*str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return (u >= a && u <= b) || CSet<rest...>::matchCharacter(u); }
};

template <char a, char b> struct CSet<a, b> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str >= a && *str <= b) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return (u >= a && u <= b); }
};

template <wchar_t a, wchar_t b> struct WSet<a, b> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str >= a && *str <= b) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return (u >= a && u <= b); }
};

template <char a, char b, char... rest> struct CNegSet {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (matchCharacter(*str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return !(u >= a && u <= b) && CNegSet<rest...>::matchCharacter(u); }
};

template <wchar_t a, wchar_t b, wchar_t... rest> struct WNegSet {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (matchCharacter(*str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return !(u >= a && u <= b) && CNegSet<rest...>::matchCharacter(u); }
};

template <char a, char b> struct CNegSet<a, b> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (!(*str >= a && * str <= b)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return !(u >= a && u <= b); }
};

template <wchar_t a, wchar_t b> struct WNegSet<a, b> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (!(*str >= a && * str <= b)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return !(u >= a && u <= b); }
};

template <typename Inner, typename... rest> struct Seq {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{pos};
		if (Inner::smatch(str, tmp, cpos, deep) && Seq<rest...>::smatch(str + tmp - pos, tmp, cpos + tmp, deep)) {
			pos = tmp;
			return true;
		} else return false;
	}
};

template <typename Inner> struct Seq<Inner> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		return Inner::smatch(str, pos, cpos, deep);
	}
};

template <char c, char... rest> struct Str {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{pos};
		if (Chr<c>::smatch(str, tmp, cpos, deep) && Str<rest...>::smatch(str + tmp - pos, tmp, cpos + tmp, deep)) {
			pos = tmp;
			return true;
		} else return false;
	}
};

template <char c> struct Str<c> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		return Chr<c>::smatch(str, pos, cpos, deep);
	}
};

template <typename Inner, typename... rest> struct Sel {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		if (Inner::smatch(str, pos, cpos, deep)) return true;
		else return Sel<rest...>::smatch(str, pos, cpos, deep);
	}
};

template <typename Inner> struct Sel<Inner> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		return Inner::smatch(str, pos, cpos, deep);
	}
};

struct Any {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str) {
			pos++;
			return true;
		} else return false;
	}
};

template <typename Inner, typename... rest> struct Star {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (Seq<rest...>::smatch(str, tmp, cpos, deep)) {
			pos += tmp;
			return true;
		} else if (Inner::smatch(str + tmp, tmp, cpos, deep)) {
			for (;;) if (Seq<rest...>::smatch(str + tmp, tmp, cpos + tmp, deep)) {
				pos += tmp;
				return true;
			} else if (!Inner::smatch(str + tmp, tmp, cpos + tmp, deep)) return false;
		} else return false;
	}
};

template <typename Inner> struct Star<Inner> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (Inner::smatch(str + tmp, tmp, cpos, deep)) {
			for (;;) if (!Inner::smatch(str + tmp, tmp, cpos + tmp, deep)) {
				pos += tmp;
				return true;
			}
		} return true;
	}
};

template <typename Inner, typename... rest> struct Plus {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (Inner::smatch(str + tmp, tmp, cpos, deep)) {
			for (;;) if (Seq<rest...>::smatch(str + tmp, tmp, cpos + tmp, deep)) {
				pos += tmp;
				return true;
			} else if (!Inner::smatch(str + tmp, tmp, cpos + tmp, deep)) return false;
		}
		return false;
	}
};

template <typename Inner> struct Plus<Inner> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (Inner::smatch(str + tmp, tmp, cpos, deep))
			for (;;) if (!Inner::smatch(str + tmp, tmp, cpos + tmp, deep)) {
				pos += tmp;
				return true;
			}
		return false;
	}
};

template <typename Inner, typename... rest> struct Opt {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (Seq<rest...>::smatch(str, pos, cpos, deep + 1))
			return true;
		else if (Inner::smatch(str, tmp, cpos, deep + 1) && Seq<rest...>::smatch(str + tmp, tmp, cpos + tmp, deep + 1)) {
			pos += tmp;
			return true;
		}
		return false;
	}
};

struct End {
	template <typename chartype> static inline bool smatch(const chartype * str, size_t &, size_t, size_t) {
		return (!* str);
	}
};

struct Begin {
	template <typename chartype> static inline bool smatch(const chartype *, size_t &, size_t cpos, size_t) {
		return cpos == 0;
	}
};

template <typename... rest> struct Eat {
	template <typename chartype> static inline bool smatch(const chartype * str, size_t &, size_t cpos, size_t deep) {
		size_t tmp{0};
		while (*(str + tmp)) {
			if (Seq<rest...>::smatch(str + tmp, tmp, cpos + tmp, deep)) return true;
			else tmp++;
		}
		return false;
	}
};

template <typename... rest> bool sregexp_smatch(const wchar_t * str) {
	size_t tmp{0};
	return Eat<rest...>::smatch(str, tmp, tmp, 0);
}

template <typename... rest> bool sregexp_smatch(std::wstring str) {
	size_t tmp{0};
	return Eat<rest...>::smatch(str.c_str(), tmp, tmp, 0);
}

template <typename... rest> bool sregexp_smatch(const char * str) {
	size_t tmp{0};
	return Eat<rest...>::smatch(str, tmp, tmp, 0);
}

template <typename... rest> bool sregexp_smatch(std::string str) {
	size_t tmp{0};
	return Eat<rest...>::smatch(str.c_str(), tmp, tmp, 0);
}

template <typename... rest> class StaticRegExp {
protected:
	Eat<rest...> definition;

public:
	bool operator()(const wchar_t * str) {
		size_t tmp{0};
		return definition.smatch(str, tmp, tmp, 0);
	}
	bool operator()(const std::wstring &str) {
		size_t tmp{0};
		return definition.smatch(str.c_str(), tmp, tmp, 0);
	}
	bool operator()(const char * str) {
		size_t tmp{0};
		return definition.smatch(str, tmp, tmp, 0);
	}
	bool operator()(const std::string &str) {
		size_t tmp{0};
		return definition.smatch(str.c_str(), tmp, tmp, 0);
	}
};
}

#endif
