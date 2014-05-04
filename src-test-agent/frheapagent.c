/**
 *
 */
#include <stdbool.h>

#include <jvmti.h>
#include <jni.h>

#include "frthread.h"
#include "frlog.h"
#include "frjvmti.h"
#include "frtlog.h"
#include "frstamp.h"
#include "frinstr.h"

typedef void (InstrFunc)(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni);

InstrFunc* instrFunc;

char* javaCommand;

int isMainLoaded = 0;

static void JNICALL ClassFileLoadEvent(jvmtiEnv* jvmti, JNIEnv* jni,
		jclass class_being_redefined, jobject loader, const char* name,
		jobject protection_domain, jint class_data_len,
		const unsigned char* class_data, jint* new_class_data_len,
		unsigned char** new_class_data) {
	_TLOG("CLASSFILELOAD:%s", name);

	if (strcmp(name, javaCommand) == 0) {
		isMainLoaded = 1;
	}

	if (!FrIsProxyClassName(name)) {
		(*instrFunc)(jvmti, (unsigned char*) class_data, class_data_len, name,
				new_class_data_len, new_class_data, jni);
	}
}

static void JNICALL ClassLoadEvent(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread,
		jclass klass) {
	char* classsig;
	FrGetClassSignature(jvmti, klass, &classsig, NULL);

	_TLOG("CLASSLOAD:%s", classsig);

	FrDeallocate(jvmti, classsig);

	StampClass(jvmti, jni, klass);
}

/**
 * Event sent during JVMTI_PHASE_START or JVMTI_PHASE_LIVE phase.
 */
static void JNICALL ClassPrepareEvent(jvmtiEnv* jvmti, JNIEnv* jni,
		jthread thread, jclass klass) {
	char* classsig;

	FrGetClassSignature(jvmti, klass, &classsig, NULL);

	_TLOG("CLASSPREPARE:%s", classsig);

	if (FrIsProxyClassSignature(classsig)) {
		FrSetInstrHandlerNatives(jvmti, jni, klass);
	}

	FrDeallocate(jvmti, classsig);

	StampClass(jvmti, jni, klass);
}

extern signed char frproxy_FrInstrProxy_class[];
extern int frproxy_FrInstrProxy_class_len;

int inStartPhase = 0;
int inLivePhase = 0;

/**
 * Changes from JVMTI_PHASE_PRIMORDIAL to JVMTI_PHASE_START phase.
 * This phase is when the JVM is starting to execute java bytecode.
 * Therefore since we want to catch all the event in this bootstrap phase,
 * we need to define the proxy class as earlier as possibly.
 *
 * Also it is needed to stamp classes that won't be prepared ever,
 * like primitive classes and Finalizer (why?)
 */
static void JNICALL VMStartEvent(jvmtiEnv* jvmti, JNIEnv* jni) {
	inStartPhase = 1;

	NOTICE("VM started");

	_TLOG("VMSTART");

	jclass proxyClass = (*jni)->DefineClass(jni, FR_PROXY_CLASS, NULL,
			frproxy_FrInstrProxy_class, frproxy_FrInstrProxy_class_len);

	if (proxyClass == NULL) {
		ERROR("Error on define class");
	}
}

static void JNICALL VMInitEvent(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread) {
	inLivePhase = 1;

	NOTICE("VM init");

	_TLOG("VMINIT");

	StampThread(jvmti, thread);
}

static void JNICALL ExceptionEvent(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread,
		jmethodID method, jlocation location, jobject ex,
		jmethodID catch_method, jlocation catch_location) {
//	ERROR("Exception in bytecode");

//	(*jni)->ExceptionClear(jni);

//	jclass cls = (*jni)->GetObjectClass(jni, ex);
//
//	jclass javaLangClass = (*jni)->FindClass(jni, "java/lang/Class");
//	jmethodID getNameMethod = (*jni)->GetMethodID(jni, javaLangClass, "getName",
//			"()Ljava/lang/String;");
//	jstring name = (*jni)->CallObjectMethod(jni, cls, getNameMethod);
//	jsize namelen = (*jni)->GetStringUTFLength(jni, name);
//	const char* nameutf8 = (*jni)->GetStringUTFChars(jni, name, NULL);
//	INFO("nameutf8:%.*s", namelen, nameutf8);
//	(*jni)->ReleaseStringUTFChars(jni, name, nameutf8);

	if (inLivePhase) {
//		jclass clazz = (*jni)->FindClass(jni, "java/lang/Throwable");
//		jmethodID pstid = (*jni)->GetMethodID(jni, clazz, "printStackTrace",
//				"()V");
//		(*jni)->CallObjectMethod(jni, ex, pstid);
	}

	_TLOG("EXCEPTION");
}

static void JNICALL ObjectFreeEvent(jvmtiEnv *jvmti, jlong tag) {
	_TLOG("FREE:%ld", tag);
}

static void JNICALL GarbageCollectionStartEvent(jvmtiEnv *jvmti) {
	NOTICE("Garbage Collection started");

	_TLOG("GARBAGECOLLECTIONSTART");
}

static void JNICALL GarbageCollectionFinishEvent(jvmtiEnv *jvmti) {
	NOTICE("Garbage Collection finished");

	_TLOG("GARBAGECOLLECTIONFINISH");
}

static void JNICALL ThreadStartEvent(jvmtiEnv* jvmti, JNIEnv* jni,
		jthread thread) {
	StampThread(jvmti, thread);

	NOTICE("Thread start: Thread id: %d, tag: %ld", tldget()->threadId,
			tldget()->threadTag);
}

