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
	for (const JavaFile& jf : tests) {
		instr(jf);
	}
}

static void testEmptyClassFilePrinter() {
	ClassFile cf("HolaQueTal", "ComoEstas");
	cout << cf;
}

static void testPrinter() {
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
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				instList.push_front(ZeroInst(OPCODE_nop));
				instList.push_front(ZeroInst(OPCODE_nop));

				methodsWithCode++;
			}
		}

		int diff = methodsWithCode * 2;

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
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				Inst* nop = new Inst(OPCODE_nop);
				instList.push_front(nop);
				instList.push_front(nop);

				methodsWithCode++;
			}
		}

		int diff = methodsWithCode * 2;

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

#define RUN(test) ( fprintf(stderr, "Running test " #test "... "), \
	test(), fprintf(stderr, "[OK]\n") )

int main(int, const char*[]) {
	RUN(testPrinter);
	RUN(testEmptyClassFilePrinter);
	RUN(testIdentityComputeSize);
	RUN(testIdentityParserWriter);
	RUN(testNopAdderInstrSize);
	RUN(testNopAdderInstr);

	return 0;
}
