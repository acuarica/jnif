/*
 * Includes
 */
#include <jnif.hpp>
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

#define JAVAFILE(name) extern u1 jnif_##name ## _class[]; \
	extern int jnif_##name ## _class_len; \
	static JavaFile jnif##name = { jnif_##name ## _class, jnif_##name ## _class_len, "jnif/" # name }

JAVAFILE(TestAbs);
JAVAFILE(ExceptionClass);
JAVAFILE(BasicClass);
JAVAFILE(TestProxy);
JAVAFILE(MainClass);

JavaFile tests[] = { jnifTestAbs, jnifExceptionClass, jnifBasicClass,
		jnifTestProxy, jnifMainClass };

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

static void testPrinterModel() {
	ClassFile emptyCf("jnif/test/generated/Class1");
	cout << emptyCf;

	ClassFile cf2("jnif/test/generated/Class2", "jnif/test/generated/Class");
	cf2.addMethod("main", "([Ljava/lang/String;)V",
			METHOD_FINAL | METHOD_PUBLIC);
	cout << cf2;
}

static void testPrinterParser() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);
		cout << cf;
	});
}

static void testPrinterParserWithFrames() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		cout << "Computing frames on class " << cf.getThisClassName() << endl;
		cf.computeFrames();
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

static void testIdentityComputeSizeWithFrames() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		cf.computeFrames();
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
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

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

int main(int, const char*[]) {
	run(testPrinterModel);
	run(testPrinterParser);
	run(testPrinterParserWithFrames);
	run(testIdentityComputeSize);
	run(testIdentityComputeSizeWithFrames);
	run(testIdentityParserWriter);
	run(testNopAdderInstrSize);
	run(testNopAdderInstr);

	return 0;
}
