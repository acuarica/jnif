/*
 * Error.cpp
 *
 *  Created on: Jun 13, 2014
 *      Author: luigi
 */
#include "Error.hpp"
#include "base.hpp"

#include <stdio.h>
#include <execinfo.h>
#include <unistd.h>

namespace jnif {

void Error::_backtrace(std::ostream& os) {
	void* array[20];
	size_t size;

	size = backtrace(array, 20);

	char** symbols = backtrace_symbols(array, size);
	for (size_t i = 0; i < size; i++) {
		const char* symbol = symbols[i];
		os << "    " << symbol << std::endl;
	}

	free(symbols);
}

}
