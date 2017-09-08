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

void apply(ostream& os, const list<JavaFile>& classes, TestFunc instr) {
    int i = 0;
    for (const JavaFile& jf : classes) {
        try {
            instr(jf);
        } catch (JnifException& ex) {
            os << "Error: Exception on class " << jf.name << endl;
            throw ex;
        }

        i++;
        if (i % 1000 == 0) {
            os << ".";
        }
    }
}

int main(int argc, const char* argv[]) {
    list<pair<string, TestFunc>> availableTests = {
        {"printer", &testPrinter},
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
        cerr << endl;
        cerr << "  where testI is one of the following: " << endl;
        for (auto& t : availableTests) {
            cerr << "    " << t.first << endl;
        }
        cerr << endl;
        return 1;
    }

    map<string, TestFunc> availableTestsLookup;
    for (const auto& t : availableTests) {
        availableTestsLookup[t.first] = t.second;
    }

    list<pair<string, TestFunc>> tests;
    list<string> jars;
    list<JavaFile> classes;

    for (int i = 1; i < argc; i++) {
        if (isSuffix(".jar", string(argv[i]))) {
            jars.push_back(argv[i]);
        } else {
            auto t = availableTestsLookup.find(argv[i]);
            if (t != availableTestsLookup.end()) {
                tests.push_back(make_pair(t->first, t->second));
            } else {
                cerr << "Test " << argv[i] << " not available." << endl;
                return 1;
            }
        }
    }

    if (tests.empty()) {
        for (const auto& t : availableTests) {
            tests.push_back(make_pair(t.first, t.second));
        }
    }

    for (const string& j : jars) {
        cout << "[Loading " << j << " .. " << flush;
        try {
            jnif::jar::JarFile uf(j.c_str());
            int csc = uf.forEach(&classes, 0, [] (void* classes, int, void* buf, int s, const char* fileNameInZip) {
                    u1* b = new u1[s];
                    memcpy(b, buf, s);
                    JavaFile jf = { b, s, fileNameInZip };
                    ((list<JavaFile>*)classes)->push_back(jf);
                });
            cout << csc << " classes OK]" << endl;
        } catch (const jnif::jar::JarException& ex) {
            cerr << "ERROR: Can't open file" << endl;
            return 1;
        }
    }

    for (auto& t : tests) {
        cout << "* Running test " << t.first << " " << flush;
        try {
            apply(cerr, classes, t.second);
            cout << " [OK]" << endl;
        } catch (const JnifException& ex) {
            cerr << ex << endl;
            return 1;
        }
    }
}
