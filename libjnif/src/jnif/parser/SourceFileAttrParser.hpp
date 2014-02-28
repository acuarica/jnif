#ifndef JNIF_SOURCEFILEATTRPARSER_HPP
#define JNIF_SOURCEFILEATTRPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

/**
 * SourceFile attribute parser.
 */
class SourceFileAttrParser {
public:

	static constexpr const char* AttrName = "SourceFile";

	template<typename TVisitor>
	inline static void parse(BufferReader& br, TVisitor& v, ConstPool& cp,
			u2 nameIndex) {
		u2 sourceFileIndex = br.readu2();

		v.visitSourceFile(nameIndex, sourceFileIndex);
	}
};

}

#endif
