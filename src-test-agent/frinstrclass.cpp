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

class ClassPath: public IClassPath {
public:

	ClassPath(JNIEnv* jni) :
			jni(jni) {
		javaLangClass = jni->FindClass("java/lang/Class");
		ASSERT(javaLangClass != NULL, "javaLangClass is null!");

		getNameMethodId = jni->GetMethodID(javaLangClass, "getName",
				"()Ljava/lang/String;");
		ASSERT(getNameMethodId != NULL, "getNameMethodId is null!");
	}

	const std::string getCommonSuperClass(const std::string& className1,
			const std::string& className2) {
		cerr << "arg clazz1: " << className1 << ", arg clazz2: " << className2
				<< endl;

		jclass clazz1 = jni->FindClass(className1.c_str());

		if (clazz1 == NULL) {
//			jni->ExceptionDescribe();
//
			jthrowable ex = jni->ExceptionOccurred();
			ASSERT(ex != NULL, "invalid ex");
//
//			jni->ExceptionClear();
//
//			jclass clazzT = jni->FindClass("java/lang/Throwable");
//			jmethodID pstid = jni->GetMethodID(clazzT, "printStackTrace",
//					"()V");
//			jni->CallObjectMethod(ex, pstid);

			jni->ExceptionDescribe();
			jclass exClass = jni->GetObjectClass(ex);
			jmethodID mid = jni->GetMethodID(exClass, "toString",
					"()Ljava/lang/String;");
			jni->ExceptionClear();
			jstring errMsg = (jstring) jni->CallObjectMethod(ex, mid);
			jboolean isCopy = JNI_FALSE;
			const char* msg = jni->GetStringUTFChars(errMsg, &isCopy);

			INFO("Exception message: %s", msg);
			jni->ReleaseStringUTFChars(errMsg, msg);

		}

		ASSERT(clazz1 != NULL, "clazz1 is null for class: %s",
				className1.c_str());

		jclass clazz2 = jni->FindClass(className2.c_str());
		ASSERT(clazz2 != NULL, "clazz2 is null!");

		cerr << "clazz1: " << getClassName(clazz1) << ", clazz2: "
				<< getClassName(clazz2) << endl;

		while (!jni->IsAssignableFrom(clazz2, clazz1)) {
			clazz1 = jni->GetSuperclass(clazz1);
			ASSERT(clazz1 != NULL, "superclass is null!");

			cerr << "super clazz: " << getClassName(clazz1) << endl;
		}

		string common = fromBinaryName(getClassName(clazz1));
		cerr << common << endl;

		return common;
	}

private:

	string fromBinaryName(const string& className) {
		string newName = className;

		for (u4 i = 0; i < newName.length(); i++) {
			newName[i] = className[i] == '.' ? '/' : className[i];
		}

		return newName;
	}

	string getString(jstring str) {
		jsize len = jni->GetStringUTFLength(str);
		ASSERT(len > 0, "len is zero!");

		const char* strUtf8 = jni->GetStringUTFChars(str, NULL);
		ASSERT(strUtf8 != NULL, "strUtf8 is zero!");

		string res(strUtf8, len);
		jni->ReleaseStringUTFChars(str, strUtf8);

		return res;
	}

	string getClassName(jclass clazz) {
		jstring name = (jstring) jni->CallObjectMethod(clazz, getNameMethodId);
		ASSERT(name != NULL, "name is null!");

		return getString(name);
	}

	JNIEnv* jni;
	jclass javaLangClass;
	jmethodID getNameMethodId;
};

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

void InstrClassEmpty(jvmtiEnv*, u1*, int, const char*, int*, u1**, JNIEnv*) {

}

void InstrClassDump(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**, JNIEnv*) {

	ofstream os(outFileName(className, "class").c_str(), ios::binary);
	os.write((char*) data, len);
}

void InstrClassPrint(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**, JNIEnv*) {
	ClassFile cf(data, len);

	ofstream os(outFileName(className, "disasm").c_str());
	os << cf;
}

