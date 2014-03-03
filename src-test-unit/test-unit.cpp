/*
 * Includes
 */
#include "frlog.h"
#include "frexception.h"

#include "jnif.hpp"

#include <iostream>

using namespace std;
using namespace jnif;

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
//
//	c.visitVersion(CLASSFILE_MAGIC, 0, 1);
//	c.visitConstPool(cp);
//	c.visitThis(ACC_PUBLIC, thisIndex, 0);
//
//	c.visitField(1, 1, 1);
//
//	auto m = c.visitMethod(ACC_PUBLIC, aMethodNameIndex, aMethodDescIndex);
//	auto cd = m.visitCode(codeAttrIndex);
//	cd.visitNewArray(OPCODE_newarray + 2, OPCODE_newarray, 5);
//
//	cd.visitJump(-23, OPCODE_ifeq, -1);
//}
//

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

int main(int argc, const char* argv[]) {
	testIdentityComputeSize();
	testIdentityParserWriter();
	//testSimpleModel();

	printf("argc: %d, %d\n", argc, jnif_BasicClass_class_len);
	return 0;
}
