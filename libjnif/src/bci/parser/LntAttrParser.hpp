#ifndef __BCI__LNT_ATTR_PARSER__HPP__
#define __BCI__LNT_ATTR_PARSER__HPP__

#include "../base.hpp"

namespace JNIFNS {

/**
 *
 * Line number table
 */
class LntAttrParser {
public:

	static constexpr const char* AttrName = "LineNumberTable";

	template<typename TMethodVisitor>
	inline void parse(BufferReader& br, TMethodVisitor& v, ConstPool& cp,
			u2 nameIndex) {
		u2 lntlen = br.readu2();

		for (int i = 0; i < lntlen; i++) {
			u2 startpc = br.readu2();
			u2 lineno = br.readu2();

			v.visitLnt(startpc, lineno);
		}
	}
};

}

#endif
