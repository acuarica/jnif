#ifndef __BCI__PARSER__CLASS_BASE_PARSER_H__
#define	__BCI__PARSER__CLASS_BASE_PARSER_H__

#include "../base.hpp"
#include "../BufferReader.hpp"
#include "../ConstPool.hpp"
#include "AttrsParser.hpp"

namespace bci {
namespace parser {

template<typename TClassAttrsParser, typename TMethodAttrsParser,
		typename TFieldAttrsParser>
class ClassBaseParser;

/**
 * Class file parser
 */
template<typename ... TClassAttrParserList, typename ... TMethodAttrParserList,
		typename ... TFieldAttrParserList>
class ClassBaseParser<AttrsParser<TClassAttrParserList...>,
		AttrsParser<TMethodAttrParserList...>,
		AttrsParser<TFieldAttrParserList...> > {
public:

	template<typename TVisitor>
	static void parse(const u1* fileImage, const int fileImageLen,
			TVisitor& cfv) {
		BufferReader br(fileImage, fileImageLen);

		u4 magic = br.readu4();

		CHECK(magic == 0xcafebabe,
				"Invalid magic number. Expected 0xcafebabe, found: %x", magic);

		u2 minor = br.readu2();
		u2 major = br.readu2();
		cfv.visitVersion(magic, minor, major);

		ConstPool cp;
		parseConstPool(br, cp);

		cfv.visitConstPool(cp);

		cfv.visitThis(br.readu2(), br.readu2(), br.readu2());

		u2 interCount = br.readu2();
		for (int i = 0; i < interCount; i++) {
			cfv.visitInterface(br.readu2());
		}

		u2 fieldCount = br.readu2();
		for (int i = 0; i < fieldCount; i++) {
			auto fv = cfv.visitField(br.readu2(), br.readu2(), br.readu2());

			AttrsParser<TFieldAttrParserList...>::parse(br, cp, fv);
		}

		u2 methodCount = br.readu2();
		for (int i = 0; i < methodCount; i++) {
			u2 accessFlags = br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			auto mv = cfv.visitMethod(accessFlags, nameIndex, descIndex);

			AttrsParser<TMethodAttrParserList...>::parse(br, cp, mv);
		}

		AttrsParser<TClassAttrParserList...>::parse(br, cp, cfv);
	}

private:

	static void parseConstPool(BufferReader& br, ConstPool& cp) {
		u2 count = br.readu2();

		for (int i = 1; i < count; i++) {
			ConstPool::Entry e;
			ConstPool::Entry* entry = &e;

			entry->tag = br.readu1();

			switch (entry->tag) {
			case CONSTANT_Class:
				entry->clazz.name_index = br.readu2();
				break;
			case CONSTANT_Fieldref:
			case CONSTANT_Methodref:
			case CONSTANT_InterfaceMethodref:
				entry->memberref.class_index = br.readu2();
				entry->memberref.name_and_type_index = br.readu2();
				break;
			case CONSTANT_String:
				entry->s.string_index = br.readu2();
				break;
			case CONSTANT_Integer:
				entry->i.value = br.readu4();
				break;
			case CONSTANT_Float:
				entry->f.value = br.readu4();
				break;
			case CONSTANT_Long:
				entry->l.high_bytes = br.readu4();
				entry->l.low_bytes = br.readu4();
				i++;
				break;
			case CONSTANT_Double:
				entry->d.high_bytes = br.readu4();
				entry->d.low_bytes = br.readu4();
				i++;
				break;
			case CONSTANT_NameAndType:
				entry->nameandtype.name_index = br.readu2();
				entry->nameandtype.descriptor_index = br.readu2();
				break;
			case CONSTANT_Utf8: {
				u2 len = br.readu2();
				string str((const char*) br.pos(), len);
				entry->utf8.str = str;
				br.skip(len);
				break;
			}
			case CONSTANT_MethodHandle:
				entry->methodhandle.reference_kind = br.readu1();
				entry->methodhandle.reference_index = br.readu2();
				break;
			case CONSTANT_MethodType:
				entry->methodtype.descriptor_index = br.readu2();
				break;
			case CONSTANT_InvokeDynamic:
				entry->invokedynamic.bootstrap_method_attr_index = br.readu2();
				entry->invokedynamic.name_and_type_index = br.readu2();
				break;
			default:
				EXCEPTION("Error while reading tag: %i", entry->tag);
			}

			if (entry->tag == CONSTANT_Long || entry->tag == CONSTANT_Double) {
				cp.addDouble(e);
			} else {
				cp.addSingle(e);
			}
		}
	}
};

}
}

#endif
