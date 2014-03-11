/*
 * jniferr.hpp
 *
 *  Created on: Mar 10, 2014
 *      Author: luigi
 */

#ifndef JNIFERR_HPP
#define JNIFERR_HPP

#include <stdlib.h>

#define _STREXPAND(token) #token
#define _STR(token) _STREXPAND(token)

#define _LOG(levelmsg, format, ...) \
	fprintf(stderr, levelmsg " | %4d %8ld | " format "\n", 0, 0l, ##__VA_ARGS__)

#define FATAL(format, ...)	_LOG("FATAL", format, ##__VA_ARGS__)
#define ERROR(format, ...)	_LOG("ERROR", format, ##__VA_ARGS__)

static inline int _exception(int) __attribute__((noreturn));

static inline int _exception(int) {
	exit(1);
}

#define EXCEPTION(format, ...) exit( ( FATAL(format, ##__VA_ARGS__), 1 ) )

#define ASSERT(cond, format, ...) \
	((cond) ? 0 : _exception( ERROR("Assert Error found. '" #cond "' failed. " format " @ Line:" _STR(__LINE__), ##__VA_ARGS__) ))

#define CHECK(cond, format, ...) \
	((cond) ? 0 : _exception( ERROR("Check Error found. '" #cond "' failed. " format, ##__VA_ARGS__) ))

#endif