void InstrClassIdentity(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv*) {
	ClassFile cf(data, len);
	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

extern int isMainLoaded;

extern int inStartPhase;
extern int inLivePhase;

void InstrClassCompute(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv* jni) {
	ClassFile cf(data, len);

	//if (inLivePhase) {
	if (isMainLoaded) {
		ClassPath cp(jni);
		cf.computeFrames(&cp);
	}

	ofstream os(outFileName(className, "disasm").c_str());
	os << cf;

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassComputeApp(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv* jni) {

	if (isMainLoaded == 0) {
		return;
	}

//	if (string(className) != "frheapagent/HeapTest") {
//		return;
//	}

	ClassFile cf(data, len);
	ClassPath cp(jni);
	cf.computeFrames(&cp);

	ofstream os(outFileName(className, "disasm").c_str());
	os << cf;

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassObjectInit(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni) {

	if (string(className) != "java/lang/Object") {
		return;
	}

	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");

	u2 allocMethodRef = cf.addMethodRef(classIndex, "alloc",
			"(Ljava/lang/Object;)V");

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst* inst = new Inst(opcode, KIND_INVOKE);
		//inst->kind = KIND_INVOKE;
		//inst->opcode = opcode;
			inst->invoke.methodRefIndex = index;

			return inst;
		};

	for (Method* m : cf.methods) {

		string name = cf.getUtf8(m->nameIndex);

		if (m->hasCode() && name == "<init>") {
			InstList& instList = m->instList();

			instList.push_front(invoke(OPCODE_invokestatic, allocMethodRef));
			instList.push_front(new Inst(OPCODE_aload_0));
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassNewArray(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni) {
	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");

	u2 newArrayEventRef = cf.addMethodRef(classIndex, "newArrayEvent",
			"(ILjava/lang/Object;I)V");

	auto bipush = [&](u1 value) {
		Inst* inst = new Inst(OPCODE_bipush, KIND_BIPUSH);

		//inst->kind = KIND_BIPUSH;
		//inst->opcode = OPCODE_bipush;
			inst->push.value = value;

			return inst;
		};

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst* inst = new Inst(opcode, KIND_INVOKE);
		//inst->kind = KIND_INVOKE;
		//inst->opcode = opcode;
			inst->invoke.methodRefIndex = index;

			return inst;
		};

	for (Method* m : cf.methods) {
		if (m->hasCode()) {
			InstList& instList = m->instList();

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

			m->instList(code);
			m->codeAttr()->maxStack += 3;
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassANewArray(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni) {

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
		Inst* inst = new Inst(opcode, KIND_INVOKE);
		//inst->kind = KIND_INVOKE;
		//inst->opcode = opcode;
			inst->invoke.methodRefIndex = index;

			return inst;
		};

	auto ldc = [&] (Opcode opcode, u2 valueIndex) {
		Inst* inst = new Inst(opcode, KIND_LDC);
		//inst->kind = KIND_LDC;
		//inst->opcode = opcode;
			inst->ldc.valueIndex = valueIndex;

			return inst;
		};

	for (Method* m : cf.methods) {
		if (m->hasCode()) {
			InstList& instList = m->instList();

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

			m->codeAttr()->maxStack += 3;

			//	m.instList(code);
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassMain(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni) {

	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");
	u2 enterMainMethodRef = cf.addMethodRef(classIndex, "enterMainMethod",
			"()V");

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst* inst = new Inst(opcode, KIND_INVOKE);
		//inst->kind = KIND_INVOKE;
		//inst->opcode = opcode;
			inst->invoke.methodRefIndex = index;

			return inst;
		};

	for (Method* m : cf.methods) {

		string name = cf.getUtf8(m->nameIndex);
		string desc = cf.getUtf8(m->descIndex);

		if (m->hasCode() && name == "main" && (m->accessFlags & METHOD_STATIC)
				&& (m->accessFlags & METHOD_PUBLIC)
				&& desc == "([Ljava/lang/String;)V") {
			InstList& instList = m->instList();

			instList.push_front(
					invoke(OPCODE_invokestatic, enterMainMethodRef));

//			if ((opcode >= Opcodes.IRETURN && opcode <= Opcodes.RETURN) || opcode == Opcodes.ATHROW) {
//				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "exitMainMethod", "()V");
//			}

		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassHeap(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni) {

	ClassFile cf(data, len);

	ConstPool::Index classIndex = cf.addClass("frproxy/FrInstrProxy");
	ConstPool::Index aNewArrayEventRef = cf.addMethodRef(classIndex,
			"aNewArrayEvent", "(ILjava/lang/Object;Ljava/lang/String;)V");
	ConstPool::Index enterMainMethodRef = cf.addMethodRef(classIndex,
			"enterMainMethod", "()V");
	ConstPool::Index exitMainMethodRef = cf.addMethodRef(classIndex,
			"exitMainMethod", "()V");

	auto invoke = [&] (Opcode opcode, u2 index) {
		Inst* inst = new Inst(opcode, KIND_INVOKE);
		inst->invoke.methodRefIndex = index;

		return inst;
	};

	auto ldc = [&] (Opcode opcode, u2 valueIndex) {
		Inst* inst = new Inst(opcode, KIND_LDC);
		inst->ldc.valueIndex = valueIndex;

		return inst;
	};

	if (string(className) == "java/lang/Object") {
		u2 allocMethodRef = cf.addMethodRef(classIndex, "alloc",
				"(Ljava/lang/Object;)V");

		for (Method* m : cf.methods) {
			const string& name = cf.getUtf8(m->nameIndex);

			if (m->hasCode() && name == "<init>") {
				InstList& instList = m->instList();

				instList.push_front(
						invoke(OPCODE_invokestatic, allocMethodRef));
				instList.push_front(new Inst(OPCODE_aload_0));
			}
		}
	}

	for (Method* m : cf.methods) {
		if (m->hasCode()) {
			const string& methodName = cf.getUtf8(m->nameIndex);
			const string& methodDesc = cf.getUtf8(m->descIndex);

			InstList& instList = m->instList();
			InstList& code = instList;

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

			m->codeAttr()->maxStack += 3;

			if (methodName == "main" && (m->accessFlags & METHOD_STATIC)
					&& (m->accessFlags & METHOD_PUBLIC)
					&& methodDesc == "([Ljava/lang/String;)V") {
				instList.push_front(
						invoke(OPCODE_invokestatic, enterMainMethodRef));

				for (auto instp = instList.begin(); instp != instList.end();
						instp++) {
					Inst& inst = **instp;

					if ((inst.opcode >= OPCODE_ireturn
							&& inst.opcode <= OPCODE_return)
							|| inst.opcode == OPCODE_athrow) {
						instList.insert(instp,
								invoke(OPCODE_invokestatic, exitMainMethodRef));
					}
				}
			}
		}

		cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
	}
}

}
