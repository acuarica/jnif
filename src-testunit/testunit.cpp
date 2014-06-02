/*
 * Includes
 */
#include <jnif.hpp>
#include <stdlib.h>
#include <ftw.h>
#include <iostream>
#include <sstream>
#include <fstream>

static inline int _exception(int) __attribute__((noreturn));

static inline int _exception(int) {
	exit(1);
}

#define ASSERT(cond, format, ...) ( (cond) ? 0 : _exception(fprintf(stderr, \
			"ASSERT | '" #cond "' failed | " format "\n", ##__VA_ARGS__ )))

using namespace std;
using namespace jnif;

class JavaFile {
public:
	const u1* const data;
	const int len;
	const String name;
};

list<JavaFile> tests4;

template<typename TFunc>
static void apply(const char* action, TFunc instr) {

	//int ratio = tests4.size() / 50;
	int ratio = 100;
	int count = 0;
	auto invokeInstr = [&](const JavaFile& jf) {
		cout << action << ":"<< jf.name << ":" << jf.len << flush;
		instr(jf);

		count++;

		if (count == ratio) {
			cerr << ".";
			count = 0;
		}

		cout << endl;
	};

	int i = 0;
	for (const JavaFile& jf : tests4) {
		invokeInstr(jf);

		i++;

		if (i == 1000) {

		}
	}
}

static string outFileName(const String& className, const char* ext) {
	stringstream path;
	path << className << "." << ext;
	return path.str();
}

class UnitTestClassPath: public IClassPath {
public:

	String getCommonSuperClass(const String&, const String&) {
		return "java/lang/Object";
	}

};

static void testPrinterModel() {
	ClassFile emptyCf("jnif/test/generated/Class1");
	ofstream os(
			outFileName("jnif/test/generated/Class1", "model.disasm").c_str());
	os << emptyCf;

	ClassFile cf2("jnif/test/generated/Class2", "jnif/test/generated/Class");
	cf2.addMethod("main", "([Ljava/lang/String;)V",
			METHOD_FINAL | METHOD_PUBLIC);
	ofstream os2(
			outFileName("jnif/test/generated/Class2", "model.disasm").c_str());
	os2 << cf2;
}

static void testParser() {
	apply("parser", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);
	});
}

static void testPrinterParser() {
	apply("printerparser", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);
		ofstream os(outFileName(jf.name, "disasm").c_str());
		os << cf;
	});
}

static void testPrinterParserWithFrames() {
	apply("p", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		UnitTestClassPath cp;
		cf.computeFrames(&cp);

		ofstream os(outFileName(jf.name, "frames.disasm").c_str());
		os << cf;
	});
}

static void testIdentityComputeSize() {
	apply("parser+computeSize", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int newlen = cf.computeSize();

		ASSERT(newlen == jf.len,
				"Expected class file len %d, actual was %d, on class %s",
				jf.len, newlen, jf.name.c_str());
	});
}

static void testIdentityComputeFrames() {
	apply("parser+computeFrames", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		UnitTestClassPath cp;
		cf.computeFrames(&cp);

		//int newlen = cf.computeSize();

			//ASSERT(newlen == jf.len,
			//	"Expected class file len %d, actual was %d, on class %s",
			//jf.len, newlen, jf.name.c_str());
		});
}

static void testIdentityParserWriter() {
	apply("parser+writer", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int newlen = cf.computeSize();

		ASSERT(jf.len == newlen, "Expected class file len %d, "
				"actual was %d, on class %s",
				jf.len, newlen, jf.name.c_str());

		u1* newdata = new u1[newlen];

		cf.write(newdata, newlen);

		for (int i = 1; i < newlen; i++) {
			if (jf.data[i] != newdata[i]) {
				cerr << "Validation failed!" << endl;
				cf.write(newdata, i+1);
			}

			ASSERT(jf.data[i] == newdata[i], "error on %d: "
					"%d:%d:%d != %d:%d:%d @ class: %s", i,
					jf.data[i-1],jf.data[i],jf.data[i+1],
					newdata[i-1],newdata[i],newdata[i+1],jf.name.c_str()
			);
		}

		delete [] newdata;
	});
}

