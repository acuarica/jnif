#ifndef __FRINSTR_H__
#define	__FRINSTR_H__

/**
 *
 */
#include <string.h>
#include <stdbool.h>

#include <jvmti.h>

#define FR_PROXY_CLASS "frproxy/FrInstrProxy"

extern int eventspersample;

static inline bool FrIsProxyClassName(const char* className) {
	return strcmp(className, FR_PROXY_CLASS) == 0;
}

static inline bool FrIsProxyClassSignature(const char* classSignature) {
	return strcmp(classSignature, "L" FR_PROXY_CLASS ";") == 0;
}

void FrInstrClassFile(jvmtiEnv* jvmti, const unsigned char* classFileImage, int classFileImageLen,
		const char* className, int* new_class_data_len, unsigned char** new_class_data);
void FrSetInstrHandlerNatives(jvmtiEnv* jvmti, JNIEnv* jni, jclass klass);
void FrSetInstrHandlerJvmtiEnv(jvmtiEnv* jvmti);

void FrInstrClassFile(jvmtiEnv* jvmti, const unsigned char* classFileImage, int classFileImageLen,
		const char* className, int* new_class_data_len, unsigned char** new_class_data);

#endif
