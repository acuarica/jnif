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

static string outFileName(const char* className, const char* ext,
		const char* prefix = "$") {
	string fileName = className;

	for (u4 i = 0; i < fileName.length(); i++) {
		fileName[i] = className[i] == '/' ? '.' : className[i];
	}

	stringstream path;
	path << prefix << fileName << "." << ext;

	return path.str();
}

extern "C" {

void InstrClassEmpty(jvmtiEnv*, u1*, int, const char*, int*, u1**) {
}

void InstrClassDump(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**) {

	ofstream os(outFileName(className, "class").c_str(), ios::binary);
	os.write((char*) data, len);
}

void InstrClassPrint(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**) {
	ClassFile cf(data, len);

	ofstream os(outFileName(className, "disasm").c_str());
	os << cf;
}

void InstrClassIdentity(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlenp, u1** newdatap) {
	ClassFile cf(data, len);

	int newlen = cf.computeSize();

	ASSERT(len == newlen,
			"Expected class file len %d, actual was %d, on class %s", len,
			newlen, className);

	u1* newdata = Allocate(jvmti, newlen);

	cf.write(newdata, newlen);

	for (int i = 0; i < newlen; i++) {
		ASSERT(data[i] == newdata[i],
				"error at position %d in class %s: %d:%d != %d:%d", i,
				className, data[i], data[i + 1], newdata[i], newdata[i + 1]);
	}

	*newlenp = newlen;
	*newdatap = newdata;
}

void InstrClassObjectInit(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata) {

	if (string(className) != "java/lang/Object") {
		return;
	}

	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");

	u2 allocMethodRef = cf.addMethodRef(classIndex, "alloc",
			"(Ljava/lang/Object;)V");

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst* inst = new Inst();
		inst->kind = KIND_INVOKE;
		inst->opcode = opcode;
		inst->invoke.methodRefIndex = index;

		return inst;
	};

	for (Method& m : cf.methods) {

		string name = cf.getUtf8(m.nameIndex);

		if (m.hasCode() && name == "<init>") {
			InstList& instList = m.instList();

			instList.push_front(invoke(OPCODE_invokestatic, allocMethodRef));
			instList.push_front(new Inst(OPCODE_aload_0));
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassNewArray(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata) {
	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");

	u2 newArrayEventRef = cf.addMethodRef(classIndex, "newArrayEvent",
			"(ILjava/lang/Object;I)V");

	auto bipush = [&](u1 value) {
		Inst* inst = new Inst();
		inst->kind = KIND_BIPUSH;
		inst->opcode = OPCODE_bipush;
		inst->push.value = value;

		return inst;
	};

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst* inst = new Inst();
		inst->kind = KIND_INVOKE;
		inst->opcode = opcode;
		inst->invoke.methodRefIndex = index;

		return inst;
	};

	for (Method& m : cf.methods) {
		if (m.hasCode()) {
			InstList& instList = m.instList();

			InstList code;

			for (Inst* instp : instList) {
				Inst& inst = *instp;

				if (inst.opcode == OPCODE_newarray) {
					// FORMAT: newarray atype
					// OPERAND STACK: ... | count: int -> ... | arrayref

					// STACK: ... | count

					code.push_back(new Inst(OPCODE_dup));
					// STACK: ... | count | count

					code.push_back(&inst); // newarray
					// STACK: ... | count | arrayref

					code.push_back(new Inst(OPCODE_dup_x1));
					// STACK: ... | arrayref | count | arrayref

					code.push_back(bipush(inst.newarray.atype));
					//u2 typeindex = instr.cp->addInteger(atype);

					//bv.visitLdc(offset, OPCODE_ldc_w, typeindex);
					// STACK: ... | arrayref | count | arrayref | atype

					code.push_back(
							invoke(OPCODE_invokestatic, newArrayEventRef));
					// STACK: ... | arrayref

				} else {
					code.push_back(&inst);
				}
			}

			m.instList(code);
			m.codeAttr()->maxStack += 3;
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassANewArray(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata) {

//	if (string(className) != "frheapagent/HeapTest") {
//		return;
//	}

	ClassFile cf(data, len);
//	Version v = cf.getVersion();
	//fprintf(stderr, "%d.%d ", v.getMajor(), v.getMinor());

	ConstPool::Index classIndex = cf.addClass("frproxy/FrInstrProxy");
	ConstPool::Index aNewArrayEventRef = cf.addMethodRef(classIndex,
			"aNewArrayEvent", "(ILjava/lang/Object;Ljava/lang/String;)V");

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst* inst = new Inst();
		inst->kind = KIND_INVOKE;
		inst->opcode = opcode;
		inst->invoke.methodRefIndex = index;

		return inst;
	};

	auto ldc = [&] (Opcode opcode, u2 valueIndex) {
		Inst* inst = new Inst();
		inst->kind = KIND_LDC;
		inst->opcode = opcode;
		inst->ldc.valueIndex = valueIndex;

		return inst;
	};

	for (Method& m : cf.methods) {
		if (m.hasCode()) {
			InstList& instList = m.instList();

			InstList& code = instList;

//			for (Inst* instp : instList) {
			for (auto instp = instList.begin(); instp != instList.end();
					instp++) {
				Inst& inst = **instp;

				if (inst.opcode == OPCODE_anewarray) {
					// FORMAT: anewarray (indexbyte1 << 8) | indexbyte2
					// OPERAND STACK: ... | count: int -> ... | arrayref

					// STACK: ... | count

					code.insert(instp, new Inst(OPCODE_dup));
					// STACK: ... | count | count

					instp++;
					//code.push_back(&inst); // anewarray
					// STACK: ... | count | arrayref

					code.insert(instp, new Inst(OPCODE_dup_x1));
					// STACK: ... | arrayref | count | arrayref

					ConstPool::Index strIndex = cf.addStringFromClass(
							inst.type.classIndex);

					code.insert(instp, ldc(OPCODE_ldc_w, strIndex));
					// STACK: ... | arrayref | count | arrayref | classname

					instp = code.insert(instp,
							invoke(OPCODE_invokestatic, aNewArrayEventRef));
					// STACK: ... | arrayref

				} else {
					//code.push_back(&inst);
				}
			}

			m.codeAttr()->maxStack += 3;

			//	m.instList(code);
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassMain(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata) {

	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");
	u2 enterMainMethodRef = cf.addMethodRef(classIndex, "enterMainMethod",
			"()V");

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst* inst = new Inst();
		inst->kind = KIND_INVOKE;
		inst->opcode = opcode;
		inst->invoke.methodRefIndex = index;

		return inst;
	};

	for (Method& m : cf.methods) {

		string name = cf.getUtf8(m.nameIndex);
		string desc = cf.getUtf8(m.descIndex);

		if (m.hasCode() && name == "main" && (m.accessFlags & ACC_STATIC)
				&& (m.accessFlags & ACC_PUBLIC)
				&& desc == "([Ljava/lang/String;)V") {
			InstList& instList = m.instList();

			instList.push_front(
					invoke(OPCODE_invokestatic, enterMainMethodRef));

//			if ((opcode >= Opcodes.IRETURN && opcode <= Opcodes.RETURN) || opcode == Opcodes.ATHROW) {
//				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "exitMainMethod", "()V");
//			}

		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

}