static void testIdentityParserWriterWithFrames() {
	apply("2", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		//cf.computeFrames();

			int newlen = cf.computeSize();

			ASSERT(jf.len == newlen, "Expected class file len %d, "
					"actual was %d, on class %s",
					jf.len, newlen, jf.name.c_str());

			u1* newdata = new u1[newlen];

			cf.write(newdata, newlen);

			for (int i = 0; i < newlen; i++) {
				ASSERT(jf.data[i] == newdata[i], "error on %d: "
						"%d:%d != %d:%d, on class %s", i,
						jf.data[i],jf.data[i+1],
						newdata[i],newdata[i+1], jf.name.c_str()
				);
			}

			delete [] newdata;
		});
}
static void testNopAdderInstrComputeSizeSize() {
	apply("NopAdderInstrComputeSizeSize", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int methodsWithCode = 0;
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				InstList& instList =
				m->instList();

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

	int diff = methodsWithCode * 4;

	int newlen = cf.computeSize();

	ASSERT(jf.len + diff == newlen,
			"Expected class file len %d, actual was %d, on class %s",
			jf.len+diff, newlen, jf.name.c_str());

});
}

static void testNopAdderInstr() {
	apply("2", [](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int methodsWithCode = 0;
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				InstList& instList =
				m->instList();

				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);

				methodsWithCode++;
			}
		}

		int diff = methodsWithCode * 4;

		int newlen = cf.computeSize();

		ASSERT(jf.len + diff == newlen,
				"Expected class file len %d, actual was %d, on class %s",
				jf.len, newlen, jf.name.c_str());

		u1* newdata = new u1[newlen];
		cf.write(newdata, newlen);

		ClassFile newcf(newdata, newlen);

		int newlen2 = cf.computeSize();

		ASSERT(newlen2 == newlen,
				"Expected class file len %d, actual was %d, on class %s",
				newlen2, newlen, jf.name.c_str());

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

static void testException() {
//	try {
//		JnifException ex();
//		//ex << "arg1: " << 1 << ", arg2: " << "2" << ", arg3: " << 3.4;
//
//		throw ex;
//	} catch (const JnifException& ex) {
//		cerr << ex << endl;
//	}
}

static void testJoinFrameObjectAndEmpty() {
	UnitTestClassPath cp;

	const Type& s = Type::objectType("TypeS");
	const Type& t = Type::objectType("TypeT");

	Frame lhs;
	lhs.setVar2(0, s);
	lhs.setVar2(1, t);

	Frame rhs;
	rhs.setVar2(0, s);

	lhs.join(rhs, &cp);

	Frame res;
	res.lva.resize(2, Type::topType());
	res.setVar2(0, s);

	Error::assertEquals(res, lhs);
}

static void testJoinFrameException() {
	UnitTestClassPath cp;

	const Type& classType = Type::objectType("testunit/Class");
	const Type& exType = Type::objectType("java/lang/Exception");

	Frame lhs;
	lhs.setVar2(0, Type::nullType());
	lhs.setVar2(1, exType);

	Frame rhs;
	rhs.setVar2(0, classType);

	lhs.join(rhs, &cp);

	cerr << lhs;

	Frame res;
	res.lva.resize(2, Type::topType());
	res.setVar2(0, classType);
	//res.setVar2(1, Type::topType());

	Error::assertEquals(res, lhs);
}

static void testJoinFrame() {
	UnitTestClassPath cp;

	ClassFile cf("testunit/Class");
	Method* m = cf.addMethod("method", "()Ltestunit/Class;",
			METHOD_PUBLIC | METHOD_STATIC);
	auto cidx = cf.addUtf8("Code");
	CodeAttr* code = new CodeAttr(cidx, &cf);
	m->add(code);
	InstList& instList = m->codeAttr()->instList;

	auto idx = cf.addClass("testunit/Class");
	auto exidx = cf.addClass("java/lang/Exception");
	auto initidx = cf.addMethodRef(idx, "<init>", "()V");

	auto l = instList.createLabel();

	auto tryLabel = instList.createLabel();
	tryLabel->isTryStart = true;

	auto endLabel = instList.createLabel();
	endLabel->isTryEnd = true;

	auto handlerLabel = instList.createLabel();
	handlerLabel->isCatchHandler = true;

	instList.addLabel(tryLabel);
	instList.addType(OPCODE_new, idx);
	instList.addZero(OPCODE_dup);
	instList.addZero(OPCODE_iadd);
	instList.addInvoke(OPCODE_invokespecial, initidx);
	instList.addZero(OPCODE_astore_0);
	instList.addLabel(endLabel);
	instList.addJump(OPCODE_goto, l);
	instList.addLabel(handlerLabel);
	instList.addZero(OPCODE_astore_1);
	instList.addZero(OPCODE_aconst_null);
	instList.addZero(OPCODE_astore_0);
	instList.addLabel(l);
	instList.addZero(OPCODE_aload_0);
	instList.addZero(OPCODE_areturn);

	CodeExceptionEntry ex;
	ex.startpc = tryLabel;
	ex.endpc = endLabel;
	ex.handlerpc = handlerLabel;
	ex.catchtype = exidx;
	code->exceptions.push_back(ex);

	cf.computeFrames(&cp);

//	u1* data;
//	int len;
//	cf.write(&data, &len, [&](u4 size) {return new u1[size];});
//	ClassFile cf2(data, len);
//	cf2.computeFrames(&cp);
//	cerr << cf2;
}

