/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <jvmti.h>

#include "frlog.hpp"
#include "frtlog.hpp"
#include "frexception.hpp"
#include "frinstr.hpp"
#include "testagent.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <mutex>

#include "jnif.hpp"

using namespace std;
using namespace jnif;

ClassHierarchy classHierarchy;

class ClassNotLoadedException {
public:

	ClassNotLoadedException(const String& className) :
			className(className) {
	}

	String className;

};

class TooEarlyException {
public:

	TooEarlyException(const String& className) :
			className(className) {
	}

	const String className;

};

bool isPrefix(const String& prefix, const String& text) {
	auto res = std::mismatch(prefix.begin(), prefix.end(), text.begin());
	return res.first == prefix.end();
}

bool inLivePhase = false;

bool skipCompute(const char* className) {
//	if (!init) {
	//	return true;
	//}

	if (className != NULL
			&& (isPrefix("java/", className) || isPrefix("suqn", className))) {
		return true;
	}

	return false;
}

class ClassPath: public IClassPath {
public:

	ClassPath(const char*, JNIEnv* jni, jobject loader) :
			jni(jni), loader(loader) {

		if (loader != NULL) {
			inLivePhase = true;
		}

//		if (proxyClass == NULL) {
//			proxyClass = jni->FindClass("frproxy/FrInstrProxy");
//			ASSERT(proxyClass != NULL, "");
//
//			getResourceId = jni->GetStaticMethodID(proxyClass, "getResource",
//					"(Ljava/lang/String;Ljava/lang/ClassLoader;)[B");
//			ASSERT(getResourceId != NULL, "");
//
//			proxyClass = (jclass) jni->NewGlobalRef(proxyClass);
//			ASSERT(proxyClass != NULL, "");
//		}
	}

