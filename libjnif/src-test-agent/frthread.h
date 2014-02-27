#ifndef __FRTHREAD_H__
#define	__FRTHREAD_H__

/**
 *
 */
#include <jni.h>

/**
 *
 */
typedef struct __ThreadLocalDataTag {
	jint threadId;
	char name[1024];
	jint priority;
	jboolean isDaemon;
	jlong threadTag;
	int socketfd;
	FILE* _tlog;
} ThreadLocalData;

/**
 *
 */
extern __thread ThreadLocalData __tld;

extern jint __nextthreadid;

/**
 *
 */
inline static ThreadLocalData* tldget() {
	if (__tld.threadId == -1) {
		__tld.threadId = __nextthreadid;
		__nextthreadid++;
	}

	return &__tld;
}

#endif