static int visitFile(const char* filePath, const struct stat*, int) {

	auto isSuffix = [&](const string& suffix, const string& text) {
		auto res = std::mismatch(suffix.rbegin(), suffix.rend(), text.rbegin());
		return res.first == suffix.rend();
	};

	auto addJavaFile = [&]() {
		ifstream is(filePath, ios::in | ios::binary | ios::ate);

		if (!is.is_open()) {
			int m;
			is >> m;
			cerr << "Erro on opening file: " << m << endl;
			throw "File not opened!";
		}

		int fileSize = is.tellg();
		u1* buffer = new u1[fileSize];

		is.seekg(0, ios::beg);
		if (!is.read((char*) buffer, fileSize)) {
			cerr << "fail to read" << endl;
			throw "File not opened!";
		}

		JavaFile jf= {buffer,fileSize,filePath};
		tests4.push_back(jf);
	};

	if (isSuffix(".class", string(filePath))) {
		addJavaFile();
	}

	return 0;
}

int main(int argc, const char* argv[]) {

#define ENTRY(testName) { &testName, #testName }

	typedef void (TestFunc)();

	struct TestEntry {
		TestFunc* testFunc;
		String testName;
	};

	auto run = [](TestFunc* testFunc, const String& testName) {
		cerr << "Running test " << testName << " ";

		try {
			testFunc();
		} catch (const JnifException& ex) {
			cerr << ex << endl;
			//throw ex;
			exit(-2);
		}

		cerr << " [OK]" << endl;
	};

	run(&testException, "testException");
	run(&testJoinFrameObjectAndEmpty, "testFrame");
	run(&testJoinFrameException, "testJoinFrameException");
	run(&testJoinFrame, "testJoinFrame");
	run(&testIdentityParserWriter,"testIdentityParserWriter");
	return 0;

	vector<TestEntry> testEntries = { ENTRY(testPrinterModel),
	ENTRY(testParser), ENTRY(testPrinterParser),
	ENTRY(testPrinterParserWithFrames),
	ENTRY(testIdentityComputeSize),
	ENTRY(testIdentityComputeFrames),
	ENTRY(testIdentityParserWriter),
	ENTRY(testIdentityParserWriterWithFrames),
	ENTRY(testNopAdderInstrComputeSizeSize),
	ENTRY(testNopAdderInstr) };

	String classPath;
	String testName;

	if (argc == 2 && argv[1] == String("--list")) {
		for (TestEntry& te : testEntries) {
			cout << te.testName << " ";
		}
		cout << endl;
		return 0;
	} else if (argc == 2) {
		classPath = argv[1];
		//testName = argv[2];
		cerr << "[Loading classes from " << classPath << "... " << flush;

		ftw(classPath.c_str(), visitFile, 50);

		cerr << "loaded " << tests4.size() << " class(es)]" << endl;

		for (TestEntry& te : testEntries) {
			//if (testName == te.testName) {
			run(te.testFunc, te.testName);
			//return 0;
			//}
		}

		return 0;
		//return 2;
	} else {
		cerr << "Usage: " << endl;
		cerr << "  [1] " << argv[0] << " <classPath> <testName>" << endl;
		cerr << "  [2] " << argv[0] << " --list" << endl;
		cerr << "Available tests: " << endl;
		for (TestEntry& te : testEntries) {
			cerr << "  " << te.testName << endl;
		}

		return 1;
	}
}
