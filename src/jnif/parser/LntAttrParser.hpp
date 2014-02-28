#ifndef JNIF_LNTATTRPARSER_HPP
#define JNIF_LNTATTRPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

/**
 *
 * Line number table
 */
class LntAttrParser {
public:

	static constexpr const char* AttrName = "LineNumberTable";

	template<typename TMethodVisitor,typename TReader>
	inline void parse(TReader& br, TMethodVisitor& v, ConstPool& cp,
			u2 nameIndex) {
		u2 lntlen = br.readu2();

//		vector<>

		for (int i = 0; i < lntlen; i++) {
			u2 startpc = br.readu2();
			u2 lineno = br.readu2();

			v.visitLnt(nameIndex, startpc, lineno);
		}
	}
};

}

#endif
