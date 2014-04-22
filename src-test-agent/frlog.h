#ifndef __FRLOG_H__
#define	__FRLOG_H__

/**
 *
 */
#include <stdio.h>

#include "frthread.h"

#define _STREXPAND(token) #token
#define _STR(token) _STREXPAND(token)

static inline int nothing() {
	return 0;
}

//#define NO_LOG

#ifndef NO_LOG
//#define _LOG(levelmsg, format, ...) \
//	fprintf(stderr, levelmsg " | %4d %8ld | " format " | %s @ " __FILE__ ":" _STR(__LINE__) "\n", \
//			tldget()->threadId, tldget()->threadTag, ##__VA_ARGS__, __func__)
#define _LOG(levelmsg, format, ...) \
	fprintf(stderr, levelmsg " | %4d %8ld | " format "\n", \
			tldget()->threadId, tldget()->threadTag, ##__VA_ARGS__)
#else
#define _LOG(levelmsg, format, ...) nothing()
#endif

#define FATAL(format, ...)	_LOG("FATAL", format, ##__VA_ARGS__)
#define ERROR(format, ...)	_LOG("ERROR", format, ##__VA_ARGS__)
#define WARN(format, ...)	_LOG("WARN ", format, ##__VA_ARGS__)
#define NOTICE(format, ...)	_LOG("NOTI ", format, ##__VA_ARGS__)
#define INFO(format, ...)	_LOG("INFO ", format, ##__VA_ARGS__)
#define DEBUG(format, ...)	_LOG("DEBUG", format, ##__VA_ARGS__)
#define TRACE(format, ...)	_LOG("TRACE", format, ##__VA_ARGS__)
#define STEP(format, ...)	_LOG("STEP ", format, ##__VA_ARGS__)

#endif
