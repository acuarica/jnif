#ifndef __FRTHREAD_H__
#define	__FRTHREAD_H__

/**
 *
 */
#include <jni.h>

#include <fstream>
#include <sstream>

#include "testagent.hpp"

#include "../src-include/Profiler.hpp"

/**
 *
 */
extern "C" {

struct ThreadLocalData {
	jint threadId;
	jlong threadTag;
	//char name[1024];
	jint priority;
	jboolean isDaemon;
	int socketfd;
	FILE* _tlog;
	FILE* _prof;
	int classLoadedStack;
};

}

#ifdef __MACH__
#define AGENT_THREAD_LOCAL __thread
#else
//#define AGENT_THREAD_LOCAL thread_local
#define AGENT_THREAD_LOCAL __thread
#endif

/**
 *
 */
extern AGENT_THREAD_LOCAL ThreadLocalData __tld;

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

inline Profiler getProf() {
	return Profiler(&tldget()->_prof, tldget()->threadId, args.runId.c_str());
}

#endif
