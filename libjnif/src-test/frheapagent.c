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
#include "frheapdump.h"

static void JNICALL ClassFileLoadEvent(jvmtiEnv* jvmti, JNIEnv* jni,
		jclass class_being_redefined, jobject loader, const char* name,
		jobject protection_domain, jint class_data_len,
		const unsigned char* class_data, jint* new_class_data_len,
		unsigned char** new_class_data) {
	_TLOG("CLASSFILELOAD:%s", name);

	if (!FrIsProxyClassName(name)) {
		FrInstrClassFile(jvmti, class_data, class_data_len, name,
				new_class_data_len, new_class_data);
	}
}

static void JNICALL ClassLoadEvent(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread,
		jclass klass) {
	char* classsig;
	FrGetClassSignature(jvmti, klass, &classsig, NULL );

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

	FrGetClassSignature(jvmti, klass, &classsig, NULL );

	_TLOG("CLASSPREPARE:%s", classsig);

	if (FrIsProxyClassSignature(classsig)) {
		FrSetInstrHandlerNatives(jvmti, jni, klass);
	}

	FrDeallocate(jvmti, classsig);

	StampClass(jvmti, jni, klass);
}

extern unsigned char frproxy_FrInstrProxy_class[];
extern unsigned int frproxy_FrInstrProxy_class_len;

/**
 * Changes from JVMTI_PHASE_PRIMORDIAL to JVMTI_PHASE_START phase.
 * This phase is when the JVM is starting to execute java bytecode.
 * Therefore since we want to catch all the event in this bootstrap phase, we need
 * to define the proxy class as earlier as possibly.
 *
 * Also it is needed to stamp classes that won't be prepared ever, like primitive classes
 * and Finalizer (why?)
 */
static void JNICALL VMStartEvent(jvmtiEnv* jvmti, JNIEnv* jni) {
	NOTICE("VM started");

	_TLOG("VMSTART");

//	jclass proxyClass = (*jni)->DefineClass(jni, FR_PROXY_CLASS, NULL, frproxy_FrInstrProxy_class,
//			frproxy_FrInstrProxy_class_len);
//
//	if (proxyClass == NULL){
//		ERROR("Error on define class");
//	}
}

static void JNICALL VMInitEvent(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread) {
	NOTICE("VM init");

	_TLOG("VMINIT");

	StampThread(jvmti, thread);

	NextHeapRequest(jvmti);
}

static void JNICALL ExceptionEvent(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread,
		jmethodID method, jlocation location, jobject exception,
		jmethodID catch_method, jlocation catch_location) {
	//NOTICE("Exception ocurred");

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
	if (options != NULL ) {
		eventspersample = atoi(options);

		CHECK(eventspersample != 0,
				"Invalid options for eventspersample: %s. Got zero.", options);
	}
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char* options, void* reserved) {
	NOTICE("Agent loaded. options: %s", options);

	ParseOptions(options);

	jvmtiEnv* jvmti;
	jint res = (*jvm)->GetEnv(jvm, (void **) &jvmti, JVMTI_VERSION_1_0);
	if (res != JNI_OK || jvmti == NULL ) {
		EXCEPTION(
				"Unable to access JVMTI Version 1 (0x%x)," " is your J2SE a 1.5 or newer version?" " JNIEnv's GetEnv() returned %d\n",
				JVMTI_VERSION_1, res);
	}

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
			JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_CLASS_LOAD,
			NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE,
			NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_OBJECT_FREE,
			NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_START,
			NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH,
			NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_THREAD_START,
			NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_THREAD_END,
			NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_EXCEPTION,
			NULL );
	FrSetEventNotificationMode(jvmti, JVMTI_ENABLE,
			JVMTI_EVENT_GARBAGE_COLLECTION_START, NULL );

	FrSetInstrHandlerJvmtiEnv(jvmti);

	//FrOpenTransactionLog();

	return JNI_OK;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* jvm) {
	NOTICE("Agent unloaded");

	//FrCloseTransactionLog();
}
