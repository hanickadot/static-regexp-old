#ifndef __REGEXP__REGEXP__HPP__
#define __REGEXP__REGEXP__HPP__

#include <string>
#include <vector>

// ideally SRegExp<"ahoj|cau$">::smatch

namespace SRegExp {

struct Catches {
	size_t start;
	size_t size;
};

struct CatchReturn {
protected:
	Catches * vdata;
	size_t vsize;
public:
	size_t size() {
		return vsize;
	}
	const Catches * data() {
		return vdata;
	}
	CatchReturn(): vdata{nullptr}, vsize{0} { }
	CatchReturn(Catches * ldata, size_t lsize): vdata{ldata}, vsize{lsize} { }
	Catches * begin() const {
		return vdata;
	}
	Catches * end() const {
		return (vdata + vsize);
	}
};
	
template <unsigned int id, size_t max, typename Inner> struct StaticCatch {
	Inner exp_inner;
	size_t count{0};
	Catches catches[max];
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t start{pos};
		bool ret{exp_inner.smatch(str,pos,cpos,deep)};
		if (ret) {
			if (count < max) catches[count++] = {cpos,pos-start};
		}
		else exp_inner.reset();
		return ret;
	}
	inline void reset() {
		count = 0;
		exp_inner.reset();
	}
	template <unsigned int lid> inline bool get(CatchReturn & lcatches) {
		if (lid == id) {
			lcatches = CatchReturn{catches, count};
			return true;
		}
		else return exp_inner.template get<lid>(lcatches);
	}
};

template <unsigned int id, typename Inner> struct DynamicCatch {
	Inner exp_inner;	
	std::vector<Catches> catches;
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t start{pos};
		bool ret{exp_inner.smatch(str,pos,cpos,deep)};
		if (ret) {
			catches.push_back({cpos,pos-start});
		}
		else exp_inner.reset();
		return ret;
	}
	inline void reset() {
		catches = {};
		exp_inner.reset();
	}
	template <unsigned int lid> inline bool get(CatchReturn & lcatches) {
		if (lid == id) {
			lcatches = {catches.data(), catches.size()};
			return true;
		}
		else return exp_inner.template get<lid>(lcatches);
	}
};

