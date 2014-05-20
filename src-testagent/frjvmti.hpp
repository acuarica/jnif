#ifndef __FRJVMTI_H__
#define	__FRJVMTI_H__

#define ERR_PREFIX "testagent agent error: "

#ifndef ERR_PREFIX
#error ERR_PREFIX macro has to be deffined
#endif

#include <jvmti.h>
#include <stdlib.h>

#include "frexception.hpp"

// error nums
#define ERR 10000
#define ERR_STD 10002
#define ERR_JVMTI 10003

/*
 * Check error routine - reporting on one place
 */
//static void check_std_error(int retval, int errorval, const char *str) {
//	if (retval == errorval) {
//		static const int BUFFSIZE = 1024;
//
//		char msgbuf[BUFFSIZE];
//		snprintf(msgbuf, BUFFSIZE, "%s%s", ERR_PREFIX, str);
//		perror(msgbuf);
//		exit(ERR_STD);
//	}
//}
/**
 * Every JVMTI interface returns an error code, which should be checked
 *   to avoid any cascading errors down the line.
 *   The interface GetErrorName() returns the actual enumeration constant
 *   name, making the error messages much easier to understand.
 */
static void __FrCheckJvmtiError(jvmtiEnv* env, jvmtiError error,
		const char *str) {
	if (error != JVMTI_ERROR_NONE) {
		char* errnum_str = NULL;
		env->GetErrorName(error, &errnum_str);

		FATAL("%sJVMTI: %d(%s): Unable to %s.\n", ERR_PREFIX, error,
				(errnum_str == NULL ? "Unknown" : errnum_str),
				(str == NULL ? "" : str));

		exit(ERR_JVMTI);
	}
}

static inline void FrAllocate(jvmtiEnv* env, jlong size,
		unsigned char** mem_ptr) {
	jvmtiError error = env->Allocate(size, mem_ptr);
	__FrCheckJvmtiError(env, error, "Allocate");
}

static inline void FrDeallocate(jvmtiEnv* env, void* mem) {
	jvmtiError error = env->Deallocate((unsigned char *) mem);
	__FrCheckJvmtiError(env, error, "Deallocate");
}

static inline void FrGetClassSignature(jvmtiEnv* env, jclass klass,
		char** signature_ptr, char** generic_ptr) {
	jvmtiError error = env->GetClassSignature(klass, signature_ptr,
			generic_ptr);
	__FrCheckJvmtiError(env, error, "GetClassSignature");
}

static inline void FrAddCapabilities(jvmtiEnv* env,
		const jvmtiCapabilities* capabilities_ptr) {
	jvmtiError error = env->AddCapabilities(capabilities_ptr);
	__FrCheckJvmtiError(env, error, "AddCapabilities");
}

static inline void FrSetEventCallbacks(jvmtiEnv* env,
		const jvmtiEventCallbacks* callbacks, jint size_of_callbacks) {
	jvmtiError error = env->SetEventCallbacks(callbacks, size_of_callbacks);
	__FrCheckJvmtiError(env, error, "SetEventCallbacks");
}

static inline void FrSetEventNotificationMode(jvmtiEnv* env,
		jvmtiEventMode mode, jvmtiEvent event_type, jthread event_thread) {
	jvmtiError error = env->SetEventNotificationMode(mode, event_type,
			event_thread);
	__FrCheckJvmtiError(env, error, "SetEventNotificationMode");
}

static inline void FrGetTag(jvmtiEnv* env, jobject object, jlong* tag_ptr) {
	jvmtiError error = env->GetTag(object, tag_ptr);
	__FrCheckJvmtiError(env, error, "GetTag");
}

static inline void FrSetTag(jvmtiEnv* env, jobject object, jlong tag) {
	jvmtiError error = env->SetTag(object, tag);
	__FrCheckJvmtiError(env, error, "SetTag");
}

static inline void FrFollowReferences(jvmtiEnv* env, jint heap_filter,
		jclass klass, jobject initial_object,
		const jvmtiHeapCallbacks* callbacks, const void* user_data) {
	jvmtiError error = env->FollowReferences(heap_filter, klass, initial_object,
			callbacks, user_data);
	__FrCheckJvmtiError(env, error, "FollowReferences");
}

static inline void FrGetThreadInfo(jvmtiEnv* env, jthread thread,
		jvmtiThreadInfo* info_ptr) {
	jvmtiError error = env->GetThreadInfo(thread, info_ptr);
	__FrCheckJvmtiError(env, error, "GetThreadInfo");
}

static inline jvmtiError FrGetClassFields(jvmtiEnv* env, jclass klass,
		jint* field_count_ptr, jfieldID** fields_ptr) {
	jvmtiError error = env->GetClassFields(klass, field_count_ptr, fields_ptr);
	__FrCheckJvmtiError(env, error, "GetClassFields");
	return error;
}

static inline void FrGetFieldName(jvmtiEnv* env, jclass klass, jfieldID field,
		char** name_ptr, char** signature_ptr, char** generic_ptr) {
	jvmtiError error = env->GetFieldName(klass, field, name_ptr, signature_ptr,
			generic_ptr);
	__FrCheckJvmtiError(env, error, "GetFieldName");
}

static inline void FrGetLoadedClasses(jvmtiEnv* env, jint* class_count_ptr,
		jclass** classes_ptr) {
	jvmtiError error = env->GetLoadedClasses(class_count_ptr, classes_ptr);
	__FrCheckJvmtiError(env, error, "GetLoadedClasses");
}

static inline void FrGetImplementedInterfaces(jvmtiEnv* jvmti, jclass klass,
		jint* interfacecountptr, jclass** interfacesptr) {
	jvmtiError error = jvmti->GetImplementedInterfaces(klass, interfacecountptr,
			interfacesptr);
	__FrCheckJvmtiError(jvmti, error, "GetImplementedInterfaces");
}

static inline void FrGetClassStatus(jvmtiEnv* jvmti, jclass klass,
		jint* statusptr) {
	jvmtiError error = jvmti->GetClassStatus(klass, statusptr);
	__FrCheckJvmtiError(jvmti, error, "GetClassStatus");
}

#endif
