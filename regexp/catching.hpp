#ifndef __REGEXP__CATCHING__HPP__
#define __REGEXP__CATCHING__HPP__

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
	template <typename BaseType> inline bool match(const StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t start{pos};
		bool ret{exp_inner.match(str,pos,deep)};
		if (ret) {
			if (count < max) catches[count++] = {str.getPosition(),pos-start};
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

template <unsigned int id, typename Inner> using OneCatch = StaticCatch<id, 1, Inner>;

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
	template <typename BaseType> inline bool match(const StringAbstraction<BaseType> str, size_t & pos, size_t deep) {
		size_t start{pos};
		bool ret{exp_inner.match(str,pos,deep)};
		if (ret) {
			catches.push_back({str.getPosition(),pos-start});
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

}

#endif
