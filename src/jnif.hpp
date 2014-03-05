#ifndef JNIF_HPP
#define JNIF_HPP

#include "utils/BufferReader.hpp"
#include "utils/BufferWriter.hpp"

#include "tree/Attr.hpp"
#include "tree/Attrs.hpp"
#include "tree/UnknownAttr.hpp"
#include "tree/CodeAttr.hpp"
#include "tree/ExceptionsAttr.hpp"
#include "tree/SourceFileAttr.hpp"
#include "tree/Member.hpp"
#include "tree/Members.hpp"
#include "tree/ConstPool.hpp"
#include "tree/ClassFile.hpp"
#include "tree/LntAttr.hpp"

#include "AccessFlags.hpp"
#include "Opcode.hpp"
#include "ClassWriterVisitor.hpp"
#include "ClassPrinterVisitor.hpp"
#include "ClassForwardVisitor.hpp"

#include "ClassWriter.hpp"

#include "parser/AttrsParser.hpp"
#include "parser/ClassBaseParser.hpp"
#include "parser/SourceFileAttrParser.hpp"
#include "parser/CodeAttrParser.hpp"
#include "parser/LntAttrParser.hpp"
#include "parser/LvtAttrParser.hpp"
#include "parser/ExceptionsAttrParser.hpp"
//#include "parser/StackMapTableAttrParser.hpp"

namespace jnif {

//StackMapTableAttrParser, LntAttrParser,LvtAttrParser
/**
 * Defines the full class parser using all known attributes parsers.
 */
typedef ClassBaseParser<AttrsParser<SourceFileAttrParser>,
		AttrsParser<
				CodeAttrParser<>, ExceptionsAttrParser>, AttrsParser<>,
		BufferReader> ClassParser;

}

#endif
