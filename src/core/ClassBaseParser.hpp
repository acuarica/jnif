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

	/**
	 *
	 */
	template<typename TWriter>
	struct FieldWriter: FieldAttrsParser::template Writer<TWriter> {
		FieldWriter(TWriter& w) :
				FieldAttrsParser::template Writer<TWriter>(w) {
		}
	};

	/**
	 *
	 */
	template<typename TWriter>
	struct MethodWriter: public MethodAttrsParser::template Writer<TWriter> {
		MethodWriter(TWriter& w) :
				MethodAttrsParser::template Writer<TWriter>(w) {
		}
	};

	/**
	 *
	 */
	template<typename TWriter>
	struct ClassWriter: ClassAttrsParser::template Writer<TWriter> {

		/**
		 *
		 */
		TWriter& w;

		ClassWriter(TWriter& w) :
				ClassAttrsParser::template Writer<TWriter>(w), w(w) {
		}

		void visitVersion(Magic magic, u2 minor, u2 major) {
			w.writeu4(magic);
			w.writeu2(minor);
			w.writeu2(major);
		}

		void visitConstPool(ConstPool& cp) {
			ConstPoolParser::write(w, cp);
		}

		void visitThis(u2 accessFlags, u2 thisClassIndex, u2 superClassIndex) {
			w.writeu2(accessFlags);
			w.writeu2(thisClassIndex);
			w.writeu2(superClassIndex);
		}

		void visitInterfaceCount(u2 count) {
			w.writeu2(count);
		}

		void visitInterface(u2 interIndex) {
			w.writeu2(interIndex);
		}

		void visitFieldCount(u2 count) {
			w.writeu2(count);
		}

		FieldWriter<TWriter> visitField(u2 accessFlags, u2 nameIndex,
				u2 descIndex) {
			w.writeu2(accessFlags);
			w.writeu2(nameIndex);
			w.writeu2(descIndex);

			return FieldWriter<TWriter>(w);
		}

		void visitMethodCount(u2 count) {
			w.writeu2(count);
		}

		MethodWriter<TWriter> visitMethod(u2 accessFlags, u2 nameIndex,
				u2 descIndex) {
			w.writeu2(accessFlags);
			w.writeu2(nameIndex);
			w.writeu2(descIndex);

			return MethodWriter<TWriter>(w);
		}
	};

	/**
	 *
	 */
	template<typename TVisitor>
	struct FieldForward: FieldAttrsParser::template Forward<TVisitor> {
		TVisitor fv;

		FieldForward(TVisitor& fv) :
				FieldAttrsParser::template Forward<TVisitor>(fv), fv(fv) {
		}
	};

	/**
	 *
	 */
	template<typename TVisitor>
	struct MethodForward: MethodAttrsParser::template Forward<TVisitor> {
		TVisitor mv;

		MethodForward(TVisitor& mv) :
				MethodAttrsParser::template Forward<TVisitor>(mv), mv(mv) {
		}
	};

	/**
	 *
	 */
	template<typename TVisitor>
	struct ClassForward: ClassAttrsParser::template Forward<TVisitor> {
		TVisitor& cv;

		ClassForward(TVisitor& cv) :
				ClassAttrsParser::template Forward<TVisitor>(cv), cv(cv) {
		}

		void visitVersion(Magic magic, u2 minor, u2 major) {
			cv.visitVersion(magic, minor, major);
		}

		void visitConstPool(ConstPool& cp) {
			cv.visitConstPool(cp);
		}

		void visitThis(u2 accessFlags, u2 thisClassIndex, u2 superClassIndex) {
			cv.visitThis(accessFlags, thisClassIndex, superClassIndex);
		}

		void visitInterfaceCount(u2 count) {
			cv.visitInterfaceCount(count);
		}

		void visitInterface(u2 interIndex) {
			cv.visitInterface(interIndex);
		}

		void visitFieldCount(u2 count) {
			cv.visitFieldCount(count);
		}

		auto visitField(u2 accessFlags, u2 nameIndex,
				u2 descIndex)-> decltype(cv.visitField(0, 0, 0)) {
			auto fv = cv.visitField(accessFlags, nameIndex, descIndex);
			return fv;
		}

		void visitMethodCount(u2 count) {
			cv.visitInterfaceCount(count);
		}

		auto visitMethod(u2 accessFlags, u2 nameIndex,
				u2 descIndex)-> decltype(cv.visitMethod(0, 0, 0)) {
			auto mv = cv.visitMethod(accessFlags, nameIndex, descIndex);
			return mv;
		}
	};
};

}

#endif
