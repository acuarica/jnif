#ifndef JNIF_CLASSBASEPARSER_HPP
#define JNIF_CLASSBASEPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"
#include "ConstPoolParser.hpp"
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
class ClassBaseParser;

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
class ClassBaseParser<AttrsParser<TClassAttrParserList...>,
		AttrsParser<TMethodAttrParserList...>,
		AttrsParser<TFieldAttrParserList...>> {
public:

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
	template<typename TReader, typename TVisitor>
	static void parse(TReader& br, TVisitor& cfv) {
		u4 magic = br.readu4();

		CHECK(magic == CLASSFILE_MAGIC,
				"Invalid magic number. Expected 0xcafebabe, found: %x", magic);

		u2 minor = br.readu2();
		u2 major = br.readu2();
		cfv.visitVersion(CLASSFILE_MAGIC, minor, major);

		ConstPool cp;
		ConstPoolParser::parse(br, cp);

		cfv.visitConstPool(cp);

		cfv.visitThis(br.readu2(), br.readu2(), br.readu2());

		u2 interCount = br.readu2();
		cfv.visitInterfaceCount(interCount);

		for (int i = 0; i < interCount; i++) {
			cfv.visitInterface(br.readu2());
		}

		u2 fieldCount = br.readu2();
		cfv.visitFieldCount(fieldCount);

		for (int i = 0; i < fieldCount; i++) {
			u2 accessFlags = br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			auto fv = cfv.visitField(accessFlags, nameIndex, descIndex);

			FieldAttrsParser::parse(br, cp, fv);
		}

		u2 methodCount = br.readu2();
		cfv.visitMethodCount(methodCount);

		for (int i = 0; i < methodCount; i++) {
			u2 accessFlags = br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			auto mv = cfv.visitMethod(accessFlags, nameIndex, descIndex);

			MethodAttrsParser::parse(br, cp, mv);
		}

		ClassAttrsParser::parse(br, cp, cfv);
	}

	template<typename TWriter>
	class Writer: public ClassAttrsParser::template Writer<TWriter> {
	public:

		/**
		 *
		 */
		class Field: public FieldAttrsParser::template Writer<TWriter> {
		public:
			inline Field(TWriter& w) :
					FieldAttrsParser::template Writer<TWriter>(w) {
			}
		};

		/**
		 *
		 */
		class Method: public MethodAttrsParser::template Writer<TWriter> {
		public:
			inline Method(TWriter& w) :
					MethodAttrsParser::template Writer<TWriter>(w) {
			}
		};

		inline Writer(TWriter& w) :
				ClassAttrsParser::template Writer<TWriter>(w), w(w) {
		}

		inline void visitVersion(Magic magic, u2 minor, u2 major) {
			w.writeu4(magic);
			w.writeu2(minor);
			w.writeu2(major);
		}

		inline void visitConstPool(ConstPool& cp) {
			ConstPoolParser::write(w, cp);
		}

		inline void visitThis(u2 accessFlags, u2 thisClassIndex,
				u2 superClassIndex) {
			w.writeu2(accessFlags);
			w.writeu2(thisClassIndex);
			w.writeu2(superClassIndex);
		}

		inline void visitInterfaceCount(u2 count) {
			w.writeu2(count);
		}

		inline void visitInterface(u2 interIndex) {
			w.writeu2(interIndex);
		}

		inline void visitFieldCount(u2 count) {
			w.writeu2(count);
		}

		inline Field visitField(u2 accessFlags, u2 nameIndex, u2 descIndex) {
			w.writeu2(accessFlags);
			w.writeu2(nameIndex);
			w.writeu2(descIndex);

			return Field(w);
		}

		inline void visitMethodCount(u2 count) {
			w.writeu2(count);
		}

		inline Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {
			w.writeu2(accessFlags);
			w.writeu2(nameIndex);
			w.writeu2(descIndex);

			return Method(w);
		}

	private:
		TWriter& w;
	};

	template<typename TVisitor>
	struct Forward: ClassAttrsParser::template Forward<TVisitor> {
		TVisitor& cv;

		inline Forward(TVisitor& cv) :
				ClassAttrsParser::template Forward<TVisitor>(cv), cv(cv) {
		}

		struct Field: FieldAttrsParser::template Forward<
				typename TVisitor::Field> {
			typename TVisitor::Field fv;

			Field(typename TVisitor::Field& fv) :
					FieldAttrsParser::template Forward<typename TVisitor::Field>(
							fv), fv(fv) {
			}
		};

		struct Method: MethodAttrsParser::template Forward<
				typename TVisitor::Method> {
			typename TVisitor::Method mv;

			Method(typename TVisitor::Method & mv) :
					MethodAttrsParser::template Forward<
							typename TVisitor::Method>(mv), mv(mv) {
			}
		};

		inline void visitVersion(Magic magic, u2 minor, u2 major) {
			cv.visitVersion(magic, minor, major);
		}

		inline void visitConstPool(ConstPool& cp) {
			cv.visitConstPool(cp);
		}

		inline void visitThis(u2 accessFlags, u2 thisClassIndex,
				u2 superClassIndex) {
			cv.visitThis(accessFlags, thisClassIndex, superClassIndex);
		}

		inline void visitInterfaceCount(u2 count) {
			cv.visitInterfaceCount(count);
		}

		inline void visitInterface(u2 interIndex) {
			cv.visitInterface(interIndex);
		}

		inline void visitFieldCount(u2 count) {
			cv.visitFieldCount(count);
		}

		inline Field visitField(u2 accessFlags, u2 nameIndex, u2 descIndex) {
			auto fv = cv.visitField(accessFlags, nameIndex, descIndex);
			return Field(fv);
		}

		inline void visitMethodCount(u2 count) {
			cv.visitInterfaceCount(count);
		}

		inline Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {
			auto mv = cv.visitMethod(accessFlags, nameIndex, descIndex);
			return Method(mv);
		}
	};
};

}

#endif
