/*
 * Includes
 */
#include "jnif.hpp"

#include <stdlib.h>
#include <iostream>

static inline int _exception(int) __attribute__((noreturn));

static inline int _exception(int) {
	exit(1);
}

#define ASSERT(cond, format, ...) ( (cond) ? 0 : _exception(fprintf(stderr, \
			"ASSERT | '" #cond "' failed | " format "\n", ##__VA_ARGS__ )))

using namespace std;
using namespace jnif;

struct JavaFile {
	u1* data;
	int len;
	const char* name;
};

extern u1 jnif_BasicClass_class[];
extern int jnif_BasicClass_class_len;

//extern u1 jnif_ExceptionClass_class[];
//extern int jnif_ExceptionClass_class_len;
//
//extern u1 jnif_TestProxy_class[];
//extern int jnif_TestProxy_class_len;
//
//extern u1 classes_java_io_OutputStreamWriter_class[];
//extern int classes_java_io_OutputStreamWriter_class_len;

JavaFile tests[] = {

{ jnif_BasicClass_class, jnif_BasicClass_class_len, "jnif/BasicClass" },

//{ jnif_ExceptionClass_class, jnif_ExceptionClass_class_len,
//		"jnif/ExceptionClass" },

//{ jnif_TestProxy_class, jnif_TestProxy_class_len, "jnif/TestProxy" },

//{ classes_java_io_OutputStreamWriter_class,
//		classes_java_io_OutputStreamWriter_class_len,
//		"java/io/OutputStreamWriter" },

		};

template<typename TFunc>
static void apply(TFunc instr) {
	for (const JavaFile& jf : tests) {
		instr(jf);
	}
}

template<typename TTestFunc>
static void _run(TTestFunc test, const char* testName) {
	fprintf(stderr, "Running test %s... ", testName);
	test();
	fprintf(stderr, "[OK]\n");
}

#define run(test) _run(test, #test);

static void testPrinter() {
	ClassFile cf1("jnif/test/generated/Class1");
	cout << cf1;

	ClassFile cf2("jnif/test/generated/Class2", "jnif/test/generated/Class");
	Method& m = cf2.addMethod("main", "([Ljava/lang/String;)V");
	//m.

	cout << cf2;

	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);
		cout << cf;
	});
}

static void testIdentityComputeSize() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int newlen = cf.computeSize();

		ASSERT(newlen == jf.len,
				"Expected class file len %d, actual was %d, on class %s",
				jf.len, newlen, jf.name);
	});
}

static void testIdentityParserWriter() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int newlen = cf.computeSize();

		ASSERT(jf.len == newlen, "Expected class file len %d, "
				"actual was %d, on class %s",
				jf.len, newlen, jf.name);

		u1* newdata = new u1[newlen];

		cf.write(newdata, newlen);

		for (int i = 0; i < newlen; i++) {
			ASSERT(jf.data[i] == newdata[i], "error on %d: "
					"%d:%d != %d:%d", i,
					jf.data[i],jf.data[i+1],
					newdata[i],newdata[i+1]
			);
		}

		delete [] newdata;
	});
}

static void testNopAdderInstrSize() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int methodsWithCode = 0;
		for (Method& m: cf.methods) {
			if (m.hasCode()) {
				InstList& instList = m.instList();

				Inst* nop = new Inst(OPCODE_nop);

				// If there is a tableswitch or a lookupswitch instruction
				// bytes added to the instruction flow must be a multiple
				// of four to keep the padding in this instructions.
			instList.push_front(nop);
			instList.push_front(nop);
			instList.push_front(nop);
			instList.push_front(nop);

			methodsWithCode++;
		}
	}

	int diff = methodsWithCode * 4;

	int newlen = cf.computeSize();

	ASSERT(jf.len + diff == newlen,
			"Expected class file len %d, actual was %d, on class %s",
			jf.len, newlen, jf.name);

});
}

static void testNopAdderInstr() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int methodsWithCode = 0;
		for (Method& m: cf.methods) {
			if (m.hasCode()) {
				InstList& instList = m.instList();

				Inst* nop = new Inst(OPCODE_nop);
				instList.push_front(nop);
				instList.push_front(nop);
				instList.push_front(nop);
				instList.push_front(nop);

				methodsWithCode++;
			}
		}

		int diff = methodsWithCode * 4;

		int newlen = cf.computeSize();

		ASSERT(jf.len + diff == newlen,
				"Expected class file len %d, actual was %d, on class %s",
				jf.len, newlen, jf.name);

		u1* newdata = new u1[newlen];
		cf.write(newdata, newlen);

		ClassFile newcf(newdata, newlen);

		int newlen2 = cf.computeSize();

		ASSERT(newlen2 == newlen,
				"Expected class file len %d, actual was %d, on class %s",
				newlen2, newlen, jf.name);

		u1* newdata2 = new u1[newlen2];
		cf.write(newdata2, newlen2);

		for (int i = 0; i < newlen2; i++) {
			ASSERT(newdata2[i] == newdata[i],
					"error on %d: %d:%d != %d:%d", i,
					newdata[i],newdata[i+1],
					newdata2[i],newdata2[i+1]
			);
		}

		delete [] newdata;

	});
}

#define PSIZEOF(typeExpr) fprintf(stderr, "sizeof(" #typeExpr "): %ld\n", sizeof(typeExpr))

int main(int, const char*[]) {
	run(testPrinter);
	run(testIdentityComputeSize);
	run(testIdentityParserWriter);
	run(testNopAdderInstrSize);
	run(testNopAdderInstr);

	PSIZEOF(vector<int>);
	PSIZEOF(std::string);
	PSIZEOF(ConstPool);
	PSIZEOF(ConstPoolEntry);
	PSIZEOF(Opcode);
	PSIZEOF(OpKind);
	PSIZEOF(Inst);
	PSIZEOF(InstList);

	return 0;
}
