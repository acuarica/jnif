/*
 * ClassParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_CLASSPARSER_HPP
#define JNIF_PARSER_CLASSPARSER_HPP

#include "AttrsParser.hpp"

namespace jnif {

/**
 * Represents an abstract java class file parser.
 *
 * Only suitable when TClassAttrsParser, TMethodAttrsParser and
 * TFieldAttrsParser are AttrsParser classes.
 */
template<typename TClassAttrsParser, typename TMethodAttrsParser,
		typename TFieldAttrsParser>
class ClassParser;

/**
 * Represents an abstract java class file parser.
 *
 * Instantiation of the parser implementation.
 *
 * The template instantiation is the only one accepted, since it receives
 * the members and class attributes parsers as templates lists.
 */
template<typename ... TClassAttrParserList, typename ... TMethodAttrParserList,
		typename ... TFieldAttrParserList>
class ClassParser<AttrsParser<TClassAttrParserList...>,
		AttrsParser<TMethodAttrParserList...>,
		AttrsParser<TFieldAttrParserList...>> {
public:

	TypeFactory _typeFactory;

	/**
	 *
	 */
	typedef AttrsParser<TClassAttrParserList...> ClassAttrsParser;

	/**
	 *
	 */
	typedef AttrsParser<TMethodAttrParserList...> MethodAttrsParser;

	/**
	 *
	 */
	typedef AttrsParser<TFieldAttrParserList...> FieldAttrsParser;

	/**
	 *
	 */
	void parse(BufferReader& br, ClassFile& cf) {
		u4 magic = br.readu4();

		Error::check(magic == CLASSFILE_MAGIC,
				"Invalid magic number. Expected 0xcafebabe, found: ", magic);

		u2 minorVersion = br.readu2();
		u2 majorVersion = br.readu2();

		cf.version = Version(majorVersion, minorVersion);

		_parseConstPool(br, cf);

		cf.accessFlags = br.readu2();
		cf.thisClassIndex = br.readu2();
		cf.superClassIndex = br.readu2();

		u2 interCount = br.readu2();
		for (int i = 0; i < interCount; i++) {
			u2 interIndex = br.readu2();
			cf.interfaces.push_back(interIndex);
		}

		u2 fieldCount = br.readu2();
		for (int i = 0; i < fieldCount; i++) {
			u2 accessFlags = br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			Field* f = cf.addField(nameIndex, descIndex, accessFlags);

			FieldAttrsParser().parse(br, cf, *f);
		}

		u2 methodCount = br.readu2();
		for (int i = 0; i < methodCount; i++) {
			u2 accessFlags = br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			Method* m = cf.addMethod(nameIndex, descIndex, accessFlags);

			MethodAttrsParser().parse(br, cf, *m);
		}

		ClassAttrsParser().parse(br, cf, cf);
	}

private:

	void _parseConstPool(BufferReader& br, ConstPool& cp) {
		u2 count = br.readu2();

		for (int i = 1; i < count; i++) {
			u1 tag = br.readu1();

			switch (tag) {
				case CONST_CLASS: {
					u2 classNameIndex = br.readu2();
					cp.addClass(classNameIndex);
					break;
				}
				case CONST_FIELDREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addFieldRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONST_METHODREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addMethodRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONST_INTERMETHODREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addInterMethodRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONST_STRING: {
					u2 utf8Index = br.readu2();
					cp.addString(utf8Index);
					break;
				}
				case CONST_INTEGER: {
					u4 value = br.readu4();
					cp.addInteger(value);
					break;
				}
				case CONST_FLOAT: {
					u4 value = br.readu4();
					float fvalue = *(float*) &value;
					//auto idx =
					cp.addFloat(fvalue);

					//float v = cp.entries[idx].f.value;
					//Error::assert(value== *(u4*) &v, "invalid: ", br.offset() );
//					float fvalue = entry->f.value;
//					u4 value = *(u4*) &fvalue;

					break;
				}
				case CONST_LONG: {
					u4 high = br.readu4();
					u4 low = br.readu4();
					long value = ((long) high << 32) + low;
					cp.addLong(value);
					i++;
					break;
				}
				case CONST_DOUBLE: {
					u4 high = br.readu4();
					u4 low = br.readu4();
					long lvalue = ((long) high << 32) + low;
					double dvalue = *(double*) &lvalue;
					cp.addDouble(dvalue);
					i++;
					break;
				}
				case CONST_NAMEANDTYPE: {
					u2 nameIndex = br.readu2();
					u2 descIndex = br.readu2();
					cp.addNameAndType(nameIndex, descIndex);
					break;
				}
				case CONST_UTF8: {
					u2 len = br.readu2();
					cp.addUtf8((const char*) br.pos(), len);
					br.skip(len);
					break;
				}
				case CONST_METHODHANDLE: {
					u1 refKind = br.readu1();
					u2 refIndex = br.readu2();
					cp.addMethodHandle(refKind, refIndex);
					break;
				}
				case CONST_METHODTYPE: {
					u2 descIndex = br.readu2();
					cp.addMethodType(descIndex);
					break;
				}
				case CONST_INVOKEDYNAMIC: {
					u2 bootMethodAttrIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addInvokeDynamic(bootMethodAttrIndex, nameAndTypeIndex);
					break;
				}
				default:
					Error::raise("Error while reading tag: ", tag);
			}
		}
	}

};

}

#endif
