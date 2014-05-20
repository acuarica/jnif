/**
 *
 */
#include <string.h>
#include <jni.h>

#include "frlog.hpp"
#include "frjvmti.hpp"
#include "frstamp.hpp"
#include "frinstr.hpp"
#include "frtlog.hpp"

/**
 *
 */
static jvmtiEnv* _jvmti;

#define HANDLER(name) Java_frproxy_FrInstrProxy_ ## name

#define DEFHANDLER(name) JNIEXPORT void JNICALL HANDLER(name)

#define WITH(jni, string, body) \
	jsize string ## len = jni->GetStringUTFLength(string); \
	const char* string ## utf8 = jni->GetStringUTFChars(string, NULL ); \
	body; \
	jni->ReleaseStringUTFChars(string, string ## utf8);

static inline jlong FrLiveStamp(JNIEnv* jni, jobject object) {
	return StampObject(_jvmti, jni, object);
}

DEFHANDLER(alloc) (JNIEnv* jni, jclass proxyClass, jobject thisObject) {
	jlong stamp = StampObject(_jvmti, jni, thisObject);

	_TLOG("ALLOC:%ld", stamp);
}

DEFHANDLER(newArrayEvent) (JNIEnv* jni, jclass proxyClass, jint count,
		jobject thisArray, jint atype) {
	jlong stamp = StampObject(_jvmti, jni, thisArray);

	_TLOG("NEWARRAY:%ld:%d:%d", stamp, count, atype);
}

DEFHANDLER(aNewArrayEvent) (JNIEnv* jni, jclass proxyClass, jint count,
		jobject thisArray, jstring type) {

	WITH(jni, type, {

	jlong stamp = StampObject(_jvmti, jni, thisArray);

	_TLOG("ANEWARRAY:%ld:%d:%.*s", stamp, count, typelen, typeutf8);

	});
}

DEFHANDLER(multiANewArray1Event) (JNIEnv* jni, jclass proxyClass, int count1,
		jobject thisArray, jstring type) {
	jlong stamp = StampObject(_jvmti, jni, thisArray);

	_TLOG("MULTI1:%ld:%d", stamp, count1);
}

DEFHANDLER(multiANewArray2Event) (JNIEnv* jni, jclass proxyClass, int count1,
		int count2, jobject thisArray, jstring type) {
	jlong stamp = StampObject(_jvmti, jni, thisArray);

	_TLOG("MULTI2:%ld:%d%d", stamp, count1, count2);
}

DEFHANDLER(multiANewArrayNEvent) (JNIEnv* jni, jclass proxyClass,
		jobject thisArray, int dims, jstring type) {
	jlong stamp = StampObject(_jvmti, jni, thisArray);

	_TLOG("MULTIN:%ld:%d", stamp, dims);
}

DEFHANDLER(putFieldEvent) (JNIEnv* jni, jclass proxyClass, jobject thisObject,
		jobject newValue, jstring fieldName) {
	WITH(jni, fieldName,
			{

			jlong thisObjectStamp = FrLiveStamp(jni, thisObject);

			jlong newValueStamp = newValue!=NULL ? FrLiveStamp(jni, newValue) : -1;

			_TLOG("PUTFIELD:%.*s:%ld:%ld", fieldNamelen, fieldNameutf8, thisObjectStamp, newValueStamp);

			});
}

DEFHANDLER(putStaticEvent) (JNIEnv* jni, jclass proxyClass, jobject newValue,
		jstring thisClassName, jstring fieldName) {
	WITH(jni, thisClassName,
			{ WITH(jni, fieldName, {

			jlong newValueStamp = newValue!=NULL ? FrLiveStamp(jni, newValue) : -1;

			_TLOG("PUTSTATIC:%.*s:%.*s:%ld", fieldNamelen, fieldNameutf8, thisClassNamelen,thisClassNameutf8, newValueStamp);

			}); });
}

DEFHANDLER(aastoreEvent) (JNIEnv* jni, jclass proxyClass, jint index,
		jobject newValue, jobject thisArray) {

	jlong thisArrayStamp = FrLiveStamp(jni, thisArray);
	jlong newValueStamp = newValue != NULL ? FrLiveStamp(jni, newValue) : -1;

	_TLOG("AASTORE:%d:%ld:%ld", index, thisArrayStamp, newValueStamp);
}

DEFHANDLER(enterMethod) (JNIEnv* jni, jclass proxyClass, jstring className,
		jstring methodName) {
	_TLOG("ENTERMETHOD");

//	fprintf(stderr, "we just enter a method!!!\n");

	WITH(jni, className,
			{ WITH(jni, methodName, {

			_TLOG("ENTERMETHOD:%.*s:%.*s", classNamelen, classNameutf8, methodNamelen,methodNameutf8);

			}); });

}

DEFHANDLER(enterMainMethod) (JNIEnv* jni, jclass proxyClass) {
	_TLOG("ENTERMAIN");
}

DEFHANDLER(exitMainMethod) (JNIEnv* jni, jclass proxyClass) {
	_TLOG("EXITMAIN");
}

void FrSetInstrHandlerNatives(jvmtiEnv* jvmti, JNIEnv* jni, jclass proxyClass) {
	static JNINativeMethod methods[] = {

	{ "alloc", "(Ljava/lang/Object;)V", (void *) &HANDLER(alloc) },

	{ "newArrayEvent", "(ILjava/lang/Object;I)V",
			(void *) &HANDLER(newArrayEvent) },

	{ "aNewArrayEvent", "(ILjava/lang/Object;Ljava/lang/String;)V",
			(void *) &HANDLER(aNewArrayEvent) },

	{ "multiANewArray1Event", "(ILjava/lang/Object;Ljava/lang/String;)V",
			(void*) &HANDLER(multiANewArray1Event) },

	{ "multiANewArray2Event", "(IILjava/lang/Object;Ljava/lang/String;)V",
			(void*) &HANDLER(multiANewArray2Event) },

	{ "multiANewArrayNEvent", "(Ljava/lang/Object;ILjava/lang/String;)V",
			(void*) &HANDLER(multiANewArrayNEvent) },

	{ "putFieldEvent",
			"(Ljava/lang/Object;Ljava/lang/Object;Ljava/lang/String;)V",
			(void*) &HANDLER(putFieldEvent) },

	{ "putStaticEvent",
			"(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;)V",
			(void*) &HANDLER(putStaticEvent) },

	{ "aastoreEvent", "(ILjava/lang/Object;Ljava/lang/Object;)V",
			(void*) &HANDLER(aastoreEvent) },

	{ "enterMethod", "(Ljava/lang/String;Ljava/lang/String;)V",
			(void*) &HANDLER(enterMethod) },

	{ "enterMainMethod", "()V", (void*) &HANDLER(enterMainMethod) },

	{ "exitMainMethod", "()V", (void*) &HANDLER(exitMainMethod) },

	};

	jint res = jni->RegisterNatives(proxyClass, methods,
			sizeof(methods) / sizeof(methods[0]));

	CHECK(res == 0, "reg natives");
}

void FrSetInstrHandlerJvmtiEnv(jvmtiEnv* jvmti) {
	ASSERT(jvmti != NULL, "Setting jvmti null");

	_jvmti = jvmti;
}
