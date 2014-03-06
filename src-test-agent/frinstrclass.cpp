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

void FrInstrClassFile1(jvmtiEnv* jvmti, unsigned char* data, int len,
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

void FrInstrClassFile2(jvmtiEnv* jvmti, unsigned char* data, int len,
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

void FrInstrClassFile(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata) {

//	if (string(className) != "frheapagent/HeapTest") {
//		return;
//	}
	if (string(className) != "java/lang/Object") {
		return;
	}

	ofstream os(outFileName(className, "disasm").c_str());

	ClassFile cf;
	parseClassFile(data, len, cf);
	printClassFile(cf, os);

	u2 classIndex = cf.cp.addClass("frproxy/FrInstrProxy");

//	u2 methodRefIndex = cf.cp.addMethodRef(classIndex, "alloc",
//			"(Ljava/lang/Object;)V");
//
//	u2 methodRefIndex2 = cf.cp.addMethodRef(classIndex, "newArrayEvent",
//			"(ILjava/lang/Object;I)V");
	u2 mindex = cf.cp.addMethodRef(classIndex, "enterMainMethod", "()V");

	auto newarray = [&](u1 atype) {
		Inst inst;
		inst.kind = KIND_NEWARRAY;
		inst.opcode = OPCODE_newarray;
		inst.newarray.atype = atype;

		return inst;
	};

	auto bipush = [&](u1 value) {
		Inst inst;
		inst.kind = KIND_BIPUSH;
		inst.opcode = OPCODE_bipush;
		inst.push.value = value;

		return inst;
	};

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst inst;
		inst.kind = KIND_INVOKE;
		inst.opcode = opcode;
		inst.invoke.methodRefIndex = index;

		return inst;
	};

	for (Method* m : cf.methods) {

		string name = cf.cp.getUtf8(m->nameIndex);

		if (m->hasCode() && m->nameIndex && name == "<init>") {
			InstList& instList = m->instList();

			instList.push_front(invoke(OPCODE_invokestatic, mindex));

//			InstList code;
//
//			for (Inst inst : instList) {
//				if (inst.opcode == OPCODE_newarray) {
//					// FORMAT: newarray atype
//					// OPERAND STACK: ... | count: int -> ... | arrayref
//
//					// STACK: ... | count
//
//					code.push_back(Inst(OPCODE_dup));
//					// STACK: ... | count | count
//
//					code.push_back(inst);
//					//code.push_back(newarray(inst.newarray.atype)); // newarray
//					// STACK: ... | count | arrayref
//
//					code.push_back(Inst(OPCODE_dup_x1));
//					// STACK: ... | arrayref | count | arrayref
//
//					code.push_back(bipush(inst.newarray.atype));
//					//u2 typeindex = instr.cp->addInteger(atype);
//
//					//bv.visitLdc(offset, OPCODE_ldc_w, typeindex);
//					// STACK: ... | arrayref | count | arrayref | atype
//
//					code.push_back(
//							invoke(OPCODE_invokestatic, methodRefIndex2));
//					// STACK: ... | arrayref
//
//				} else {
//					code.push_back(inst);
//				}
//			}
//
//			m->instList(code);
		}
	}

	*newlen = getClassFileSize(cf);
	*newdata = Allocate(jvmti, *newlen);

	writeClassFile(cf, *newdata, *newlen);

}

}
