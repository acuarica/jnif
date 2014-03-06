/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include <jvmti.h>

//#include "frlog.h"
//#include "frexception.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "jnif.hpp"

using namespace std;
using namespace jnif;

static unsigned char* Allocate(jvmtiEnv* jvmti, jlong size) {

	unsigned char* memptr;

	jvmtiError error = jvmti->Allocate(size, &memptr);
	if (error != JVMTI_ERROR_NONE) {
		char* errnum_str = NULL;
		jvmti->GetErrorName(error, &errnum_str);

		FATAL("%sJVMTI: %d(%s): Unable to %s.\n", "err", error,
				(errnum_str == NULL ? "Unknown" : errnum_str),
				("c++ allocate"));

		exit(1);
	}

	return memptr;
}

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

void FrInstrClassFile0(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata) {

	if (string(className) != "frheapagent/HeapTest") {
		return;
	}

	ofstream os(outFileName(className, "disasm").c_str());

	ClassFile cf;
	parseClassFile(data, len, cf);
	printClassFile(cf, os);

	*newlen = getClassFileSize(cf);
	*newdata = Allocate(jvmti, *newlen);
	writeClassFile(cf, *newdata, *newlen);
}

void FrInstrClassFile(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlenp, unsigned char** newdatap) {

	ofstream os(outFileName(className, "disasm").c_str());

	ClassFile cf;
	parseClassFile(data, len, cf);
	printClassFile(cf, os);

	int newlen = getClassFileSize(cf);

	ASSERT(len == newlen,
			"Expected class file len %d, actual was %d, on class %s", len,
			newlen, className);

	u1* newdata = Allocate(jvmti, newlen);

	writeClassFile(cf, newdata, newlen);

	for (int i = 0; i < newlen; i++) {
		ASSERT(data[i] == newdata[i],
				"error at position %d in class %s: %d:%d != %d:%d", i,
				className, data[i], data[i + 1], newdata[i], newdata[i + 1]);
	}

	*newlenp = newlen;
	*newdatap = newdata;
}

void FrInstrClassFile3(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata) {

	if (string(className) != "frheapagent/HeapTest") {
		return;
	}

	ofstream os(outFileName(className, "disasm").c_str());

	ClassFile cf;
	parseClassFile(data, len, cf);
	printClassFile(cf, os);

	*newlen = getClassFileSize(cf);
	*newdata = Allocate(jvmti, *newlen);

	writeClassFile(cf, *newdata, *newlen);

}

}
