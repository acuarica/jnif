/*
 * tests.hpp
 *
 *  Created on: Jun 2, 2014
 *      Author: luigi
 */

#include "apply.hpp"

#include <fstream>

using namespace std;
using namespace jnif;

//static string outFileName(const String& className, const char* ext) {
//	stringstream path;
//	path << className << "." << ext;
//	return path.str();
//}

template<typename T>
void assertEquals(const T* lhsData, int lhsLen, const T* rhsData, int rhsLen) {
	Error::assertEquals(lhsLen, rhsLen, "Invalid len for arrays");

	for (int i = 0; i < lhsLen; i++) {
		Error::assertEquals(lhsData[i], rhsData[i], "Data difers at pos:", i);
	}
}

class UnitTestClassPath: public IClassPath {
public:

	String getCommonSuperClass(const String&, const String&) {
		return "java/lang/Object";
	}

};

class NopAdderInstr {
public:

	int diff;

	NopAdderInstr(ClassFile& cf) :
			diff(0) {
		int methodsWithCode = 0;
		for (Method* m : cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				// If there is a tableswitch or a lookupswitch instruction
				// bytes added to the instruction flow must be a multiple
				// of four to keep the padding in this instructions.
				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);

				methodsWithCode++;
			}
		}

		diff = methodsWithCode * 4;
	}
};

void testPrinter(const JavaFile&, ClassFile& cf) {
	fstream os;
	os << cf;
}

void testSize(const JavaFile& jf, ClassFile& cf) {
	int newlen = cf.computeSize();

	Error::assertEquals(newlen, jf.len);
}

void testWriter(const JavaFile& jf, ClassFile& cf) {
	int newlen = cf.computeSize();

	Error::assertEquals(newlen, jf.len);

	u1* newdata = new u1[newlen];

	cf.write(newdata, newlen);

	assertEquals(jf.data, jf.len, newdata, newlen);

	delete[] newdata;
}

void testNopAdderInstrPrinter(const JavaFile&, ClassFile& cf) {
	NopAdderInstr instr(cf);
	fstream os;
	os << cf;
}

void testNopAdderInstrSize(const JavaFile& jf, ClassFile& cf) {
	NopAdderInstr instr(cf);
	int newlen = cf.computeSize();

	Error::assertEquals(jf.len + instr.diff, newlen);
}

void testNopAdderInstrWriter(const JavaFile& jf, ClassFile& cf) {
	NopAdderInstr instr(cf);

	int newlen = cf.computeSize();

	Error::assertEquals(jf.len + instr.diff, newlen);

	u1* newdata = new u1[newlen];
	cf.write(newdata, newlen);

	ClassFile newcf(newdata, newlen);

	int newlen2 = cf.computeSize();

	Error::assertEquals(newlen2, newlen);

	u1* newdata2 = new u1[newlen2];
	cf.write(newdata2, newlen2);

	assertEquals(newdata, newlen, newdata2, newlen2);

	delete[] newdata;
}

void testAnalysis(const JavaFile&, ClassFile& cf) {
	UnitTestClassPath cp;
	cf.computeFrames(&cp);
}

void testPrinterParserWithFrames(const JavaFile&, ClassFile& cf) {
	UnitTestClassPath cp;
	cf.computeFrames(&cp);

	//ofstream os(outFileName(jf.name, "frames.disasm").c_str());
	ofstream os;
	os << cf;
}

void testIdentityComputeFrames(const JavaFile&, ClassFile& cf) {
	UnitTestClassPath cp;
	cf.computeFrames(&cp);

	//int newlen = cf.computeSize();

	//ASSERT(newlen == jf.len,
	//	"Expected class file len %d, actual was %d, on class %s",
	//jf.len, newlen, jf.name.c_str());
}

void testIdentityWriterWithFrames(const JavaFile& jf, ClassFile& cf) {
	//cf.computeFrames();

	int newlen = cf.computeSize();

	ASSERT(jf.len == newlen,
			"Expected class file len %d, " "actual was %d, on class %s", jf.len,
			newlen, jf.name.c_str());

	u1* newdata = new u1[newlen];

	cf.write(newdata, newlen);

	for (int i = 0; i < newlen; i++) {
		ASSERT(jf.data[i] == newdata[i],
				"error on %d: " "%d:%d != %d:%d, on class %s", i, jf.data[i],
				jf.data[i + 1], newdata[i], newdata[i + 1], jf.name.c_str());
	}

	delete[] newdata;
}

