#include "frthread.hpp"

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

//#define _TLOGBEGIN(format, ...) ( 0 )

//#define _TLOG(format, ...) ( FrOpenTransactionLog(&tldget()->_tlog, tldget()->threadId), fprintf( tldget()->_tlog, format "\n", ##__VA_ARGS__) )
#define _TLOG(format, ...) _TLOGBEGIN(format "\n", ##__VA_ARGS__)

void FrOpenTransactionLog(FILE** _log, int tid);

//static void FrCloseTransactionLog() {
//NOTICE("Closing transaction log...");

//fclose(_log);

//free(_logbuf);
//}
