/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <jvmti.h>

#include "frlog.h"
#include "frtlog.h"
#include "frexception.h"
#include "frinstr.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "jnif.hpp"

using namespace std;
using namespace jnif;

ClassHierarchy classHierarchy;

class ClassNotLoadedException {

};

class ClassPath: public IClassPath {
public:

	ClassPath(JNIEnv* jni, jobject loader) :
			jni(jni), loader(loader) {
	}

	string getCommonSuperClass(const string& className1,
			const string& className2) {
		_TLOG("Common super class: left: %s, right: %s, loader: %s",
				className1.c_str(), className2.c_str(),
				(loader != NULL ? "object" : "(null)"));

		try {
			loadClassIfNotLoaded(className1);
			loadClassIfNotLoaded(className2);

			string sup = className1;
			const string& sub = className2;

			while (!isAssignableFrom(sub, sup)) {
				loadClassIfNotLoaded(sup);
				sup = classHierarchy.getSuperClass(sup);
				if (sup == "0") {
					_TLOG("Common class is java/lang/Object!!!");
					return "java/lang/Object";
				}

				_TLOG("Intermediate super class is %s", sup.c_str());
			}

			_TLOG("Common super class found: %s", sup.c_str());

			return sup;
		} catch (const ClassNotLoadedException& e) {
			string res = "java/lang/Object";
			_TLOG(
					"Class not loaded while looking the common super class between %s and %s, returning %s",
					className1.c_str(), className2.c_str(), res.c_str());
			return res;
		}
	}

	bool isAssignableFrom(const string& sub, const string& sup) {
		string cls = sub;
		while (cls != "0") {
			if (cls == sup) {
				return true;
			}

			loadClassIfNotLoaded(cls);
			cls = classHierarchy.getSuperClass(cls);
		}

		return false;
	}

private:

	void loadClassIfNotLoaded(const string& className) {
		if (!classHierarchy.isDefined(className)) {
			loadClassAsResource(className);
		}
	}

	void loadClassAsResource(const string& className) {
		_TLOG("loadClassAsResource: Trying to load class %s as a resource...",
				className.c_str());

		jclass proxyClass = jni->FindClass("frproxy/FrInstrProxy");
		ASSERT(proxyClass != NULL, "");

		jmethodID getResourceId = jni->GetStaticMethodID(proxyClass,
				"getResource", "(Ljava/lang/String;Ljava/lang/ClassLoader;)[B");
		ASSERT(getResourceId != NULL, "");

		jstring targetName = jni->NewStringUTF(className.c_str());
		ASSERT(targetName != NULL, "loadClassAsResource: ");

		jobject res = jni->CallStaticObjectMethod(proxyClass, getResourceId,
				targetName, loader);
		if (res != NULL) {
			ASSERT(res != NULL,
					"loadClassAsResource: getResource returned null");

			jsize len = jni->GetArrayLength((jarray) res);
			_TLOG("loadClassAsResource: resource len: %d", len);

			u1* bytes = (u1*) jni->GetByteArrayElements((jbyteArray) res, NULL);
			ASSERT(bytes != NULL, "loadClassAsResource: ");

			ClassFile cf(bytes, len);
			classHierarchy.addClass(cf);
		} else {
			WARN("Class not found: %s", className.c_str());
			throw ClassNotLoadedException();
		}
	}

	JNIEnv* jni;
	jobject loader;
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
		const char* prefix = "./instr/") {
	string fileName = className;

	for (u4 i = 0; i < fileName.length(); i++) {
		fileName[i] = className[i] == '/' ? '.' : className[i];
	}

	stringstream path;
	path << prefix << fileName << "." << ext;

	return path.str();
}

typedef void (InstrFunc)(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args);

