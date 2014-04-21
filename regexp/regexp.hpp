#ifndef __REGEXP__REGEXP__HPP__
#define __REGEXP__REGEXP__HPP__

#include <string>
#include <vector>

// ideally SRegExp<"ahoj|cau$">::smatch

#include "abstraction.hpp"
#include "catching.hpp"

#define INSERT_HERE Any

namespace SRegExp {

template <char c> struct Chr {
	template <typename BaseType> static inline bool match(const StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (str.equal(c)) {
			pos++;
			return true;
		} else return false;
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t c> struct WChr {
	template <typename BaseType> static inline bool match(const StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (str.equal(c)) {
			pos++;
			return true;
		} else return false;
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <char a, char b, char... rest> struct CSet {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (matchCharacter(str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename BaseType> static inline bool matchCharacter(const StringAbstraction<BaseType> & str) {
		return (str.charIsBetween(a,b)) || CSet<rest...>::matchCharacter(str);
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t a, wchar_t b, wchar_t... rest> struct WSet {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (matchCharacter(str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename BaseType> static inline bool matchCharacter(const StringAbstraction<BaseType> & str) { 
		return (str.charIsBetween(a,b)) || WSet<rest...>::matchCharacter(str); 
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <char a, char b> struct CSet<a, b> {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (str.charIsBetween(a,b)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename BaseType> static inline bool matchCharacter(const StringAbstraction<BaseType> & str) {
		return str.charIsBetween(a,b);
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t a, wchar_t b> struct WSet<a, b> {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (str.charIsBetween(a,b)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename BaseType> static inline bool matchCharacter(const StringAbstraction<BaseType> & str) {
		return str.charIsBetween(a,b);
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <char a, char b, char... rest> struct CNegSet {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (matchCharacter(str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename BaseType> static inline bool matchCharacter(const StringAbstraction<BaseType> & str) { return !(str.charIsBetween(a,b)) && CNegSet<rest...>::matchCharacter(str); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t a, wchar_t b, wchar_t... rest> struct WNegSet {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (matchCharacter(str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename BaseType> static inline bool matchCharacter(const StringAbstraction<BaseType> & str) { return !(str.charIsBetween(a,b)) && CNegSet<rest...>::matchCharacter(str); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <char a, char b> struct CNegSet<a, b> {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (!(str.charIsBetween(a,b))) {
			pos++;
			return true;
		} else return false;
	}
	template <typename BaseType> static inline bool matchCharacter(const StringAbstraction<BaseType> & str) { return !(str.charIsBetween(a,b)); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t a, wchar_t b> struct WNegSet<a, b> {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (!(str.charIsBetween(a,b))) {
			pos++;
			return true;
		} else return false;
	}
	template <typename BaseType> static inline bool matchCharacter(const StringAbstraction<BaseType> & str) { return !(str.charIsBetween(a,b)); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <typename Inner, typename... rest> struct Seq {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{pos};
		if (exp_inner.match(str, tmp, deep) && exp_rest.match(str.add(tmp - pos), tmp, deep)) {
			pos = tmp;
			return true;
		} else {
			reset();
			return false;
		}
	}
	inline void reset() {
		exp_inner.reset();
		exp_rest.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		 if (exp_rest.template get<id>(catches)) return true;
		 else return exp_inner.template get<id>(catches);
	}
};

template <typename Inner> struct Seq<Inner> {
	Inner exp_inner;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		if (exp_inner.match(str, pos, deep)) {
			return true;
		} else {
			reset();
			return false;
		}
	}
	inline void reset() {
		exp_inner.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		return exp_inner.template get<id>(catches);
	}
};

template <char c, char... rest> struct Str {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{pos};
		if (Chr<c>::match(str, tmp, deep) && Str<rest...>::match(str.add(1), tmp, deep)) {
			pos = tmp;
			return true;
		} else return false;
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <char c> struct Str<c> {
	template <typename BaseType> static inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		return Chr<c>::match(str, pos, deep);
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <typename Inner, typename... rest> struct Sel {
	Inner exp_inner;
	Sel<rest...> exp_rest;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		if (exp_inner.match(str, pos, deep)) return true;
		else {
			if (exp_rest.match(str, pos, deep)) {
				return true;
			} else {
				reset();
				return false;
			}
		}
	}
	inline void reset() {
		exp_inner.reset();
		exp_rest.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		 if (exp_rest.template get<id>(catches)) return true;
		 else return exp_inner.template get<id>(catches);
	}
};

template <typename Inner> struct Sel<Inner> {
	Inner exp_inner;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		if (exp_inner.match(str, pos, deep)) {
			return true;
		} else {
			reset();
			return false;
		}
	}
	inline void reset() {
		exp_inner.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		return exp_inner.template get<id>(catches);
	}
};

struct Any {
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t) {
		if (str.exists()) {
			pos++;
			return true;
		} else return false;
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <typename Inner, typename... rest> struct Star {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{0};
		if (exp_rest.match(str, tmp, deep)) {
			pos += tmp;
			return true;
		} else if (exp_inner.match(str, tmp, deep)) {
			for (;;) {
				if (exp_rest.match(str.add(tmp), tmp, deep)) {
					pos += tmp;
					return true;
				} else if (!exp_inner.match(str.add(tmp), tmp, deep)) {
					reset();
					return false;
				}
			}
		} else {
			reset();
			return false;
		}
	}
	inline void reset() {
		exp_inner.reset();
		exp_rest.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		 if (exp_rest.template get<id>(catches)) return true;
		 else return exp_inner.template get<id>(catches);
	}
};

template <typename Inner> struct Star<Inner> {
	Inner exp_inner;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{0};
		if (exp_inner.match(str, tmp, deep)) {
			for (;;) {
				if (!exp_inner.match(str.add(tmp), tmp, deep)) {
					pos += tmp;
					return true;
				}
			}
		} return true;
	}
	inline void reset() {
		exp_inner.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		return exp_inner.template get<id>(catches);
	}
};

template <typename Inner, typename... rest> struct GStar {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	using Memory = std::pair<Inner,Seq<rest...>>;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{0};
	
		auto memory = remember();
	
		if (exp_inner.match(str, tmp, deep) && match(str.add(tmp), tmp, deep)) {
			pos += tmp;
			return true;
		}
		else {
			if (exp_rest.match(str.add(tmp), tmp, deep)) {
				pos += tmp;
				return true;
			}
			else {
				restore(memory);
				return false;
			}
		}
	}
	inline Memory remember() {
		return {exp_inner, exp_rest};
	}
	inline void restore(Memory & mem) {
		exp_inner = std::move(mem.first);
		exp_rest = std::move(mem.second);
	}
	inline void reset() {
		exp_inner.reset();
		exp_rest.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		 if (exp_rest.template get<id>(catches)) return true;
		 else return exp_inner.template get<id>(catches);
	}
};

template <typename Inner> struct GStar<Inner> {
	Star<Inner> star;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		return star.match(str,pos,deep);
	}
	inline void reset() {
		star.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		return star.template get<id>(catches);
	}
};

template <typename Inner, typename... rest> struct Plus {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{0};
		if (exp_inner.match(str.add(tmp), tmp, deep)) {
			for (;;) {
				if (exp_rest.match(str.add(tmp), tmp, deep)) {
					pos += tmp;
					return true;
				} else if (!exp_inner.match(str.add(tmp), tmp, deep)) {
					reset();
					return false;
				}
			}
		}
		reset();
		return false;
	}
	inline void reset() {
		exp_inner.reset();
		exp_rest.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		 if (exp_rest.template get<id>(catches)) return true;
		 else return exp_inner.template get<id>(catches);
	}
};

template <typename Inner> struct Plus<Inner> {
	Inner exp_inner;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{0};
		if (exp_inner.match(str.add(tmp), tmp, deep)) {
			for (;;) {
				if (!exp_inner.match(str.add(tmp), tmp, deep)) {
					pos += tmp;
					return true;
				}
			}
		}
		reset();
		return false;
	}
	inline void reset() {
		exp_inner.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		return exp_inner.template get<id>(catches);
	}
};

template <typename Inner, typename... rest> struct GPlus {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	using Memory = std::pair<Inner,Seq<rest...>>;
	template <typename BaseType, bool first = true> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{0};
		
		auto memory = remember();
		
		if (exp_inner.match(str, tmp, deep) && match<BaseType,false>(str.add(tmp), tmp, deep)) {
			pos += tmp;
			return true;
		}
		else if (first) {
			restore(memory);
			return false;
		}
		else {
			if (exp_rest.match(str.add(tmp), tmp, deep)) {
				pos += tmp;
				return true;
			}
			else {
				restore(memory);
				return false;
			}
		}
	}
	inline Memory remember() {
		return {exp_inner, exp_rest};
	}
	inline void restore(Memory & mem) {
		exp_inner = std::move(mem.first);
		exp_rest = std::move(mem.second);
	}
	inline void reset() {
		exp_inner.reset();
		exp_rest.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		 if (exp_rest.template get<id>(catches)) return true;
		 else return exp_inner.template get<id>(catches);
	}
};

template <typename Inner> struct GPlus<Inner> {
	Plus<Inner> plus;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		return plus.match(str,pos,deep);
	}
	inline void reset() {
		plus.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		return plus.template get<id>(catches);
	}
};

template <typename Inner, typename... rest> struct Opt {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t tmp{0};
		if (exp_rest.match(str, pos, deep + 1))
			return true;
		else if (exp_inner.match(str, tmp, deep + 1) && exp_rest.match(str.add(tmp), tmp, deep + 1)) {
			pos += tmp;
			return true;
		}
		reset();
		return false;
	}
	inline void reset() {
		exp_inner.reset();
		exp_rest.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		 if (exp_rest.template get<id>(catches)) return true;
		 else return exp_inner.template get<id>(catches);
	}
};

struct End {
	template <typename BaseType> inline bool match(const StringAbstraction<BaseType> str, size_t &, size_t) {
		return str.isEnd();
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

struct Begin {
	template <typename BaseType> inline bool match(const StringAbstraction<BaseType> str, size_t &, size_t) {
		return str.isBegin();
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <typename... rest> struct Eat {
	Seq<rest...> exp_inner;
	template <typename BaseType> inline bool match(StringAbstraction<BaseType> str, size_t &, size_t deep) {
		size_t tmp{0};
		while (str.exists(tmp)) {
			if (exp_inner.match(str.add(tmp), tmp, deep)) return true;
			else tmp++;
		}
		reset();
		return false;
	}
	inline void reset() {
		exp_inner.reset();
	}
	template <unsigned int id> inline bool get(CatchReturn & catches) {
		return exp_inner.template get<id>(catches);
	}
};

template <typename... rest> class StaticRegExp {
protected:
	Eat<rest...> definition;
public:
	bool operator()(const wchar_t * str) {
		size_t tmp{0};
		return definition.match(StringAbstraction<decltype(str)>(str), tmp, 0);
	}
	bool operator()(const std::wstring &str) {
		size_t tmp{0};
		return definition.match(StringAbstraction<decltype(str.c_str())>(str.c_str()), tmp, 0);
	}
	bool operator()(const char * str) {
		size_t tmp{0};
		return definition.match(StringAbstraction<decltype(str)>(str), tmp, 0);
	}
	bool operator()(const std::string &str) {
		size_t tmp{0};
		return definition.match(StringAbstraction<decltype(str.c_str())>(str.c_str()), tmp, 0);
	}
	template <unsigned int id> CatchReturn get() {
		CatchReturn ret;
		if (definition.template get<id>(ret)) {
			return ret;
		} else {
			return {nullptr,0};
		}
	}
	template <typename T> bool match(const T str) {
		definition.reset();
		return operator()(str);
	}
};

template <typename StringType, typename... rest> bool sregexp_smatch(StringType str) {
	StaticRegExp<rest...> regexp;
	return regexp(str);
}

}

#endif
