/**
 *
 */
#include <stdbool.h>

#include "frthread.hpp"

/**
 *
 */
__thread ThreadLocalData __tld = { .threadId = -1, .threadTag = -1, .name = "<NOT INIT>", .priority = 0, .isDaemon =
		false, .socketfd = -1, ._tlog = NULL };

jint __nextthreadid = 1;

jlong __globaleventno = 0;
