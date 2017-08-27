/*
 * Includes
 */
#include <jnif.hpp>
#include "tests.hpp"

#include <ftw.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace jnif;


typedef void (*TestFunc)(const JavaFile& jf);

static bool isSuffix(const string& suffix, const string& text) {
	auto res = std::mismatch(suffix.rbegin(), suffix.rend(), text.rbegin());
	return res.first == suffix.rend();
}

void apply(ostream& os, const JavaFile& jf, TestFunc instr) {
    // int i = 0;
    // for (const JavaFile& jf : jfs) {
        try {
            instr(jf);
        } catch (JnifException& ex) {
            os << "Error: Exception on class " << jf.name << endl;
            throw ex;
        }

        // i++;
        // if (i % 1000 == 0) {
        //     os << ".";
        // }
    // }
}

int main(int argc, const char* argv[]) {
    map<string, TestFunc> availableTests = {
        {"print", &testPrinter},
        {"size", &testSize},
        {"writer", &testWriter},
        {"analysis", &testAnalysis},
        {"analysisPrinter", &testAnalysisPrinter},
        {"analysisWriter", &testAnalysisWriter},
        {"nopAdderInstrPrinter", &testNopAdderInstrPrinter},
        {"nopAdderInstrSize", &testNopAdderInstrSize},
        {"nopAdderInstrWriter", &testNopAdderInstrWriter},
        {"nopAdderInstrAnalysisPrinter", &testNopAdderInstrAnalysisPrinter},
        {"nopAdderInstrAnalysisWriter", &testNopAdderInstrAnalysisWriter}
    };

    if (argc == 1) {
        cerr << "Usage: " << endl;
        cerr << "  " << argv[0];
        cerr << " [test1..testN] <j1>.jar [<j2>.jar..<jM>.jar]" << endl;
        cerr << "Available tests: " << endl;
        for (auto& t : availableTests) {
            cerr << "  " << t.first << endl;
        }
        cout << endl;
        return 1;
    }

    map<string, TestFunc> tests;
    list<string> jars;
    list<JavaFile> classes;

    for (int i = 1; i < argc; i++) {
        if (isSuffix(".jar", string(argv[i]))) {
            jars.push_back(argv[i]);
        } else {
            if (availableTests.find(argv[i]) != availableTests.end()) {
                // tests.push_back(argv[i]);
            } else {
                cerr << "Test " << argv[i] << " not available." << endl;
                return 1;
            }
        }
    }

    if (tests.empty()) {
        for (auto& t : availableTests) {
            tests[t.first] = t.second;
        }
    }

    for (const string& j : jars) {
        cout << "[Processing " << j << " .. " << flush;
        try {
            JarFile uf(j.c_str());
            int csc = uf.forEach(&tests, 0, [] (void* tests, int, void* buf, int s, const char* fileNameInZip) {
                    JavaFile jf = { (u1*)buf, s, fileNameInZip };
                    // cout << "Class " << fileNameInZip << ": " << flush;
                    for (auto& t : *((map<string, TestFunc>*)tests)) {
                        // cout << t.first << " " << flush;
                        try {
                            apply(cerr, jf, t.second);
                        } catch (const JnifException& ex) {
                            cerr << ex << endl;
                            exit(1);
                        }
                    }
                });
            cout << csc << " classes OK]" << endl;
        } catch (const JarException& ex) {
            cerr << "ERROR: Can't open file" << endl;
            return 1;
        }
    }
}