	String getCommonSuperClass(const String& className1,
			const String& className2) {
		_TLOG("Common super class: left: %s, right: %s, loader: %s",
				className1.c_str(), className2.c_str(),
				(loader != NULL ? "object" : "(null)"));

		if (!inLivePhase) {
			///String res = "java/lang/Object";
			//WARN("Too early for Class : %s", className);
			return "java/lang/Object";
		}

		//if (isPrefix("java/", className) || isPrefix("suqn", className)) {
		//WARN("class in java lang: %s", className);
		//return "java/lang/Object";
		//throw TooEarlyException(className);
		//}

		try {
			loadClassIfNotLoaded(className1);
			loadClassIfNotLoaded(className2);

			String sup = className1;
			const String& sub = className2;

			while (!isAssignableFrom(sub, sup)) {
				loadClassIfNotLoaded(sup);
				sup = classHierarchy.getSuperClass(sup);
				if (sup == "0") {
					//_TLOG("Common class is java/lang/Object!!!");
					return "java/lang/Object";
				}

				//_TLOG("Intermediate super class is %s", sup.c_str());
			}

			//_TLOG("Common super class found: %s", sup.c_str());

			return sup;
//		} catch (const TooEarlyException& e) {
			//		String res = "java/lang/Object";
			//	WARN("Too early for Class : %s", e.className.c_str());
			//	return res;
		} catch (const ClassNotLoadedException& e) {
			String res = "java/lang/Object";
			WARN("Class not found: %s", e.className.c_str());
//			_TLOG(
//					"Class not loaded while looking the common super class between %s and %s, returning %s",
//					className1.c_str(), className2.c_str(), res.c_str());
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

	static void initProxyClass(JNIEnv* jni) {
		if (proxyClass == NULL) {
			proxyClass = jni->FindClass("frproxy/FrInstrProxy");
			ASSERT(proxyClass != NULL, "");

			getResourceId = jni->GetStaticMethodID(proxyClass, "getResource",
					"(Ljava/lang/String;Ljava/lang/ClassLoader;)[B");
			ASSERT(getResourceId != NULL, "");

			proxyClass = (jclass) jni->NewGlobalRef(proxyClass);
			ASSERT(proxyClass != NULL, "");
		}
	}

private:

	void loadClassIfNotLoaded(const String& className) {
		if (!classHierarchy.isDefined(className)) {
			loadClassAsResource(className);
		}
	}

	void loadClassAsResource(const String& className) {
//		_TLOG("loadClassAsResource: Trying to load class %s as a resource...",
//				className.c_str());

//		jclass proxyClass = jni->FindClass("frproxy/FrInstrProxy");
//		ASSERT(proxyClass != NULL, "");
//
//		jmethodID getResourceId = jni->GetStaticMethodID(proxyClass,
//				"getResource", "(Ljava/lang/String;Ljava/lang/ClassLoader;)[B");
//		ASSERT(getResourceId != NULL, "");

//if (skipCompute(className.c_str()))

		ClassPath::initProxyClass(jni);

		jstring targetName = jni->NewStringUTF(className.c_str());
		ASSERT(targetName != NULL, "loadClassAsResource: ");

		jobject res = jni->CallStaticObjectMethod(proxyClass, getResourceId,
				targetName, loader);

		if (res == NULL) {
			throw ClassNotLoadedException(className);
		}

		jsize len = jni->GetArrayLength((jarray) res);

		u1* bytes = (u1*) jni->GetByteArrayElements((jbyteArray) res, NULL);
		ASSERT(bytes != NULL, "loadClassAsResource: ");

		ClassFile cf(bytes, len);

		jni->ReleaseByteArrayElements((jbyteArray) res, (jbyte*) bytes,
		JNI_ABORT);
		jni->DeleteLocalRef(res);
		jni->DeleteLocalRef(targetName);

		classHierarchy.addClass(cf);
	}

	//const char* className;
	JNIEnv* jni;
	jobject loader;

	static jclass proxyClass;
	static jmethodID getResourceId;
};

jclass ClassPath::proxyClass = NULL;
jmethodID ClassPath::getResourceId = NULL;

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
		const char* prefix = "./build/instr/") {
	String fileName = className == NULL ? "null" : className;

	for (u4 i = 0; i < fileName.length(); i++) {
		fileName[i] = fileName[i] == '/' ? '.' : fileName[i];
	}

	stringstream path;
	path << prefix << fileName << "." << ext;

	return path.str();
}

std::mutex _mutex;

class LoadClassEvent {
public:

	LoadClassEvent() {
		if (tldget()->classLoadedStack == 0) {

			_mutex.lock();

			//cerr << "+";
		}

		tldget()->classLoadedStack++;
	}

	~LoadClassEvent() {
		tldget()->classLoadedStack--;

		if (tldget()->classLoadedStack == 0) {
			//cerr << "-";

			_mutex.unlock();
		}
	}

private:
};

void InstrClassEmpty(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**, JNIEnv*, InstrArgs*) {
}

void InstrClassIdentity(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv*,
		InstrArgs* args) {
	ClassFile cf(data, len);
	*newlen = cf.computeSize();
	*newdata = Allocate(jvmti, *newlen);
	cf.write(*newdata, *newlen);
}

void InstrClassCompute(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv* jni,
		InstrArgs* args) {
	LoadClassEvent m;

	ClassFile cf(data, len);
	classHierarchy.addClass(cf);

	ClassPath cp(cf.getThisClassName(), jni, args->loader);
	cf.computeFrames(&cp);

	*newlen = cf.computeSize();
	*newdata = Allocate(jvmti, *newlen);
	cf.write(*newdata, *newlen);
}

class Instr {
public:

	static void instrObjectInit(ClassFile& cf, ConstIndex classIndex) {
		if (cf.getThisClassName() != String("java/lang/Object")) {
			return;
		}

		ConstIndex mid = cf.addMethodRef(classIndex, "alloc",
				"(Ljava/lang/Object;)V");

		for (Method* m : cf.methods) {
			if (m->isInit()) {
				InstList& instList = m->instList();

				Inst* p = *instList.begin();
				instList.addZero(OPCODE_aload_0, p);
				instList.addInvoke(OPCODE_invokestatic, mid, p);
			}
		}
	}

