/*
 * Includes
 */
#include "jnif.hpp"

#include <iostream>
#include <functional>

using namespace std;
using namespace jnif;

struct JavaFile {
	u1* data;
	int len;
	const char* name;
};

extern u1 jnif_BasicClass_class[];
extern int jnif_BasicClass_class_len;

extern u1 jnif_ExceptionClass_class[];
extern int jnif_ExceptionClass_class_len;

extern u1 jnif_TestProxy_class[];
extern int jnif_TestProxy_class_len;

JavaFile tests[] = {

{ jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass" },

{ jnif_ExceptionClass_class, jnif_ExceptionClass_class_len,
		"jnif/ExceptionClass" },

{ jnif_TestProxy_class, jnif_TestProxy_class_len, "jnif/TestProxy" },

};

template<typename T>
void apply(T instr) {
	for (auto& jf : tests) {
		instr(jf);
	}
}

static void testPrinter() {
	auto instr = [&](JavaFile& jf) {
		ClassFile cf;
		parseClassFile(jf.data, jf.len, cf);
		printClassFile(cf, cerr);
	};

	apply(instr);
}

static void testIdentityComputeSize() {
	auto instr = [&](JavaFile& jf) {
		ClassFile cf;
		parseClassFile(jf.data, jf.len, cf);

		int newlen = getClassFileSize(cf);

		ASSERT(newlen == jf.len,
				"Expected class file len %d, actual was %d, on class %s",
				jf.len, newlen, jf.name);
	};

	for (auto& jf : tests) {
		instr(jf);
	}
}

static void testIdentityParserWriter() {
	auto instr = [&](JavaFile& jf) {
		ClassFile cf;
		parseClassFile(jf.data, jf.len, cf);

		int newlen = getClassFileSize(cf);

		ASSERT(jf.len == newlen, "Expected class file len %d, "
				"actual was %d, on class %s",
				jf.len, newlen, jf.name);

		u1* newdata = new u1[newlen];

		writeClassFile(cf, newdata, newlen);

		for (int i = 0; i < newlen; i++) {
			ASSERT(jf.data[i] == newdata[i], "error on %d: "
					"%d:%d != %d:%d", i,
					jf.data[i],jf.data[i+1],
					newdata[i],newdata[i+1]
			);
		}

		delete [] newdata;
	};

	for (auto& jf : tests) {
		instr(jf);
	}
}

void testNopAdderInstrSize() {
	auto instr = [&](JavaFile& jf) {
		ClassFile cf;
		parseClassFile(jf.data, jf.len , cf);

		int methodsWithCode = 0;
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				Inst nop (OPCODE_nop);
				instList.push_front(nop);
				instList.push_front(nop);

				methodsWithCode++;
			}
		}

		int diff = methodsWithCode * 2;

		int newlen = getClassFileSize(cf);

		ASSERT(jf.len + diff == newlen,
				"Expected class file len %d, actual was %d, on class %s",
				jf.len, newlen, jf.name);

	};

	apply(instr);
}

void testNopAdderInstr() {
	auto instr = [&](JavaFile& jf) {
		ClassFile cf;
		parseClassFile(jf.data, jf.len , cf);

		int methodsWithCode = 0;
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				Inst nop (OPCODE_nop);
				instList.push_front(nop);
				instList.push_front(nop);

				methodsWithCode++;
			}
		}

		int diff = methodsWithCode * 2;

		int newlen = getClassFileSize(cf);

		ASSERT(jf.len + diff == newlen,
				"Expected class file len %d, actual was %d, on class %s",
				jf.len, newlen, jf.name);

		u1* newdata = new u1[newlen];
		writeClassFile(cf, newdata, newlen);

		ClassFile newcf;
		parseClassFile(newdata, newlen, newcf);

		int newlen2 = getClassFileSize(cf);

		ASSERT(newlen2 == newlen,
				"Expected class file len %d, actual was %d, on class %s",
				newlen2, newlen, jf.name);

		u1* newdata2 = new u1[newlen2];
		writeClassFile(cf, newdata2, newlen2);

		for (int i = 0; i < newlen2; i++) {
			ASSERT(newdata2[i] == newdata[i],
					"error on %d: %d:%d != %d:%d", i,
					newdata[i],newdata[i+1],
					newdata2[i],newdata2[i+1]
			);
		}

		delete [] newdata;

	};

	apply(instr);
}

#define RUN(test) ( fprintf(stderr, "Running test " #test "... "), \
	test(), fprintf(stderr, "[OK]\n") )

int main(int argc, const char* argv[]) {
	RUN(testPrinter);
	RUN(testIdentityComputeSize);
	RUN(testIdentityParserWriter);
	RUN(testNopAdderInstrSize);
	RUN(testNopAdderInstr);

	return 0;
}
