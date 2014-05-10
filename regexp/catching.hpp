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
	size_t size() const {
		return vsize;
	}
	const Catches * data() const {
		return vdata;
	}
	CatchReturn(): vdata{nullptr}, vsize{0} { }
	CatchReturn(Catches * ldata, size_t lsize): vdata{ldata}, vsize{lsize} { }
	Catches * get(const size_t id) {
		if (id < vsize) return &vdata[id];
		else return nullptr;
	}
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
	template <typename BaseType, typename AncestorType> inline bool match(const StringAbstraction<BaseType> str, size_t & pos, size_t deep, AncestorType & ancestor) {
		size_t start{pos};
		bool ret{exp_inner.match(str,pos,deep, ancestor)};
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
	template <typename BaseType, typename AncestorType> inline bool match(const StringAbstraction<BaseType> str, size_t & pos, size_t deep, AncestorType & ancestor) {
		size_t start{pos};
		bool ret{exp_inner.match(str,pos,deep, ancestor)};
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

template <unsigned int baseid, unsigned int catchid = 0> struct ReCatch {
	template <typename BaseType, typename AncestorType> inline bool match(const StringAbstraction<BaseType> str, size_t & pos, size_t, AncestorType & ancestor) {
		CatchReturn ret;
		if (ancestor.template get<baseid>(ret)) {
			const Catches * ctch{ret.get(catchid)};
			if (ctch) {
				for (size_t l{0}; l != ctch->size; ++l) {
					if (!str.equalToOriginal(ctch->start+l,l)) return false;
				}
				pos += ctch->size;
				return true;
			}
		}
		return false;
	}
	inline void reset() {
		
	}
	template <unsigned int id> inline bool get(CatchReturn &) {
		return false;
	}
};

}

#endif
