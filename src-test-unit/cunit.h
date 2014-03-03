#ifndef CUNIT_H
#define CUNIT_H

/**
 *
 */
#include <stdio.h>
#include <stdlib.h>

#define _STREXPAND(token) #token
#define _STR(token) _STREXPAND(token)

#define _LOG(levelmsg, format, ...) \
	fprintf(stderr, levelmsg " | %4d %8ld | " format "\n", \
			0/*tldget()->threadId*/, 0l/*tldget()->threadTag*/, ##__VA_ARGS__)

#define FATAL(format, ...)	_LOG("FATAL", format, ##__VA_ARGS__)
#define ERROR(format, ...)	_LOG("ERROR", format, ##__VA_ARGS__)
#define WARN(format, ...)	_LOG("WARN ", format, ##__VA_ARGS__)
#define NOTICE(format, ...)	_LOG("NOTI ", format, ##__VA_ARGS__)
#define INFO(format, ...)	_LOG("INFO ", format, ##__VA_ARGS__)
#define DEBUG(format, ...)	_LOG("DEBUG", format, ##__VA_ARGS__)
#define TRACE(format, ...)	_LOG("TRACE", format, ##__VA_ARGS__)
#define STEP(format, ...)	_LOG("STEP ", format, ##__VA_ARGS__)

static inline int _exception(int unused) __attribute__((noreturn));

static inline int _exception(int unused) {
	exit(1);
}

/**
 *
 */
#define EXCEPTION(format, ...) exit( ( FATAL(format, ##__VA_ARGS__), 1 ) )

#define ASSERT(cond, format, ...) \
	((cond) ? 0 : _exception( ERROR("Assert Error found. '" #cond "' failed. " format " @ Line:" _STR(__LINE__), ##__VA_ARGS__) ))

#define CHECK(cond, format, ...) \
	((cond) ? 0 : _exception( ERROR("Check Error found. '" #cond "' failed. " format, ##__VA_ARGS__) ))

#endif