static void JNICALL ThreadEndEvent(jvmtiEnv* jvmti, JNIEnv* jni, jthread thread) {
	NOTICE("Thread end: Thread id: %d, tag: %ld", tldget()->threadId,
			tldget()->threadTag);
}

static void JNICALL VMDeathEvent(jvmtiEnv* jvmti, JNIEnv* jni) {
	NOTICE("VM end");

	_TLOG("VMDEATH");
}

static void ParseOptions(const char* options) {
	extern InstrFunc InstrClassEmpty;
	extern InstrFunc InstrClassDump;
	extern InstrFunc InstrClassPrint;
	extern InstrFunc InstrClassIdentity;
	extern InstrFunc InstrClassCompute;
	extern InstrFunc InstrClassComputeApp;
	extern InstrFunc InstrClassObjectInit;
	extern InstrFunc InstrClassNewArray;
	extern InstrFunc InstrClassANewArray;
	extern InstrFunc InstrClassMain;
	extern InstrFunc InstrClassHeap;
	extern InstrFunc InstrClassClientServer;

	typedef struct {
		InstrFunc* instrFunc;
		const char* name;
	} InstrFuncEntry;

	InstrFuncEntry instrFuncTable[] = {

	{ &InstrClassEmpty, "Empty" },

	{ &InstrClassDump, "Dump" },

	{ &InstrClassPrint, "Print" },

	{ &InstrClassIdentity, "Identity" },

	{ &InstrClassCompute, "Compute" },

	{ &InstrClassComputeApp, "ComputeApp" },

	{ &InstrClassObjectInit, "ObjectInit" },

	{ &InstrClassNewArray, "NewArray" },

	{ &InstrClassANewArray, "ANewArray" },

	{ &InstrClassMain, "Main" },

	{ &InstrClassHeap, "Heap" },

	};

	const char* instrFuncName = options != NULL ? options : "Empty";

	NOTICE("func index: %s", instrFuncName);

	const int instrFuncTableSize = sizeof(instrFuncTable)
			/ sizeof(instrFuncTable[0]);

	for (int i = 0; i < instrFuncTableSize; i++) {
		if (strcmp(instrFuncName, instrFuncTable[i].name) == 0) {
			instrFunc = instrFuncTable[i].instrFunc;
			return;
		}
	}

	EXCEPTION("Invalid name options for instfuncindex: %s.", options);
}

void PrintProperties(jvmtiEnv* jvmti) {
	jint count;
	char** properties;
	//jvmtiError error =
	(*jvmti)->GetSystemProperties(jvmti, &count, &properties);

	for (int i = 0; i < count; i++) {
		const char* property = properties[i];
		char* value;
		(*jvmti)->GetSystemProperty(jvmti, property, &value);

		_TLOG("Property %d: %s=%s", i, property, value);
	}
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char* options, void* reserved) {
	NOTICE("Agent loaded. options: %s", options);

	ParseOptions(options);

	jvmtiEnv* jvmti;
	jint res = (*jvm)->GetEnv(jvm, (void **) &jvmti, JVMTI_VERSION_1_0);
	if (res != JNI_OK || jvmti == NULL) {
		EXCEPTION(
				"Unable to access JVMTI Version 1 (0x%x)," " is your J2SE a 1.5 or newer version?" " JNIEnv's GetEnv() returned %d\n",
				JVMTI_VERSION_1, res);
	}

	PrintProperties(jvmti);

	(*jvmti)->GetSystemProperty(jvmti, "sun.java.command", &javaCommand);
	for (int i = 0; javaCommand[i] != '\0'; i++) {
		if (javaCommand[i] == '.') {
			javaCommand[i] = '/';
		}
	}

	_TLOG("sun.java.command: %s", javaCommand);

	jvmtiCapabilities cap;
	memset(&cap, 0, sizeof(cap));

	cap.can_generate_all_class_hook_events = true;
	cap.can_tag_objects = true;
	cap.can_generate_object_free_events = true;
	cap.can_generate_exception_events = true;
	cap.can_generate_garbage_collection_events = true;

	FrAddCapabilities(jvmti, &cap);

	jvmtiEventCallbacks callbacks;
	memset(&callbacks, 0, sizeof(jvmtiEventCallbacks));

	callbacks.VMStart = &VMStartEvent;
	callbacks.VMInit = &VMInitEvent;
	callbacks.ClassFileLoadHook = &ClassFileLoadEvent;
	callbacks.ClassLoad = &ClassLoadEvent;
	callbacks.ClassPrepare = &ClassPrepareEvent;
	callbacks.Exception = &ExceptionEvent;
	callbacks.GarbageCollectionStart = &GarbageCollectionStartEvent;
	callbacks.GarbageCollectionFinish = &GarbageCollectionFinishEvent;
	callbacks.ObjectFree = &ObjectFreeEvent;
	callbacks.ThreadStart = &ThreadStartEvent;
	callbacks.ThreadEnd = &ThreadEndEvent;
	callbacks.VMDeath = &VMDeathEvent;

	FrSetEventCallbacks(jvmti, &callbacks, (jint) sizeof(jvmtiEventCallbacks));

	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE,
			JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_CLASS_LOAD,
	NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE,
	NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_OBJECT_FREE,
	NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_START,
	NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH,
	NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_THREAD_START,
	NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_THREAD_END,
	NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_EXCEPTION,
	NULL);
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE,
			JVMTI_EVENT_GARBAGE_COLLECTION_START, NULL);

	FrSetInstrHandlerJvmtiEnv(jvmti);

	//FrOpenTransactionLog();

	return JNI_OK;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* jvm) {
	NOTICE("Agent unloaded");

	//FrCloseTransactionLog();
}
