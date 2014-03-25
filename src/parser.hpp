/*
 * parser.hpp
 *
 *  Created on: Mar 24, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_HPP
#define JNIF_PARSER_HPP

#include <sstream>

namespace jnif {

class ErrorManager {
public:

	static void log(const char* message, ...) {
		va_list args;
		std::stringstream format;
		format << "" << " | " << message << "\n";

		fprintf(stderr, format.str().c_str(), args);
	}

	static inline void exception(const char* message, ...) {
		va_list args;
		log(message, args);
		exit(2);
	}

	static inline void check(bool condition, const char* message, ...) {
		if (!condition) {
			va_list args;
			exception(message, args);
		}
	}

	static inline void assert(bool condition, const char* message, ...) {
		if (!condition) {
			va_list args;
			exception(message, args);
		}
	}

};

template<typename TErrorManager>
class ConstPoolParser {
public:

	ConstPoolParser(TErrorManager& errorManager) :
			_errorManager(errorManager) {
	}

	template<typename TReader, typename TConstPoolVisitor>
	void parse(TReader& br, TConstPoolVisitor& cpv) {
		u2 count = br.readu2();

		for (int i = 1; i < count; i++) {
			u1 tag = br.readu1();

			switch (tag) {
				case CONSTANT_Class: {
					u2 classNameIndex = br.readu2();
					cpv.visitClass(classNameIndex);
					break;
				}
				case CONSTANT_Fieldref: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cpv.visitFieldRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONSTANT_Methodref: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cpv.visitMethodRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONSTANT_InterfaceMethodref: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cpv.visitInterMethodRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONSTANT_String: {
					u2 utf8Index = br.readu2();
					cpv.visitString(utf8Index);
					break;
				}
				case CONSTANT_Integer: {
					u4 value = br.readu4();
					cpv.visitInteger(value);
					break;
				}
				case CONSTANT_Float: {
					u4 value = br.readu4();
					cpv.visitFloat(value);
					break;
				}
				case CONSTANT_Long: {
					u4 high = br.readu4();
					u4 low = br.readu4();
					cpv.visitLong(((long) high << 32) + low);
					i++;
					break;
				}
				case CONSTANT_Double: {
					u4 high = br.readu4();
					u4 low = br.readu4();
					long lvalue = ((long) high << 32) + low;
					double dvalue = *(double*) &lvalue;
					cpv.visitDouble(dvalue);
					i++;
					break;
				}
				case CONSTANT_NameAndType: {
					u2 nameIndex = br.readu2();
					u2 descIndex = br.readu2();
					cpv.visitNameAndType(nameIndex, descIndex);
					break;
				}
				case CONSTANT_Utf8: {
					u2 len = br.readu2();
					cpv.visitUtf8((const char*) br.pos(), len);
					br.skip(len);
					break;
				}
				case CONSTANT_MethodHandle: {
					u1 refKind = br.readu1();
					u2 refIndex = br.readu2();
					cpv.visitMethodHandle(refKind, refIndex);
					break;
				}
				case CONSTANT_MethodType: {
					u2 descIndex = br.readu2();
					cpv.visitMethodType(descIndex);
					break;
				}
				case CONSTANT_InvokeDynamic: {
					u2 bootstrapMethodAttrIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cpv.visitInvokeDynamic(bootstrapMethodAttrIndex,
							nameAndTypeIndex);
					break;
				}
				default:
					_errorManager.exception("Error while reading tag: %i", tag);
			}
		}
	}

private:

	TErrorManager _errorManager;

};

template<typename TAttrParser>
class AttrTableParser {
public:

	AttrTableParser(TAttrParser& attrParser) :
			_attrParser(attrParser) {
	}

	template<typename TReader, typename TAttrVisitor>
	void parse(TReader& br, TAttrVisitor& av) {
		u2 attrCount = br.readu2();

		for (u2 i = 0; i < attrCount; i++) {
			u2 attrNameIndex = br.readu2();
			u4 attrLen = br.readu4();

			const u1* attrData = br.pos();

			{
				TReader br(attrData, attrLen);
				_attrParser.parse(br, av, attrNameIndex);
			}

			br.skip(attrLen);
		}
	}

private:
	TAttrParser _attrParser;

};

template<typename TClassAttrParser, typename TMethodAttrParser,
		typename TFieldAttrParser, typename TErrorManager>
class ClassFileBaseParser {
public:

	ClassFileBaseParser(TClassAttrParser& classAttrParser,
			TMethodAttrParser& methodAttrParser,
			TFieldAttrParser& fieldAttrParser, TErrorManager& errorManager) :
			_classAttrParser(classAttrParser), _methodAttrParser(
					methodAttrParser), _fieldAttrParser(fieldAttrParser), _errorManager(
					errorManager) {
	}

	template<typename TReader, typename TClassFileVisitor,
			typename TConstPoolVisitor>
	void parse(TReader& br, TClassFileVisitor& cfv, TConstPoolVisitor& cpv) {
		u4 magic = br.readu4();

		_errorManager.check(magic == CLASSFILE_MAGIC,
				"Invalid magic number. Expected 0xcafebabe, found: %x", magic);

		u2 minor = br.readu2();
		u2 major = br.readu2();

		//auto cpv = cfv.visitConstPool();

		ConstPoolParser<TErrorManager> constPoolParser(_errorManager);
		constPoolParser.parse(br, cpv);

		u2 accessFlags = br.readu2();
		u2 thisClassIndex = br.readu2();
		u2 superClassIndex = br.readu2();

		cfv.visitThis(minor, major, accessFlags, thisClassIndex,
				superClassIndex);

		u2 interCount = br.readu2();
		for (int i = 0; i < interCount; i++) {
			u2 interIndex = br.readu2();
			cfv.visitInterface(interIndex);
		}

		u2 fieldCount = br.readu2();
		for (int i = 0; i < fieldCount; i++) {
			u2 accessFlags = br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			auto fv = cfv.visitField(nameIndex, descIndex, accessFlags);

			AttrTableParser<TFieldAttrParser> attrTableParser(_fieldAttrParser);
			attrTableParser.parse(br, fv);
		}

		u2 methodCount = br.readu2();
		for (int i = 0; i < methodCount; i++) {
			u2 accessFlags = br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			auto mv = cfv.visitMethod(nameIndex, descIndex, accessFlags);

			AttrTableParser<TMethodAttrParser> attrTableParser(
					_methodAttrParser);
			attrTableParser.parse(br, mv);
		}

		AttrTableParser<TClassAttrParser> attrTableParser(_classAttrParser);
		attrTableParser.parse(br, cfv);
	}

private:

	TClassAttrParser& _classAttrParser;
	TMethodAttrParser& _methodAttrParser;
	TFieldAttrParser& _fieldAttrParser;
	TErrorManager& _errorManager;

};

template<typename ... TAttrParserList>
class AttrParser {
public:

	AttrParser(ConstPool& cp) :
			_cp(cp) {
	}

	template<typename TReader, typename TAttrVisitor>
	void parse(TReader& br, TAttrVisitor& av, u2 attrNameIndex) {

		std::string attrName = _cp.getUtf8(attrNameIndex);

		parse2<TReader, TAttrVisitor, TAttrParserList...>(attrNameIndex, br,
				attrName, av);
	}

private:

	template<typename TReader, typename TAttrVisitor, typename TAttrParser,
			typename ... TAttrParserTail>
	void parse2(u2 nameIndex, TReader& br, const std::string& attrName,
			TAttrVisitor& av) {
		if (attrName == TAttrParser::AttrName) {
			TAttrParser parser(_cp);
			parser.parse(br, av, nameIndex);
		} else {
			parse2<TReader, TAttrVisitor, TAttrParserTail...>(nameIndex, br,
					attrName, av);
		}
	}

	template<typename TReader, typename TAttrVisitor>
	inline static void parse2(u2 nameIndex, TReader& br,
			const std::string& attrName, TAttrVisitor& av) {
		av.visitAttr(attrName, nameIndex, br);
	}

	ConstPool& _cp;
};

class SourceFileAttrParser {

	template<typename TReader, typename TSourceFileVisitor>
	static void parse(TReader& br, TSourceFileVisitor& sfv) {
		u2 sourceFileIndex = br.readu2();
		sfv.visitSourceFile(sourceFileIndex);
	}
};

template<typename TClassAttrsParser, typename TMethodAttrsParser,
		typename TFieldAttrsParser, typename TErrorManager>
class ClassFileParser;

template<typename ... TClassAttrParserList, typename ... TMethodAttrParserList,
		typename ... TFieldAttrParserList, typename TErrorManager>
class ClassFileParser<AttrParser<TClassAttrParserList...>,
		AttrParser<TMethodAttrParserList...>,
		AttrParser<TFieldAttrParserList...>, TErrorManager> {
public:

	typedef ClassFileBaseParser<AttrParser<TClassAttrParserList...>,
			AttrParser<TMethodAttrParserList...>,
			AttrParser<TFieldAttrParserList...>, TErrorManager> BaseParser;

	ClassFileParser(ConstPool& cp, TErrorManager& errorManager) :
			_classAttrParser(cp), _methodAttrParser(cp), _fieldAttrParser(cp), _baseParser(
					_classAttrParser, _methodAttrParser, _fieldAttrParser,
					errorManager), _cp(cp), _errorManager(errorManager) {
	}

	template<typename TReader, typename TClassFileVisitor>
	void parse(TReader& br, TClassFileVisitor& cfv) {
		struct ConstPoolVisitor {
			ConstPool& cp;
			ConstPoolVisitor(ConstPool& cp) :
					cp(cp) {
			}
			void visitClass(u2 classNameIndex) {
				cp.addClass(classNameIndex);
			}
			void visitFieldRef(u2 classIndex, u2 nameAndTypeIndex) {
				cp.addFieldRef(classIndex, nameAndTypeIndex);
			}
			void visitMethodRef(u2 classIndex, u2 nameAndTypeIndex) {
				cp.addMethodRef(classIndex, nameAndTypeIndex);
			}
			void visitInterMethodRef(u2 classIndex, u2 nameAndTypeIndex) {
				cp.addInterMethodRef(classIndex, nameAndTypeIndex);
			}
			void visitString(u2 utf8Index) {
				cp.addString(utf8Index);
			}
			void visitInteger(u4 value) {
				cp.addInteger(value);
			}
			void visitFloat(u4 value) {
				cp.addFloat(value);
			}
			void visitLong(long value) {
				cp.addLong(value);
			}
			void visitDouble(double value) {
				cp.addDouble(value);
			}
			void visitNameAndType(u2 nameIndex, u2 descIndex) {
				cp.addNameAndType(nameIndex, descIndex);
			}
			void visitUtf8(const char* str, u2 len) {
				cp.addUtf8(str, len);
			}
			void visitMethodHandle(u1 refKind, u2 refIndex) {
				cp.addMethodHandle(refKind, refIndex);
			}
			void visitMethodType(u2 descIndex) {
				cp.addMethodType(descIndex);
			}
			void visitInvokeDynamic(u2 bootstrapAttrIndex,
					u2 nameAndTypeIndex) {
				cp.addInvokeDynamic(bootstrapAttrIndex, nameAndTypeIndex);
			}
		} cpv(_cp);

		_baseParser.parse(br, cfv, cpv);
	}

private:

	AttrParser<TClassAttrParserList...> _classAttrParser;
	AttrParser<TMethodAttrParserList...> _methodAttrParser;
	AttrParser<TFieldAttrParserList...> _fieldAttrParser;
	BaseParser _baseParser;
	ConstPool& _cp;
	TErrorManager& _errorManager;
};

typedef ClassFileParser<AttrParser<SourceFileAttrParser>, AttrParser<>,
		AttrParser<>, ErrorManager> ClassFileFullParser;

}

#endif
