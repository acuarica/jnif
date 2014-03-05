#ifndef JNIF_HPP
#define JNIF_HPP

#include "base.hpp"
#include "AccessFlags.hpp"
#include "Opcode.hpp"

#include "utils/BufferReader.hpp"
#include "utils/BufferWriter.hpp"
#include "utils/BufferSize.hpp"

#include "core/AttrsParser.hpp"
#include "core/ClassBaseParser.hpp"
#include "core/SourceFileAttrParser.hpp"
#include "core/CodeAttrParser.hpp"
#include "core/LntAttrParser.hpp"
#include "core/LvtAttrParser.hpp"
#include "core/ExceptionsAttrParser.hpp"
//#include "core/StackMapTableAttrParser.hpp"

#include "backends/ClassPrinter.hpp"


namespace jnif {

// StackMapTableAttrParser,

/**
 * Defines the full class parser using all known attributes parsers.
 */
//typedef ClassBaseParser<AttrsParser<SourceFileAttrParser>,
//		AttrsParser<CodeAttrParser<LntAttrParser, LvtAttrParser>,
//				ExceptionsAttrParser>, AttrsParser<>> ClassParser;
typedef CodeAttrParser<LntAttrParser, LvtAttrParser> CodeFullParser;

template<typename ... TList>
struct L: TList... {
};

typedef ClassBaseParser<AttrsParser<SourceFileAttrParser>,
		AttrsParser<CodeFullParser,
				L<ExceptionsAttrParser, ExceptionsAttrPrinter>>, AttrsParser<>> ClassParser;

}

#include "tree/ConstPool.hpp"
#include "tree/Attr.hpp"
#include "tree/Attrs.hpp"
#include "tree/UnknownAttr.hpp"
#include "tree/CodeAttr.hpp"
#include "tree/ExceptionsAttr.hpp"
#include "tree/SourceFileAttr.hpp"
#include "tree/Member.hpp"
#include "tree/Members.hpp"
#include "tree/ClassFile.hpp"
#include "tree/LntAttr.hpp"

#include "backends/PatchWriter.hpp"
#include "backends/ClassPrinter.hpp"

#endif
