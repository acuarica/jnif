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
	path << "build/instr/" << fileName << "." << ext;

	return path.str();
}

extern "C" {

void FrInstrClassFile(jvmtiEnv* jvmti, unsigned char* classFile,
		int classFileLen, const char* className, int* new_class_data_len,
		unsigned char** new_class_data) {

//	if (string(className) != "frheapagent/HeapTest") {
//		//return;
//	}

	auto instr =
			[&](ostream& os) {

				ClassWriterVisitor<> cwv(cout);
				ClassPrinterVisitor<decltype(cwv)> cpv(os, className, classFileLen, cwv);
//				InstrVisitor<decltype(cpv)> ii(cpv);
//				FullClassParser<decltype(ii)>().parse(classFile, classFileLen, ii);

				ClassParser::parse(classFile, classFileLen, cpv);

				int len = cwv.getClassFileSize(cwv.cf);

//				ASSERT(classFileLen == len, "%d must be equal to %d on class %s",
	//					classFileLen, len, className);

				*new_class_data_len = len;
				jvmtiError error = jvmti->Allocate(len, new_class_data );
				if (error != JVMTI_ERROR_NONE) {
					char* errnum_str = NULL;
					jvmti->GetErrorName(error, &errnum_str);

					FATAL("%sJVMTI: %d(%s): Unable to %s.\n", "err", error, (errnum_str == NULL ? "Unknown" : errnum_str),
							("c++ allocate"));

					exit(1);

				}

				BufferWriter bw(*new_class_data, len);
				cwv.writeClassFile(bw, cwv.cf);

				for (int i = 0; i < len; i++) {
					if (classFile[i] != (*new_class_data)[i]) {

						ASSERT(false, "error on %d: %d:%d != %d:%d", i,
								classFile[i],classFile[i+1],
								(*new_class_data)[i],(*new_class_data)[i+1]
						);
					}
				}
			};

	if (className == string("java/lang/Object")) {
		instr(cerr);
	} else {
		ofstream os(outFileName(className, "disasm").c_str());
		instr(os);
	}
}

}
