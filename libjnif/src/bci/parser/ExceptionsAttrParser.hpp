#ifndef __BCI_EXCEPTIONSATTRPARSER_H__
#define __BCI_EXCEPTIONSATTRPARSER_H__

#include "../base.hpp"

namespace JNIFNS {

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

		vector<u2> es;
		for (int i = 0; i < len; i++) {
			u2 exceptionIndex = br.readu2();

			es.push_back(exceptionIndex);
		}

		v.visitException(nameIndex, es);
	}
};

}

#endif
