#ifndef __FREXCEPTION_H__
#define	__FREXCEPTION_H__

/**
 *
 */
#include <stdlib.h>

#include "frlog.h"

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

#include <errno.h>

static inline int check_std_error(int res, const char* message) {
	if (res == -1) {
		ERROR("errno: %d, %s", errno, message);
		exit(1);
	}

	return res;
}

#endif /* __FREXCEPTION_H__ */
