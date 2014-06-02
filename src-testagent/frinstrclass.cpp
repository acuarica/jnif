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

};

class ClassPath: public IClassPath {
public:

	ClassPath(JNIEnv* jni, jobject loader) :
			jni(jni), loader(loader) {
		if (proxyClass == nullptr) {
			proxyClass = jni->FindClass("frproxy/FrInstrProxy");
			ASSERT(proxyClass != NULL, "");

			getResourceId = jni->GetStaticMethodID(proxyClass, "getResource",
					"(Ljava/lang/String;Ljava/lang/ClassLoader;)[B");
			ASSERT(getResourceId != NULL, "");

			proxyClass = (jclass) jni->NewGlobalRef(proxyClass);
			ASSERT(proxyClass != NULL, "");
		}
	}

	string getCommonSuperClass(const string& className1,
			const string& className2) {
//		_TLOG("Common super class: left: %s, right: %s, loader: %s",
//				className1.c_str(), className2.c_str(),
//				(loader != NULL ? "object" : "(null)"));

		try {
			loadClassIfNotLoaded(className1);
			loadClassIfNotLoaded(className2);

			string sup = className1;
			const string& sub = className2;

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
		} catch (const ClassNotLoadedException& e) {
			string res = "java/lang/Object";
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

private:

	void loadClassIfNotLoaded(const string& className) {
		if (!classHierarchy.isDefined(className)) {
			loadClassAsResource(className);
		}
	}

	void loadClassAsResource(const string& className) {
//		_TLOG("loadClassAsResource: Trying to load class %s as a resource...",
//				className.c_str());

//		jclass proxyClass = jni->FindClass("frproxy/FrInstrProxy");
//		ASSERT(proxyClass != NULL, "");
//
//		jmethodID getResourceId = jni->GetStaticMethodID(proxyClass,
//				"getResource", "(Ljava/lang/String;Ljava/lang/ClassLoader;)[B");
//		ASSERT(getResourceId != NULL, "");

		jstring targetName = jni->NewStringUTF(className.c_str());
		ASSERT(targetName != NULL, "loadClassAsResource: ");

		jobject res = jni->CallStaticObjectMethod(proxyClass, getResourceId,
				targetName, loader);
		if (res != NULL) {
			ASSERT(res != NULL,
					"loadClassAsResource: getResource returned null");

			jsize len = jni->GetArrayLength((jarray) res);
			//	_TLOG("loadClassAsResource: resource len: %d", len);

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

	static jclass proxyClass;
	static jmethodID getResourceId;

};

jclass ClassPath::proxyClass = nullptr;
jmethodID ClassPath::getResourceId = nullptr;

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

extern int inLivePhase;
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
	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
}

bool isPrefix(const string& prefix, const string& text) {
	auto res = std::mismatch(prefix.begin(), prefix.end(), text.begin());
	return res.first == prefix.end();
}

bool skipCompute(const char* className) {
	if (!inLivePhase) {
		return true;
	}

	if (className != nullptr
			&& (isPrefix("java", className) || isPrefix("sun", className))) {
		return true;
	}

	return false;
}

void InstrClassCompute(jvmtiEnv* jvmti, u1* data, int len,
		const char* className, int* newlen, u1** newdata, JNIEnv* jni,
		InstrArgs* args) {
	LoadClassEvent m;

	ClassFile cf(data, len);
	classHierarchy.addClass(cf);

	if (skipCompute(className)) {
		return;
	}

	ClassPath cp(jni, args->loader);
	cf.computeFrames(&cp);

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
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
						instList.addSiPush(inst->indy()->callSite(), inst);
						instList.addInvoke(OPCODE_invokestatic, mid, inst);
					}
				}
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

	ConstIndex classIndex = cf.addClass("frproxy/FrInstrProxy");

	Instr::instrObjectInit(cf, classIndex);
	//Instr::instrNewArray(cf, classIndex);
	Instr::instrANewArray(cf, classIndex);
	Instr::instrMain(cf, classIndex);
	Instr::instrIndy(cf, classIndex);

	if (!skipCompute(className)) {
		ClassPath cp(jni, args->loader);
		cf.computeFrames(&cp);
	}

	cf.write(newdata, newlen, [&](u4 size) {return Allocate(jvmti, size);});
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