extern "C" {

void InvokeInstrFunc(InstrFunc* instrFunc, jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv* jni,
		InstrArgs* args) {
	try {
		(*instrFunc)(jvmti, data, len, className, newlen, newdata, jni, args);
	} catch (const JnifException& ex) {
		//cerr << "Error: JNIF Exception: " << ex.message << " @ " << endl;
		//cerr << "Error: exception on jnif: (stackTrace)" << endl;
		//cerr << ex.stackTrace << endl;
		cerr << ex << endl;
		throw ex;
	}
}

void InstrUnload() {
	//cerr << "Class Hierarchy: " << endl;
	//cerr << classHierarchy;
}

void InstrClassEmpty(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**, JNIEnv*, InstrArgs*) {
}

void InstrClassPrint(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**, JNIEnv*, InstrArgs* args) {
	ClassFile cf(data, len);

//	classHierarchy.addClass(cf);

	ofstream os(outFileName(className, "disasm").c_str());
	os << cf;

//	ofstream os(outFileName(className, "class").c_str(), ios::binary);
//	os.write((char*) data, len);
}

extern int inLivePhase;

void InstrClassIdentity(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv*,
		InstrArgs* args) {
	ClassFile cf(data, len);

//	ofstream os(outFileName(className, "disasm").c_str());
//	os << cf;

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassCompute(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv* jni,
		InstrArgs* args) {
//	ClassFile f("hola/Clase", "hola/UnaSuperClase");
	//f.getClassName(0);

	ClassFile cf(data, len);
	classHierarchy.addClass(cf);

	//if (args->loader == NULL)
	if (!inLivePhase) {
		return;
	}

	auto isPrefix = [&](const string& prefix, const string& text) {
		auto res = std::mismatch(prefix.begin(), prefix.end(), text.begin());
		return res.first == prefix.end();
	};

	if (isPrefix("java", className) || isPrefix("sun", className)) {
		_TLOG("Skipping compute on class: %s", className);
		return;
	}

	ClassPath cp(jni, args->loader);
	cf.computeFrames(&cp);

	ofstream os(outFileName(className, "disasm").c_str());
	os << cf;

//	ofstream dos(outFileName(className, "dot").c_str());
//	cf.dot(dos);

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassObjectInit(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args) {

	if (string(className) != "java/lang/Object") {
		return;
	}

	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");

	u2 allocMethodRef = cf.addMethodRef(classIndex, "alloc",
			"(Ljava/lang/Object;)V");

	for (Method* m : cf.methods) {
		String name = cf.getUtf8(m->nameIndex);

		if (m->hasCode() && name == "<init>") {
			InstList& instList = m->instList();

			Inst* first = *instList.begin();
			instList.addInvoke(OPCODE_invokestatic, allocMethodRef, first);
			instList.addZero(OPCODE_aload_0, first);
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassNewArray(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args) {
	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");

	u2 newArrayEventRef = cf.addMethodRef(classIndex, "newArrayEvent",
			"(ILjava/lang/Object;I)V");

	for (Method* m : cf.methods) {
		if (m->hasCode()) {
			//InstList& instList = m->instList();

//			InstList code;
//
//			for (Inst* instp : instList) {
//				Inst& inst = *instp;
//
//				if (inst.opcode == OPCODE_newarray) {
//					// FORMAT: newarray atype
//					// OPERAND STACK: ... | count: int -> ... | arrayref
//
//					// STACK: ... | count
//
//					code.push_back(new Inst(OPCODE_dup));
//					// STACK: ... | count | count
//
//					code.push_back(&inst); // newarray
//					// STACK: ... | count | arrayref
//
//					code.push_back(new Inst(OPCODE_dup_x1));
//					// STACK: ... | arrayref | count | arrayref
//
//					code.push_back(bipush(inst.newarray.atype));
//					//u2 typeindex = instr.cp->addInteger(atype);
//
//					//bv.visitLdc(offset, OPCODE_ldc_w, typeindex);
//					// STACK: ... | arrayref | count | arrayref | atype
//
//					code.push_back(
//							invoke(OPCODE_invokestatic, newArrayEventRef));
//					// STACK: ... | arrayref
//
//				} else {
//					code.push_back(&inst);
//				}
//			}
//
//			m->instList(code);
//			m->codeAttr()->maxStack += 3;
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassANewArray(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args) {

	ClassFile cf(data, len);

	ConstIndex classIndex = cf.addClass("frproxy/FrInstrProxy");
	ConstIndex aNewArrayEventRef = cf.addMethodRef(classIndex, "aNewArrayEvent",
			"(ILjava/lang/Object;Ljava/lang/String;)V");

	for (Method* m : cf.methods) {
		if (m->hasCode()) {
//			InstList& instList = m->instList();
//
//			InstList& code = instList;
//
////			for (Inst* instp : instList) {
//			for (auto instp = instList.begin(); instp != instList.end();
//					++instp) {
//				Inst& inst = **instp;
//
//				if (inst.opcode == OPCODE_anewarray) {
//					// FORMAT: anewarray (indexbyte1 << 8) | indexbyte2
//					// OPERAND STACK: ... | count: int -> ... | arrayref
//
//					// STACK: ... | count
//
//					code.insert(instp, new Inst(OPCODE_dup));
//					// STACK: ... | count | count
//
//					instp++;
//					//code.push_back(&inst); // anewarray
//					// STACK: ... | count | arrayref
//
//					code.insert(instp, new Inst(OPCODE_dup_x1));
//					// STACK: ... | arrayref | count | arrayref
//
//					ConstIndex strIndex = cf.addStringFromClass(
//							inst.type.classIndex);
//
//					code.insert(instp, ldc(OPCODE_ldc_w, strIndex));
//					// STACK: ... | arrayref | count | arrayref | classname
//
//					instp = code.insert(instp,
//							invoke(OPCODE_invokestatic, aNewArrayEventRef));
//					// STACK: ... | arrayref
//
//				} else {
//					//code.push_back(&inst);
//				}
//			}
//
//			m->codeAttr()->maxStack += 3;

			//	m.instList(code);
		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassMain(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args) {

	ClassFile cf(data, len);

	u2 classIndex = cf.addClass("frproxy/FrInstrProxy");
	u2 enterMainMethodRef = cf.addMethodRef(classIndex, "enterMainMethod",
			"()V");

	for (Method* m : cf.methods) {

		String name = cf.getUtf8(m->nameIndex);
		String desc = cf.getUtf8(m->descIndex);

		if (m->hasCode() && name == "main" && (m->accessFlags & METHOD_STATIC)
				&& (m->accessFlags & METHOD_PUBLIC)
				&& desc == "([Ljava/lang/String;)V") {
			InstList& instList = m->instList();

			instList.addInvoke(OPCODE_invokestatic, enterMainMethodRef,
					*instList.begin());

//			if ((opcode >= Opcodes.IRETURN && opcode <= Opcodes.RETURN) || opcode == Opcodes.ATHROW) {
//				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "exitMainMethod", "()V");
//			}

		}
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

void InstrClassHeap(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args) {

	ClassFile cf(data, len);

	ConstIndex classIndex = cf.addClass("frproxy/FrInstrProxy");
	ConstIndex aNewArrayEventRef = cf.addMethodRef(classIndex, "aNewArrayEvent",
			"(ILjava/lang/Object;Ljava/lang/String;)V");
	ConstIndex enterMainMethodRef = cf.addMethodRef(classIndex,
			"enterMainMethod", "()V");
	ConstIndex exitMainMethodRef = cf.addMethodRef(classIndex, "exitMainMethod",
			"()V");

	if (string(className) == "java/lang/Object") {
		u2 allocMethodRef = cf.addMethodRef(classIndex, "alloc",
				"(Ljava/lang/Object;)V");

		for (Method* m : cf.methods) {
			const string& name = cf.getUtf8(m->nameIndex);

			if (m->hasCode() && name == "<init>") {
				InstList& instList = m->instList();

//				instList.push_front(
//						invoke(OPCODE_invokestatic, allocMethodRef));
//				instList.push_front(new Inst(OPCODE_aload_0));
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
					++instp) {
				Inst& inst = **instp;

				if (inst.opcode == OPCODE_anewarray) {
					// FORMAT: anewarray (indexbyte1 << 8) | indexbyte2
					// OPERAND STACK: ... | count: int -> ... | arrayref

					// STACK: ... | count

//					code.insert(instp, new Inst(OPCODE_dup));
//					// STACK: ... | count | count
//
//					instp++;
//					//code.push_back(&inst); // anewarray
//					// STACK: ... | count | arrayref
//
//					code.insert(instp, new Inst(OPCODE_dup_x1));
//					// STACK: ... | arrayref | count | arrayref
//
//					ConstIndex strIndex = cf.addStringFromClass(
//							inst.type.classIndex);
//
//					code.insert(instp, ldc(OPCODE_ldc_w, strIndex));
//					// STACK: ... | arrayref | count | arrayref | classname
//
//					instp = code.insert(instp,
//							invoke(OPCODE_invokestatic, aNewArrayEventRef));
					// STACK: ... | arrayref

				} else {
					//code.push_back(&inst);
				}
			}

			m->codeAttr()->maxStack += 3;

			if (methodName == "main" && (m->accessFlags & METHOD_STATIC)
					&& (m->accessFlags & METHOD_PUBLIC)
					&& methodDesc == "([Ljava/lang/String;)V") {
//				instList.push_front(
//						invoke(OPCODE_invokestatic, enterMainMethodRef));

				for (auto instp = instList.begin(); instp != instList.end();
						++instp) {
					Inst& inst = **instp;

					if ((inst.opcode >= OPCODE_ireturn
							&& inst.opcode <= OPCODE_return)
							|| inst.opcode == OPCODE_athrow) {
//						instList.insert(instp,
//								invoke(OPCODE_invokestatic, exitMainMethodRef));
					}
				}
			}
		}

		cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
	}
}

}
