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

void testIdentityComputeSize() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className) {
				ClassFile cf;
				ClassParser::parse(classFile, classFileLen, cf);

				int newlen = cf.size();

				ASSERT(classFileLen == newlen, "Expected class file len %d, actual was %d, on class %s",
						classFileLen, newlen, className);
			};

	instr(jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass");
	instr(jnif_ExceptionClass_class, jnif_ExceptionClass_class_len,
			"jnif/ExceptionClass");
}

void testIdentityParserWriter() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className) {

				ClassFile cf;
				ClassParser::parse(classFile, classFileLen, cf);

				int newlen = cf.size();

				ASSERT(classFileLen == newlen, "Expected class file len %d, actual was %d, on class %s",
						classFileLen, newlen, className);

				u1* newClassData = new u1[newlen];
				cf.write(newClassData, newlen);

				for (int i = 0; i < newlen; i++) {
					ASSERT(classFile[i] == newClassData[i], "error on %d: %d:%d != %d:%d", i,
							classFile[i],classFile[i+1],
							newClassData[i],newClassData[i+1]
					);
				}

				delete [] newClassData;
			};

	instr(jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass");
	instr(jnif_ExceptionClass_class, jnif_ExceptionClass_class_len,
			"jnif/ExceptionClass");

}

void testNopAdderInstrSize() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className,int diff) {
//				BufferSize w;
//				BufferReader r(classFile, classFileLen);
//				ClassParser::ClassWriter<BufferSize> cw(w);
//				NopAdderInstr::Class<decltype(cw)> iv(cw);
//				ClassParser::parse(r, iv);
//
//				int len = w.size();
//				ASSERT(classFileLen+diff == len, "Expected class file len %d, actual was %d, on class %s",
//						classFileLen+diff, len, className);
			};

	instr(jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass",
			4 * 2);

	instr(jnif_ExceptionClass_class, jnif_ExceptionClass_class_len,
			"jnif/ExceptionClass", 1 * 2);
}

void testNopAdderInstr() {
	auto instr =
			[&](unsigned char* classFile, int classFileLen, const char* className,int diff) {

//				int newlen = [&]() {
//					BufferSize w;
//					BufferReader r(classFile, classFileLen);
//					ClassParser::ClassWriter<BufferSize> cw(w);
//					NopAdderInstr::Class<decltype(cw)> iv(cw);
//					ClassParser::parse(r, iv);
//					return w.size();
//				}();
//
//				ASSERT(classFileLen + diff == newlen, "Expected class file len %d, actual was %d, on class %s",
//						classFileLen+diff, newlen, className);
//
//				u1* newdata = new u1[newlen];
//				{
//					BufferWriter w(newdata, newlen);
//					BufferReader r(classFile, classFileLen);
//					PatchWriter cw(w);
//					NopAdderInstr::Class<decltype(cw)> iv(cw);
//					ClassParser::parse(r, iv);
//				}
//
//				int newlen2 = [&]() {
//					BufferSize w;
//					BufferReader r(newdata, newlen);
//					ClassParser::ClassWriter<BufferSize> cw(w);
//					ClassParser::parse(r, cw);
//					return w.size();
//				}();
//
//				ASSERT(newlen == newlen2, "Expected class file len %d, actual was %d, on class %s",
//						newlen, newlen2, className);
//
////				for (int i = 0; i < newlen; i++) {
////					ASSERT(classFile[i] == newdata[i], "error on %d: %d:%d != %d:%d", i,
////							classFile[i],classFile[i+1],
////							newdata[i],newdata[i+1]
////					);
////				}

//		delete [] newdata;
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
	RUN(testNopAdderInstr);

	fprintf(stderr, "argc: %d, %d\n", argc, jnif_BasicClass_class_len);
	return 0;
}
