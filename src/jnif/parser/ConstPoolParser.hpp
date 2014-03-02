/*
 * ConstPoolParser.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: luigi
 */

#ifndef JNIF_CONSTPOOLPARSER_HPP
#define JNIF_CONSTPOOLPARSER_HPP

namespace jnif {

class ConstPoolParser {
public:

	template<typename TReader>
	static void parse(TReader& br, ConstPool& cp) {
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
					std::string str((const char*) br.pos(), len);
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
					entry->invokedynamic.bootstrap_method_attr_index =
							br.readu2();
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

	template<typename TWriter>
	static void write(TWriter& bw, const ConstPool& cp) {
		u2 count = cp.entries.size();
		bw.writeu2(count);

		for (u4 i = 1; i < cp.entries.size(); i++) {
			const ConstPool::Entry* entry = &cp.entries[i];

			bw.writeu1(entry->tag);

			switch (entry->tag) {
				case CONSTANT_Class:
					bw.writeu2(entry->clazz.name_index);
					break;
				case CONSTANT_Fieldref:
				case CONSTANT_Methodref:
				case CONSTANT_InterfaceMethodref:
					bw.writeu2(entry->memberref.class_index);
					bw.writeu2(entry->memberref.name_and_type_index);
					break;
				case CONSTANT_String:
					bw.writeu2(entry->s.string_index);
					break;
				case CONSTANT_Integer:
					bw.writeu4(entry->i.value);
					break;
				case CONSTANT_Float:
					bw.writeu4(entry->f.value);
					break;
				case CONSTANT_Long:
					bw.writeu4(entry->l.high_bytes);
					bw.writeu4(entry->l.low_bytes);
					i++;
					break;
				case CONSTANT_Double:
					bw.writeu4(entry->d.high_bytes);
					bw.writeu4(entry->d.low_bytes);
					i++;
					break;
				case CONSTANT_NameAndType:
					bw.writeu2(entry->nameandtype.name_index);
					bw.writeu2(entry->nameandtype.descriptor_index);
					break;
				case CONSTANT_Utf8: {
					u2 len = entry->utf8.str.length();
					const char* str = entry->utf8.str.c_str();
					bw.writeu2(len);
					bw.writecount(str, len);
					break;
				}
				case CONSTANT_MethodHandle:
					bw.writeu1(entry->methodhandle.reference_kind);
					bw.writeu2(entry->methodhandle.reference_index);
					break;
				case CONSTANT_MethodType:
					bw.writeu2(entry->methodtype.descriptor_index);
					break;
				case CONSTANT_InvokeDynamic:
					bw.writeu2(
							entry->invokedynamic.bootstrap_method_attr_index);
					bw.writeu2(entry->invokedynamic.name_and_type_index);
					break;
			}
		}
	}
};

}

#endif
