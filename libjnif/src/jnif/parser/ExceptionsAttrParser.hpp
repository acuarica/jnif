#ifndef JNIF_EXCEPTIONSATTRPARSER_HPP
#define JNIF_EXCEPTIONSATTRPARSER_HPP

#include "../base.hpp"

namespace jnif {

/**
 *
 * Exceptions
 */
class ExceptionsAttrParser {
public:

	static constexpr const char* AttrName = "Exceptions";

	template<typename TVisitor>
	inline static void parse(BufferReader& br, TVisitor& v, ConstPool& cp,
			u2 nameIndex) {
		u2 len = br.readu2();

		vector < u2 > es;
		for (int i = 0; i < len; i++) {
			u2 exceptionIndex = br.readu2();

			es.push_back(exceptionIndex);
		}

		v.visitException(nameIndex, es);
	}
};

}

#endif
