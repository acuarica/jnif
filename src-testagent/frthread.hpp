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
class ThreadLocalData {
public:

	jint threadId;
	jlong threadTag;
	//char name[1024];
	jint priority;
	jboolean isDaemon;
	int socketfd;
	FILE* _tlog;
	FILE* _prof;

	ThreadLocalData() :
			threadId(-1), threadTag(-1), /*name("NOT INIT"),*/priority(0), isDaemon(
					false), socketfd(-1), _tlog(nullptr), _prof(nullptr) {
	}

	void prof(const std::string& runId, const std::string& appName,
			const std::string& instrName, const std::string& className,
			double time) {
		open();

		fprintf(_prof, "%s:%s:%s:%s:%f\n", runId.c_str(), appName.c_str(),
				instrName.c_str(), className.c_str(), time);
	}

private:

	void open() {
//			if (!tldget()->prof.is_open()) {
//				stringstream ss;
//				ss << args.profPath << ".tid-" << tldget()->threadId << ".prof";
//				tldget()->prof.open(ss.str().c_str());
//				tldget()->prof.precision(15);
//			}

		if (_prof != nullptr) {
			return;
		}

		std::stringstream ss;
		ss << args.profPath << ".tid-" << threadId << ".prof";

		_prof = fopen(ss.str().c_str(), "w+");

		if (_prof == nullptr) {
			perror("Unable to create prof file");
			exit(1);
		}

	}

};

#ifdef __MACH__
#define AGENT_THREAD_LOCAL __thread
#else
#define AGENT_THREAD_LOCAL thread_local
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
