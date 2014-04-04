/*
 * ExceptionManager.hpp
 *
 *  Created on: Apr 4, 2014
 *      Author: luigi
 */

#ifndef JNIF_EXCEPTIONMANAGER_HPP
#define JNIF_EXCEPTIONMANAGER_HPP

#include <stdio.h>

namespace jnif {

class ExceptionManager {
protected:

	template<typename ... TArgs>
	static inline void raise(const char* format, TArgs ... args) {
		fprintf(stderr, "Exception jnif: ");
		fprintf(stderr, format, args ...);
		fprintf(stderr, "\n");
		throw "Error!!!";
	}

	template<typename ... TArgs>
	static inline void assert(bool cond, const char* format, TArgs ... args) {
		if (!cond) {
			raise(format, args...);
		}
	}

	template<typename ... TArgs>
	static inline void check(bool cond, const char* format, TArgs ... args) {
		if (!cond) {
			raise(format, args...);
		}
	}
};

}

#endif
