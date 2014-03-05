/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include <jvmti.h>

#include "frlog.h"
#include "frexception.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "../src/jnif.hpp"
#include "InstrVisitor.hpp"

using namespace std;
using namespace jnif;

static string outFileName(const char* className, const char* ext) {
	string fileName = className;

	for (u4 i = 0; i < fileName.length(); i++) {
		fileName[i] = className[i] == '/' ? '.' : className[i];
	}

	stringstream path;
	path << "../build/instr/" << fileName << "." << ext;

	return path.str();
}

extern "C" {

void FrInstrClassFile(jvmtiEnv* jvmti, unsigned char* classFile,
		int classFileLen, const char* className, int* new_class_data_len,
		unsigned char** new_class_data) {

	if (string(className) != "frheapagent/HeapTest") {
		return;
	}

	auto instr =
			[&](ostream& os) {

				ClassWriterVisitor<> cwv(cout);
				ClassPrinterVisitor<decltype(cwv)> cpv(os, className, classFileLen, cwv);
				InstrVisitor<decltype(cpv)> ii(cpv);

				ClassParser::parse(classFile, classFileLen, ii);

				int len = cwv.getClassFileSize(cwv.cf);

				*new_class_data_len = len;
				jvmtiError error = jvmti->Allocate(len, new_class_data );
				if (error != JVMTI_ERROR_NONE) {
					char* errnum_str = NULL;
					jvmti->GetErrorName(error, &errnum_str);

					FATAL("%sJVMTI: %d(%s): Unable to %s.\n", "err", error, (errnum_str == NULL ? "Unknown" : errnum_str),
							("c++ allocate"));

					exit(1);
				}

				BaseWriter bw(*new_class_data, len);
				cwv.writeClassFile(bw, cwv.cf);
			};

	ofstream os(outFileName(className, "disasm").c_str());
	instr(os);
}

}
