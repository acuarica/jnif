/*
 * Includes
 */
#include "frlog.h"
#include "frexception.h"

#include "jnif.hpp"

#include <iostream>

using namespace std;
using namespace jnif;

void FrInstrClassFile(unsigned char* classFile, int classFileLen,
		const char* className, unsigned int* new_class_data_len,
		unsigned char** new_class_data) {

	auto instr =
			[&](ostream& os) {

				ClassWriterVisitor<> cwv(cout);
				ClassPrinterVisitor<decltype(cwv)> cpv(os, className, classFileLen, cwv);

				BufferReader br(classFile, classFileLen);
				ClassParser::parse(br,cpv);

				int len = cwv.getClassFileSize(cwv.cf);

				ASSERT(classFileLen == len, "%d must be equal to %d on class %s",
						classFileLen, len, className);
				return;
				*new_class_data_len = len;
				*new_class_data = (u1*)malloc(len);

				BufferWriter bw(*new_class_data, len);
				cwv.writeClassFile(bw, cwv.cf);

				return;

				for (int i = 0; i < len; i++) {
					if (classFile[i] != (*new_class_data)[i]) {

						ASSERT(false, "error on %d: %d:%d != %d:%d", i,
								classFile[i],classFile[i+1],
								(*new_class_data)[i],(*new_class_data)[i+1]
						);
					}
				}

				// TODO: free of *new_class_data.
			};

	instr(cerr);
}

extern u1 jnif_BasicClass_class[];
extern u4 jnif_BasicClass_class_len;

void testSimpleModel() {
	ClassPrinterVisitor<> c(cout, "Hola", 123);

	ConstPool cp;

	u2 codeAttrIndex = cp.addUtf8("Code");
	u2 thisIndex = cp.addClass("jnif/java/dynamic/SimpleClass");
	u2 aMethodNameIndex = cp.addUtf8("aMethod");
	u2 aMethodDescIndex = cp.addUtf8("()V");

	c.visitVersion(CLASSFILE_MAGIC, 0, 1);
	c.visitConstPool(cp);
	c.visitThis(ACC_PUBLIC, thisIndex, 0);

	c.visitField(1, 1, 1);

	auto m = c.visitMethod(ACC_PUBLIC, aMethodNameIndex, aMethodDescIndex);
	auto cd = m.visitCode(codeAttrIndex);
	cd.visitNewArray(OPCODE_newarray + 2, OPCODE_newarray, 5);

	cd.visitJump(-23, OPCODE_ifeq, -1);
}

void testIdentityParserWriter() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className) {
				ClassWriterVisitor<> cw(cout);

				BufferReader br(classFile, classFileLen);
				ClassParser::parse(br,cw);


				int len = cw.getClassFileSize(cw.cf);
				ASSERT(classFileLen == len, "%d must be equal to %d on class %s",
						classFileLen, len, className);
			};

	instr(jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass");
}

void testParser() {
	u1* newClassData;
	u4 newClassDataLen;

	FrInstrClassFile(jnif_BasicClass_class, jnif_BasicClass_class_len,
			"jnif/BasicClass", &newClassDataLen, &newClassData);
}

void testWriter() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className) {
				ClassWriter<> cw;
				BufferReader br(classFile, classFileLen);
				ClassParser::parse(br,cw);

				int len = cw.getClassFileSize(cw.cf);
				ASSERT(classFileLen == len, "%d must be equal to %d on class %s",
						classFileLen, len, className);
			};

	instr(jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass");
}

int main(int argc, const char* argv[]) {
	testSimpleModel();
	testIdentityParserWriter();
	testParser();
	testWriter();

	printf("argc: %d, %d\n", argc, jnif_BasicClass_class_len);
	return 0;
}
