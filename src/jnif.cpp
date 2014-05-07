/*
 * jnif.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */

#include "jnif.hpp"

#include <stdio.h>
#include <execinfo.h>
#include <unistd.h>

namespace jnif {

void Error::_backtrace() {
	void *array[20];
	size_t size;

	size = backtrace(array, 20);

	fprintf(stderr, "Error: exception on jnif:\n");
	backtrace_symbols_fd(array, size, STDERR_FILENO);
}

}