	static void instrNewArray(ClassFile& cf, ConstIndex classIndex) {
		const char* desc = "(ILjava/lang/Object;I)V";
		ConstIndex mid = cf.addMethodRef(classIndex, "newArrayEvent", desc);

		for (Method* m : cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				for (Inst* inst : instList) {
					if (inst->opcode == OPCODE_newarray) {
						// FORMAT: newarray atype
						// OPERAND STACK: ... | count: int -> ... | arrayref

						// STACK: ... | count
						instList.addZero(OPCODE_dup, inst);

						// STACK: ... | count | count

						Inst* p = inst->next; // newarray
						// STACK: ... | count | arrayref

						instList.addZero(OPCODE_dup_x1, p);
						// STACK: ... | arrayref | count | arrayref

						instList.addBiPush(inst->newarray()->atype, p);

						// STACK: ... | arrayref | count | arrayref | atype

						instList.addInvoke(OPCODE_invokestatic, mid, p);
						// STACK: ... | arrayref
					}
				}

				m->codeAttr()->maxStack += 3;
			}
		}
	}

	static void instrANewArray(ClassFile& cf, ConstIndex classIndex) {
		const char* desc = "(ILjava/lang/Object;Ljava/lang/String;)V";
		ConstIndex mid = cf.addMethodRef(classIndex, "aNewArrayEvent", desc);

		for (Method* m : cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				for (Inst* inst : instList) {
					if (inst->opcode == OPCODE_anewarray) {
						// FORMAT: anewarray (indexbyte1 << 8) | indexbyte2
						// OPERAND STACK: ... | count: int -> ... | arrayref

						// STACK: ... | count

						instList.addZero(OPCODE_dup, inst);
						// STACK: ... | count | count

						Inst* p = inst->next; // anewarray
						// STACK: ... | count | arrayref

						instList.addZero(OPCODE_dup_x1, p);
						// STACK: ... | arrayref | count | arrayref

						auto ci = inst->type()->classIndex;
						auto strIndex = cf.addStringFromClass(ci);

						instList.addLdc(OPCODE_ldc_w, strIndex, p);
						// STACK: ... | arrayref | count | arrayref | classname

						instList.addInvoke(OPCODE_invokestatic, mid, p);
						// STACK: ... | arrayref
					}
				}

				m->codeAttr()->maxStack += 3;
			}
		}
	}

	static void instrMethodEntryExit(ClassFile& cf, ConstIndex proxyClass) {
		//if  ( cf.getThisClassName())
		ConstIndex sid = cf.addMethodRef(proxyClass, "enterMethod",
				"(Ljava/lang/String;Ljava/lang/String;)V");

		ConstIndex eid = cf.addMethodRef(proxyClass, "exitMethod",
				"(Ljava/lang/String;Ljava/lang/String;)V");

		ConstIndex classNameIdx = cf.addStringFromClass(cf.thisClassIndex);

		for (Method* m : cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				ConstIndex methodIndex = cf.addString(m->nameIndex);

				Inst* p = *instList.begin();

				instList.addLdc(OPCODE_ldc_w, classNameIdx, p);
				instList.addLdc(OPCODE_ldc_w, methodIndex, p);
				instList.addInvoke(OPCODE_invokestatic, sid, p);

				for (Inst* inst : instList) {
					if (inst->isExit()) {
						instList.addLdc(OPCODE_ldc_w, classNameIdx, inst);
						instList.addLdc(OPCODE_ldc_w, methodIndex, inst);
						instList.addInvoke(OPCODE_invokestatic, eid, inst);
					}
				}
			}
		}
	}

	static void instrMain(ClassFile& cf, ConstIndex classIndex) {
		ConstIndex sid = cf.addMethodRef(classIndex, "enterMainMethod", "()V");
		ConstIndex eid = cf.addMethodRef(classIndex, "exitMainMethod", "()V");

		for (Method* m : cf.methods) {
			if (m->isMain()) {
				InstList& instList = m->instList();

				Inst* p = *instList.begin();
				instList.addInvoke(OPCODE_invokestatic, sid, p);

				for (Inst* inst : instList) {
					if (inst->isExit()) {
						instList.addInvoke(OPCODE_invokestatic, eid, inst);
					}
				}
			}
		}
	}

	static void instrIndy(ClassFile& cf, ConstIndex classIndex) {
		ConstIndex mid = cf.addMethodRef(classIndex, "indy", "(I)V");

		for (Method* m : cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				for (Inst* inst : instList) {
					if (inst->isInvokeDynamic()) {
						//cerr << "asdafasdf" << endl;
						//throw JnifException("Indy found");
						instList.addSiPush(inst->indy()->callSite(), inst);
						instList.addInvoke(OPCODE_invokestatic, mid, inst);
					}
				}
			}
		}
	}

	static void instrAllOpcodes(ClassFile& cf, ConstIndex proxyClass) {
//		ConstIndex mid = cf.addMethodRef(proxyClass, "opcode", "(I)V");

		for (Method* m : cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				for (Inst* inst : instList) {
//					if (inst->opcode == OPCODE_newarray
//							|| inst->opcode == OPCODE_anewarray
//							|| inst->opcode == OPCODE_invokestatic)
					if (inst->kind != KIND_LABEL) {
						instList.addZero(OPCODE_nop, inst);
						//instList.addBiPush(inst->opcode, inst);
						//instList.addInvoke(OPCODE_invokestatic, mid, inst);
					}
				}

				m->codeAttr()->maxStack += 1;
			}
		}
	}

};

