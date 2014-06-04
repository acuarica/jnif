/**
 *
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "frlog.hpp"
#include "frjvmti.hpp"
#include "frstamp.hpp"
#include "frtlog.hpp"

static jlong _nextclassid = 1;
static jlong _nextobjectid = 1;

#define NULL_TAG 0

static unsigned char states[1L << CLASS_BITS];

static inline jlong _GetNextStamp(jint type, jlong threadId, jlong classId,
		jlong objectId) {
	return ((type & TYPE_MASK) << TYPE_POS)
			| ((threadId & THREAD_MASK) << THREAD_POS)
			| ((classId & CLASS_MASK) << CLASS_POS)
			| ((objectId & OBJECT_MASK) << OBJECT_POS);
}

static inline jlong _GetNextClassStamp() {
	jlong stamp = _GetNextStamp(TYPE_CLASS, tldget()->threadId, _nextclassid,
			_nextobjectid);

	_nextclassid++;
	_nextobjectid++;

	return stamp;
}

static inline jlong GetNextObjectStamp(jlong classId) {
	jlong stamp = _GetNextStamp(STAMP_TYPE_OBJECT, tldget()->threadId, classId,
			_nextobjectid);

	_nextobjectid++;

	return stamp;
}

void StampThread(jvmtiEnv* jvmti, jthread thread) {
	if (tldget()->threadTag == -1L) {
		jlong stamp;
		FrGetTag(jvmti, thread, &stamp);

		if (stamp != NULL_TAG) {
			tldget()->threadTag = stamp;
		}
	}
}

static inline bool IsClassObject(jvmtiEnv* jvmti, jobject object) {
	jvmtiError error = jvmti->GetClassSignature((jclass) object, NULL, NULL);

	return error == JVMTI_ERROR_NONE;
}

static void PrepareClass(jvmtiEnv *jvmti, JNIEnv* jni, jclass klass,
		jlong stamp) {
	jint interfacecount;
	jclass* interfaces;

	FrGetImplementedInterfaces(jvmti, klass, &interfacecount, &interfaces);
	jlong* interstamps = (jlong*) malloc(sizeof(jlong) * interfacecount);
	for (int i = 0; i < interfacecount; i++) {
		interstamps[i] = StampClass(jvmti, jni, interfaces[i]);
	}
	FrDeallocate(jvmti, interfaces);

	_TLOGBEGIN("PREPARECLASS:%ld:#%d", stamp, interfacecount);

	for (int i = 0; i < interfacecount; i++) {
		fprintf(tldget()->_tlog, "#%ld", interstamps[i]);
	}
	free(interstamps);

	jint fcount;
	jfieldID* idlist;

	FrGetClassFields(jvmti, klass, &fcount, &idlist);

	fprintf(tldget()->_tlog, ":#%d", fcount);

	for (int i = 0; i < fcount; i++) {
		char *fname;
		char *fsig;

		FrGetFieldName(jvmti, klass, idlist[i], &fname, &fsig, NULL);

		fprintf(tldget()->_tlog, "#%d@%s@%s", i, fname, fsig);

		FrDeallocate(jvmti, fname);
		FrDeallocate(jvmti, fsig);
	}
	FrDeallocate(jvmti, idlist);

	fprintf(tldget()->_tlog, "\n");
}

static inline void _CheckInitStates() {
	static bool init = false;
	if (!init) {
		init = true;
		memset(&states, 0, sizeof(states[0]) * (1L << CLASS_BITS));
	}
}

/**
 *
 */
jlong StampClass(jvmtiEnv* jvmti, JNIEnv* jni, jclass klass) {
	jlong stamp;

	FrGetTag(jvmti, klass, &stamp);

	if (stamp == NULL_TAG) {
		jclass superklass = jni->GetSuperclass(klass);
		jlong superklassstamp =
				superklass != NULL ? StampClass(jvmti, jni, superklass) : -1;

		stamp = _GetNextClassStamp();

		FrSetTag(jvmti, klass, stamp);

		char* classsig;
		FrGetClassSignature(jvmti, klass, &classsig, NULL);

		_TLOG("STAMPCLASS:%ld:%ld:%ld:%s", stamp, GetClassIdFromStamp(stamp),
				superklassstamp, classsig);

		FrDeallocate(jvmti, classsig);
	}

	_CheckInitStates();

	if (!states[GetClassIdFromStamp(stamp)]) {
		jint status;
		FrGetClassStatus(jvmti, klass, &status);
		if (status & JVMTI_CLASS_STATUS_PREPARED) {
			PrepareClass(jvmti, jni, klass, stamp);
			states[GetClassIdFromStamp(stamp)] = true;
		}
	}

	return stamp;
}

/*
 * May only be called during JVMTI_PHASE_START or JVMTI_PHASE_LIVE phase due to the use of GetTag and SetTag.
 *
 * object: Non-null valid jobject to stamp.
 */
jlong StampObject(jvmtiEnv* jvmti, JNIEnv* jni, jobject object) {
	return -2;

	jlong stamp;

	FrGetTag(jvmti, object, &stamp);

	if (stamp != NULL_TAG) {
		return stamp;
	}

	if (IsClassObject(jvmti, object)) {
		stamp = StampClass(jvmti, jni, (jclass) object);
	} else {
		jclass klass = jni->GetObjectClass(object);
		jlong classStamp = StampClass(jvmti, jni, klass);
		jlong classId = GetClassIdFromStamp(classStamp);

		stamp = GetNextObjectStamp(classId);
		FrSetTag(jvmti, object, stamp);

		static int times = 0;
		if (times < 0) {

			jclass objectclass = jni->FindClass("java/lang/Object");

			ASSERT(objectclass != NULL, "");

			jfieldID fid = jni->GetFieldID(objectclass, "__elcampito__", "J");

			ASSERT(fid != NULL, "");
			//fprintf(stderr, "%ld --\n", (long) fid);
			ASSERT(object != NULL, "");

			jlong value = jni->GetLongField(object, fid);
			//printf("%ld\n", value);

			if (value == 0) {
				jni->SetLongField(object, fid, times);
			}

//			_TLOG("__elcampito__:%ld", value);

			times++;
		}

	}

	return stamp;
}
