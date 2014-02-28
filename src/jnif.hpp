#ifndef JNIF_HPP
#define JNIF_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "jnif/utils/BufferReader.hpp"

#include "jnif/utils/BufferWriter.hpp"

#include "jnif/tree/Attr.hpp"
#include "jnif/tree/Attrs.hpp"
#include "jnif/tree/UnknownAttr.hpp"
#include "jnif/tree/CodeAttr.hpp"
#include "jnif/tree/ExceptionsAttr.hpp"
#include "jnif/tree/SourceFileAttr.hpp"
#include "jnif/tree/Member.hpp"
#include "jnif/tree/Members.hpp"
#include "jnif/tree/ConstPool.hpp"
#include "jnif/tree/ClassFile.hpp"
#include "jnif/tree/LntAttr.hpp"

#include "jnif/AccessFlags.hpp"
#include "jnif/Opcode.hpp"
#include "jnif/ClassWriterVisitor.hpp"
#include "jnif/ClassPrinterVisitor.hpp"
#include "jnif/ClassForwardVisitor.hpp"

#include "jnif/parser/AttrsParser.hpp"
#include "jnif/parser/ClassBaseParser.hpp"
#include "jnif/parser/SourceFileAttrParser.hpp"
#include "jnif/parser/CodeAttrParser.hpp"
#include "jnif/parser/LntAttrParser.hpp"
#include "jnif/parser/LvtAttrParser.hpp"
#include "jnif/parser/ExceptionsAttrParser.hpp"
#include "jnif/parser/StackMapTableAttrParser.hpp"

//typedef ClassBaseParser<AttrsParser<SourceFileAttrParser>,
//		AttrsParser<
//				CodeAttrParser<StackMapTableAttrParser, LntAttrParser,
//						LvtAttrParser>, ExceptionsAttrParser>, AttrsParser<> > ClassParser;

namespace jnif {

/**
 * Defines the full class parser using all known attributes parsers.
 */
typedef ClassBaseParser<AttrsParser<SourceFileAttrParser>,
		AttrsParser<CodeAttrParser<>, ExceptionsAttrParser>,
		AttrsParser<>, BufferReader> ClassParser;

}

#endif
