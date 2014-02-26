#ifndef __BCI__PARSER__SOURCE_FILE_ATTR_PARSER__HPP__
#define __BCI__PARSER__SOURCE_FILE_ATTR_PARSER__HPP__

#include "../base.hpp"

namespace bci {
namespace parser {

/**
 *
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
}

#endif
