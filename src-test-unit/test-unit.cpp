/*
 * Includes
 */
#include <jnif.hpp>
#include <stdlib.h>
#include <ftw.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

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

list<JavaFile> tests4;
//
//void parallelFor(const unsigned int size,
//		std::function<void(const unsigned int)> func) {
//	const unsigned int nbThreads = std::thread::hardware_concurrency();
//	std::vector<std::thread> threads;
//	for (unsigned int idThread = 0; idThread < nbThreads; idThread++) {
//		auto threadFunc = [=, &threads]() {
//			for (unsigned int i=idThread; i<size; i+=nbThreads) {
//				func(i);
//			}
//		};
//		threads.push_back(std::thread(threadFunc));
//	}
//	for (auto & t : threads)
//		t.join();
//}

template<typename TFunc>
static void apply(TFunc instr) {

	//int ratio = tests4.size() / 50;
	int ratio = 100;
	int count = 0;
	auto invokeInstr = [&](const JavaFile& jf) {
		instr(jf);

		count++;

		if (count == ratio) {
			cerr << ".";
			count = 0;
		}
	};

//	for (const JavaFile& jf : tests) {
//		invokeInstr(jf);
//	}

	int i = 0;
	for (const JavaFile& jf : tests4) {
		invokeInstr(jf);

		i++;

		if (i == 1000) {

		}
	}
}

template<typename TTestFunc>
static void _run(TTestFunc test, const char* testName) {
	cerr << "Running test " << testName << " ";

	try {
		test();
	} catch (const JnifException& ex) {
		cerr << ex << endl;
		throw ex;
	}

	cerr << " [OK]" << endl;
}

#define run(test) _run(test, #test);

// ./instr/
static string outFileName(const String& className, const char* ext,
		const char* prefix = "") {
	string fileName = className;

//	for (u4 i = 0; i < fileName.length(); i++) {
//		fileName[i] = className[i] == '/' ? '.' : className[i];
//	}

	stringstream path;
	path << prefix << fileName << "." << ext;

	//cout << path.str() << endl;
	return path.str();
}

class UnitTestClassPath: public IClassPath {
public:

	String getCommonSuperClass(const String& className1,
			const String& className2) {
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

static void testPrinterParser() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);
		ofstream os(outFileName(jf.name, "disasm").c_str());
		os << cf;
	});
}

static void testPrinterParserWithFrames() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		UnitTestClassPath cp;
		cf.computeFrames(&cp);

		ofstream os(outFileName(jf.name, "frames.disasm").c_str());
		os << cf;
	});
}

static void testIdentityComputeSize() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int newlen = cf.computeSize();

		ASSERT(newlen == jf.len,
				"Expected class file len %d, actual was %d, on class %s",
				jf.len, newlen, jf.name.c_str());
	});
}

static void testIdentityComputeFrames() {
	apply([](const JavaFile& jf) {
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
	apply([](const JavaFile& jf) {
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
	apply([](const JavaFile& jf) {
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
static void testNopAdderInstrSize() {
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int methodsWithCode = 0;
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				//InstList& instList =
			m->instList();

			//	Inst* nop = new Inst(OPCODE_nop);

			// If there is a tableswitch or a lookupswitch instruction
			// bytes added to the instruction flow must be a multiple
			// of four to keep the padding in this instructions.
//			instList.push_front(nop);
//			instList.push_front(nop);
//			instList.push_front(nop);
//			instList.push_front(nop);

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
	apply([](const JavaFile& jf) {
		ClassFile cf(jf.data, jf.len);

		int methodsWithCode = 0;
		for (Method* m: cf.methods) {
			if (m->hasCode()) {
				//InstList& instList =
			m->instList();

//				Inst* nop = new Inst(OPCODE_nop);
//				instList.push_front(nop);
//				instList.push_front(nop);
//				instList.push_front(nop);
//				instList.push_front(nop);

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

int main(int, const char*[]) {
	ftw("./", visitFile, 50);

	cerr << "[Loaded " << tests4.size() << " classes]" << endl;

	run(testPrinterModel);
	//run(testPrinterParser);
	//run(testPrinterParserWithFrames);
	run(testIdentityComputeSize);
	run(testIdentityComputeFrames);
	run(testIdentityParserWriter);
	run(testIdentityParserWriterWithFrames);
	//run(testNopAdderInstrSize);
	//run(testNopAdderInstr);

	return 0;
}