void InstrClassStats(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args) {
	LoadClassEvent m;

	ClassFile cf(data, len);
	classHierarchy.addClass(cf);

	ConstIndex proxyClass = cf.addClass("frproxy/FrInstrProxy");

	Instr::instrObjectInit(cf, proxyClass);
	//Instr::instrNewArray(cf, classIndex);
	Instr::instrANewArray(cf, proxyClass);
	Instr::instrMain(cf, proxyClass);
	//Instr::instrIndy(cf, proxyClass);

	//Instr::instrMethodEntryExit(cf, proxyClass);
	//Instr::instrAllOpcodes(cf, proxyClass);

	try {
		ClassPath cp(cf.getThisClassName(), jni, args->loader);
		cf.computeFrames(&cp);

		*newlen = cf.computeSize();
		*newdata = Allocate(jvmti, *newlen);
		cf.write(*newdata, *newlen);
	} catch (const InvalidMethodLengthException& ex) {
		cerr << "Class not instrumented: " << ex.message() << endl;
	}
}

void InstrClassAll(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args) {
	LoadClassEvent m;

	ClassFile cf(data, len);
	classHierarchy.addClass(cf);

	ConstIndex proxyClass = cf.addClass("frproxy/FrInstrProxy");

	if (!isPrefix("java/lang/", cf.getThisClassName())) {
		Instr::instrAllOpcodes(cf, proxyClass);
	}

	try {
		ClassPath cp(cf.getThisClassName(), jni, args->loader);
		cf.computeFrames(&cp);

		*newlen = cf.computeSize();
		*newdata = Allocate(jvmti, *newlen);
		cf.write(*newdata, *newlen);
	} catch (const JnifException& ex) {
		//cerr << ex;
	}
}

void InstrClassPrint(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**, JNIEnv*, InstrArgs* args) {
	ClassFile cf(data, len);
	ofstream os(outFileName(className, "disasm").c_str());
	os << cf;
}

void InstrClassDot(jvmtiEnv*, u1* data, int len, const char* className, int*,
		u1**, JNIEnv*, InstrArgs* args) {
	ClassFile cf(data, len);
	ofstream os(outFileName(className, "dot").c_str());
	cf.dot(os);
}
