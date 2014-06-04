/**
 *
 */
#include "testagent.hpp"

#include "frthread.hpp"

/**
 *
 */
AGENT_THREAD_LOCAL ThreadLocalData __tld
= {.threadId = -1, .threadTag = -1, .priority = 0, .isDaemon =
	false, .socketfd = -1, ._tlog = NULL, ._prof=NULL, .classLoadedStack=0, .instrTime=0};

jint __nextthreadid = 1;

jlong __globaleventno = 0;

void FrOpenTransactionLog(FILE** _log, int tid) {
	if (*_log != NULL) {
		return;

	}

	//const int size = 1024 * 1024;

	//_logbuf = malloc(size);

	char filename[512];
	sprintf(filename, "%stlog.%04d.log", args.outputPath.c_str(), tid);

	//NOTICE("%s", filename);

	*_log = fopen(filename, "w+");

	if (*_log == NULL) {
		perror("Unable to create log file");

		//int err = errno;
		//ERROR("errno: %d, %s", err, "Unable to open ");
		exit(1);
	}

	//check_std_error(setvbuf(_log, _logbuf, _IOFBF, size), "setvbuf");
}
