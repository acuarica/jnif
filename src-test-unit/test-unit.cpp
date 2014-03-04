/*
 * Includes
 */
#include "frlog.h"
#include "frexception.h"

#include "jnif.hpp"

#include <iostream>

using namespace std;
using namespace jnif;

template<typename TVisitor>
struct NopAdderInstr: ClassParser::Forward<TVisitor> {
	typedef ClassParser::Forward<TVisitor> base;
	using base::cv;

	NopAdderInstr(TVisitor& cv) :
			ClassParser::Forward<TVisitor>(cv) {
	}

	struct Method: base::Method {
		using base::Method::mv;

		Method(typename TVisitor::Method& mv) :
				base::Method(mv) {
		}

		inline void codeStart() {
			mv.visitZero(-42, OPCODE_nop);
			mv.visitZero(-42, OPCODE_nop);
		}
	};

	inline Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		auto mv = base::cv.visitMethod(accessFlags, nameIndex, descIndex);

		return Method(mv);
	}
};

extern u1 jnif_BasicClass_class[];
extern u4 jnif_BasicClass_class_len;

extern u1 jnif_ExceptionClass_class[];
extern u4 jnif_ExceptionClass_class_len;
//
//void testSimpleModel() {
//	ClassPrinterVisitor<> c(cout, "Hola", 123);
//
//	ConstPool cp;
//
//	u2 codeAttrIndex = cp.addUtf8("Code");
//	u2 thisIndex = cp.addClass("jnif/java/dynamic/SimpleClass");
//	u2 aMethodNameIndex = cp.addUtf8("aMethod");
//	u2 aMethodDescIndex = cp.addUtf8("()V");
//	u2 exceptionIndex = cp.addClass("java/lang/Exception");
//
//	c.visitVersion(CLASSFILE_MAGIC, 0, 1);
//	c.visitConstPool(cp);
//	c.visitThis(ACC_PUBLIC, thisIndex, 0);
//
//	auto fv = c.visitField(ACC_PUBLIC, 1, 1);
//
//	auto mv = c.visitMethod(ACC_PUBLIC | ACC_ABSTRACT, aMethodNameIndex,
//			aMethodDescIndex);
//	mv.visitExceptionEntry(exceptionIndex);
//}

void testIdentityComputeSize() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className) {
				BufferSize w;
				BufferReader r(classFile, classFileLen);
				ClassParser::Writer<BufferSize> cw(w);
				ClassParser::parse(r, cw);

				int len = w.size();
				ASSERT(classFileLen == len, "Expected class file len %d, actual was %d, on class %s",
						classFileLen, len, className);
			};

	instr(jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass");
	instr(jnif_ExceptionClass_class, jnif_ExceptionClass_class_len,
			"jnif/ExceptionClass");
}

void testIdentityParserWriter() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className) {

				int new_class_data_len;
				{
					BufferSize w;
					BufferReader br(classFile, classFileLen);
					ClassParser::Writer<BufferSize> cw(w);
					ClassParser::parse(br, cw);
					new_class_data_len = w.size();
				}

				ASSERT(classFileLen == new_class_data_len, "Expected class file len %d, actual was %d, on class %s",
						classFileLen, new_class_data_len, className);

				u1* new_class_data = new u1[new_class_data_len];

				{
					BufferWriter w(new_class_data, new_class_data_len);
					BufferReader r(classFile, classFileLen);
					ClassParser::Writer<BufferWriter> cw(w);
					ClassParser::parse(r, cw);
				}

				for (int i = 0; i < new_class_data_len; i++) {
					ASSERT(classFile[i] == new_class_data[i], "error on %d: %d:%d != %d:%d", i,
							classFile[i],classFile[i+1],
							new_class_data[i],new_class_data[i+1]
					);
				}

				delete [] new_class_data;
			};

	instr(jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass");
	instr(jnif_ExceptionClass_class, jnif_ExceptionClass_class_len,
			"jnif/ExceptionClass");

}

void testNopAdderInstrSize() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className,int diff) {
				BufferSize w;
				BufferReader r(classFile, classFileLen);

				ClassParser::Writer<BufferSize> cw(w);

				NopAdderInstr<decltype(cw)> iv(cw);

				ClassParser::parse(r, iv);

				int len = w.size();
				ASSERT(classFileLen+diff == len, "Expected class file len %d, actual was %d, on class %s",
						classFileLen+diff, len, className);
			};

	instr(jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass",
			4 * 2);

	instr(jnif_ExceptionClass_class, jnif_ExceptionClass_class_len,
			"jnif/ExceptionClass", 1 * 2);
}

#define RUN(test) ( fprintf(stderr, "Running test " #test "... "), \
	test(), fprintf(stderr, "[OK]\n") )

int main(int argc, const char* argv[]) {
	RUN(testIdentityComputeSize);
	RUN(testIdentityParserWriter);
	RUN(testNopAdderInstrSize);
	//testSimpleModel();

	fprintf(stderr, "argc: %d, %d\n", argc, jnif_BasicClass_class_len);
	return 0;
}
