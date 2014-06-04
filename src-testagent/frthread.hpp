#ifndef __FRTHREAD_H__
#define	__FRTHREAD_H__

/**
 *
 */
#include <jni.h>

#include <fstream>
#include <sstream>

#include "testagent.hpp"

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
	double instrTime;
};

}

class Profiler {
public:

	ThreadLocalData* tld;
	Profiler(ThreadLocalData* tld) :
			tld(tld) {
	}

	void prof(const std::string& runId, const std::string& className,
			double time) {
//		if (args.instrFuncName != "ClientServer") {
			open();

			fprintf(tld->_prof, "%s,%s,%f\n", runId.c_str(), className.c_str(),
					time);
	//	}
	}

	void open() {
		if (tld->_prof != NULL) {
			return;
		}

		std::stringstream ss;
		ss << args.profPath << ".tid-" << tld->threadId << ".prof";

		tld->_prof = fopen(ss.str().c_str(), "w+");

		if (tld->_prof == NULL) {
			perror("Unable to create prof file");
			exit(1);
		}

	}
};

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

#endif
