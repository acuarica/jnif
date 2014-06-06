/*
 * Includes
 */
#include "tests.hpp"

#include <ftw.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace jnif;

list<JavaFile> tests;

static bool isSuffix(const string& suffix, const string& text) {
	auto res = std::mismatch(suffix.rbegin(), suffix.rend(), text.rbegin());
	return res.first == suffix.rend();
}

static int visitFile(const char* filePath, const struct stat*, int) {
	if (isSuffix(".class", string(filePath))) {
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

		JavaFile jf = { buffer, fileSize, filePath };
		tests.push_back(jf);
	}

	return 0;
}

struct TestEntry {
	TestFunc* testFunc;
	String testName;
};

#define ENTRY(testName) { &testName, #testName }
#define SIZE(arr) ( sizeof(arr)/sizeof((arr)[0]) )

int main(int argc, const char* argv[]) {

	TestEntry testEntries[] = {
	//ENTRY(testPrinter),
	ENTRY(testSize),
	ENTRY(testWriter),
	ENTRY(testAnalysis),
	ENTRY(testAnalysisPrinter),
	ENTRY(testAnalysisWriter),
	ENTRY(testNopAdderInstrPrinter),
	ENTRY(testNopAdderInstrSize),
	ENTRY(testNopAdderInstrWriter),
	ENTRY(testNopAdderInstrAnalysisPrinter),
	ENTRY(testNopAdderInstrAnalysisWriter),

	};

	String classPath;
	String testName;

	if (argc == 2 && argv[1] == String("--list")) {
		for (u4 i = 0; i < SIZE(testEntries); i++) {
			cout << testEntries[i].testName << " ";
		}

		cout << endl;
		return 0;
	} else if (argc >= 2) {
		classPath = argv[1];
		testName = argc >= 3 ? argv[2] : "";

		cerr << "[Loading classes from " << classPath << "... " << flush;
		ftw(classPath.c_str(), visitFile, 50);

		cerr << "loaded " << tests.size() << " class(es)]" << endl;

		for (u4 i = 0; i < SIZE(testEntries); i++) {
			TestEntry& te = testEntries[i];

			if (testName == "" || testName == te.testName) {
				cerr << "Running test " << te.testName << " ";

				try {
					apply(cerr, tests, te.testFunc);
				} catch (const JnifException& ex) {
					cerr << ex << endl;
					exit(-255);
				}

				cerr << " [OK]" << endl;

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
