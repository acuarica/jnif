#include "frthread.h"

extern jlong __globaleventno;

/**
 * http://gcc.gnu.org/onlinedocs/gcc-4.3.5/gcc/Atomic-Builtins.html
 */
static inline jlong getglobal() {

	return __sync_add_and_fetch(&__globaleventno, 1);

	//__globaleventno++;
//	return __globaleventno;
}

#define _TLOGBEGIN(format, ...) ( FrOpenTransactionLog(&tldget()->_tlog, tldget()->threadId), \
		fprintf( tldget()->_tlog, "%ld:" format, getglobal(), ##__VA_ARGS__) )

//#define _TLOG(format, ...) ( FrOpenTransactionLog(&tldget()->_tlog, tldget()->threadId), fprintf( tldget()->_tlog, format "\n", ##__VA_ARGS__) )
#define _TLOG(format, ...) _TLOGBEGIN(format "\n", ##__VA_ARGS__)

static inline void FrOpenTransactionLog(FILE** _log, int tid) {
	if (*_log != NULL) {
		return;

	}

	//const int size = 1024 * 1024;

	//_logbuf = malloc(size);

	char filename[512];
	sprintf(filename, "db/tlog.%04d.log", tid);

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

//static void FrCloseTransactionLog() {
	//NOTICE("Closing transaction log...");

	//fclose(_log);

	//free(_logbuf);
//}