template <char c> struct Chr {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str == c) {
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
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str == c) {
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
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (matchCharacter(*str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return (u >= a && u <= b) || CSet<rest...>::matchCharacter(u); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t a, wchar_t b, wchar_t... rest> struct WSet {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (matchCharacter(*str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return (u >= a && u <= b) || CSet<rest...>::matchCharacter(u); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <char a, char b> struct CSet<a, b> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str >= a && *str <= b) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return (u >= a && u <= b); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t a, wchar_t b> struct WSet<a, b> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str >= a && *str <= b) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return (u >= a && u <= b); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <char a, char b, char... rest> struct CNegSet {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (matchCharacter(*str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return !(u >= a && u <= b) && CNegSet<rest...>::matchCharacter(u); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t a, wchar_t b, wchar_t... rest> struct WNegSet {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (matchCharacter(*str)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return !(u >= a && u <= b) && CNegSet<rest...>::matchCharacter(u); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <char a, char b> struct CNegSet<a, b> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (!(*str >= a && * str <= b)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return !(u >= a && u <= b); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <wchar_t a, wchar_t b> struct WNegSet<a, b> {
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (!(*str >= a && * str <= b)) {
			pos++;
			return true;
		} else return false;
	}
	template <typename chartype> static inline bool matchCharacter(chartype u) { return !(u >= a && u <= b); }
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <typename Inner, typename... rest> struct Seq {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{pos};
		if (exp_inner.smatch(str, tmp, cpos, deep) && exp_rest.smatch(str + tmp - pos, tmp, cpos + tmp - pos, deep)) {
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
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		if (exp_inner.smatch(str, pos, cpos, deep)) {
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
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{pos};
		if (Chr<c>::smatch(str, tmp, cpos, deep) && Str<rest...>::smatch(str + 1, tmp, cpos + 1, deep)) {
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
	template <typename chartype> static inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		return Chr<c>::smatch(str, pos, cpos, deep);
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <typename Inner, typename... rest> struct Sel {
	Inner exp_inner;
	Sel<rest...> exp_rest;
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		if (exp_inner.smatch(str, pos, cpos, deep)) return true;
		else 
		{
			if (exp_rest.smatch(str, pos, cpos, deep)) {
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
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		if (exp_inner.smatch(str, pos, cpos, deep)) {
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
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t, size_t) {
		if (*str) {
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
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (exp_rest.smatch(str, tmp, cpos, deep)) {
			pos += tmp;
			return true;
		} else if (exp_inner.smatch(str + tmp, tmp, cpos, deep)) {
			for (;;) if (exp_rest.smatch(str + tmp, tmp, cpos + tmp, deep)) {
				pos += tmp;
				return true;
			} else if (!exp_inner.smatch(str + tmp, tmp, cpos + tmp, deep)) 
			{
				reset();
				return false;
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
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (exp_inner.smatch(str + tmp, tmp, cpos, deep)) {
			for (;;) if (!exp_inner.smatch(str + tmp, tmp, cpos + tmp, deep)) {
				pos += tmp;
				return true;
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

template <typename Inner, typename... rest> struct Plus {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (exp_inner.smatch(str + tmp, tmp, cpos, deep)) {
			for (;;) if (exp_rest.smatch(str + tmp, tmp, cpos + tmp, deep)) {
				pos += tmp;
				return true;
			} else if (!exp_inner.smatch(str + tmp, tmp, cpos + tmp, deep)) {
				reset();
				return false;
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
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (exp_inner.smatch(str + tmp, tmp, cpos, deep))
			for (;;) if (!exp_inner.smatch(str + tmp, tmp, cpos + tmp, deep)) {
				pos += tmp;
				return true;
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

template <typename Inner, typename... rest> struct Opt {
	Inner exp_inner;
	Seq<rest...> exp_rest;
	template <typename chartype> inline bool smatch(const chartype str, size_t & pos, size_t cpos, size_t deep) {
		size_t tmp{0};
		if (exp_rest.smatch(str, pos, cpos, deep + 1))
			return true;
		else if (exp_inner.smatch(str, tmp, cpos, deep + 1) && exp_rest.smatch(str + tmp, tmp, cpos + tmp, deep + 1)) {
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
	template <typename chartype> inline bool smatch(const chartype * str, size_t &, size_t, size_t) {
		return (!* str);
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

struct Begin {
	template <typename chartype> inline bool smatch(const chartype *, size_t &, size_t cpos, size_t) {
		return cpos == 0;
	}
	inline void reset() { }
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

template <typename... rest> struct Eat {
	Seq<rest...> exp_inner;
	template <typename chartype> inline bool smatch(const chartype * str, size_t &, size_t cpos, size_t deep) {
		size_t tmp{0};
		while (*(str + tmp)) {
			if (exp_inner.smatch(str + tmp, tmp, cpos + tmp, deep)) return true;
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

template <typename... rest> bool sregexp_smatch(const wchar_t * str) {
	size_t tmp{0};
	Eat<rest...> definition;
	return definition.smatch(str, tmp, tmp, 0);
}

template <typename... rest> bool sregexp_smatch(std::wstring str) {
	size_t tmp{0};
	Eat<rest...> definition;
	return definition.smatch(str.c_str(), tmp, tmp, 0);
}

template <typename... rest> bool sregexp_smatch(const char * str) {
	size_t tmp{0};
	Eat<rest...> definition;
	return definition.smatch(str, tmp, tmp, 0);
}

template <typename... rest> bool sregexp_smatch(std::string str) {
	size_t tmp{0};
	Eat<rest...> definition;
	return definition.smatch(str.c_str(), tmp, tmp, 0);
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
	template <unsigned int id> CatchReturn get()
	{
		CatchReturn ret;
		if (definition.template get<id>(ret)) {
			return ret;
		}
		else {
			return {nullptr,0};
		}
	}
	template <typename T> bool match(const T str)
	{
		definition.reset();
		return operator()(str);
	}
};
}

#endif
