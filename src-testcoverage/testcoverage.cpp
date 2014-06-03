/*
 * Includes
 */
#include "tests.hpp"

//#include <stdlib.h>
#include <ftw.h>
#include <iostream>
//#include <sstream>
#include <fstream>

using namespace std;
using namespace jnif;

list<JavaFile> tests;

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
		tests.push_back(jf);
	};

	if (isSuffix(".class", string(filePath))) {
		addJavaFile();
	}

	return 0;
}

#define ENTRY(testName) { &testName, #testName }

int main(int argc, const char* argv[]) {

	struct TestEntry {
		TestFunc* testFunc;
		String testName;
	};

	auto run = [](TestFunc* testFunc, const String& testName) {
		cerr << "Running test " << testName << " ";

		try {
			apply(cerr, tests, testFunc);
		} catch (const JnifException& ex) {
			cerr << ex << endl;
			exit(-255);
		}

		cerr << " [OK]" << endl;
	};

	vector<TestEntry> testEntries = {
	ENTRY(testPrinter),
	ENTRY(testSize),
	ENTRY(testWriter),
	ENTRY(testNopAdderInstrPrinter),
	ENTRY(testNopAdderInstrSize),
	ENTRY(testNopAdderInstrWriter),
	ENTRY(testAnalysis),
	ENTRY(testPrinterParserWithFrames),
	ENTRY(testIdentityComputeFrames),
	ENTRY(testIdentityWriterWithFrames),

	};

	String classPath;
	String testName;

	if (argc == 2 && argv[1] == String("--list")) {
		for (TestEntry& te : testEntries) {
			cout << te.testName << " ";
		}

		cout << endl;
		return 0;
	} else if (argc >= 2) {
		classPath = argv[1];
		testName = argc >= 3 ? argv[2] : "";

		cerr << "[Loading classes from " << classPath << "... " << flush;
		ftw(classPath.c_str(), visitFile, 50);

		cerr << "loaded " << tests.size() << " class(es)]" << endl;

		for (TestEntry& te : testEntries) {
			if (testName == "" || testName == te.testName) {
				run(te.testFunc, te.testName);

				if (testName == te.testName) {
					return 0;
				};
			}
		}

		if (testName != "") {
			cerr << "Test " << testName << " not found." << endl;
			return 4;
		}

		return 0;
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
