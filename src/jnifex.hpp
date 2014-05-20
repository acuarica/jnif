/*
 * jnifex.hpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */
#ifndef JNIFEX_HPP
#define JNIFEX_HPP

#include <sstream>

#include <iostream>

namespace jnif {

/**
 * The magic number signature that must appear at the beginning of each
 * class file.
 */
enum Magic {
	CLASSFILE_MAGIC = 0xcafebabe
};

/**
 * This class contains static method to facilitate error handling mechanism.
 */
class Error {
public:

	template<typename ... TArgs>
	static inline void raise(TArgs ... args) __attribute__((noreturn)) {
		std::stringstream message;
		_raise(message, args...);

		std::stringstream stackTrace;
		_backtrace(stackTrace);

		throw JnifException(message.str(), stackTrace.str());
	}

	template<typename ... TArgs>
	static inline void assert(bool cond, TArgs ... args) {
		if (!cond) {
			raise(args...);
		}
	}

	template<typename ... TArgs>
	static inline void check(bool cond, TArgs ... args) {
		if (!cond) {
			raise(args...);
		}
	}

private:

	static void _backtrace(std::ostream& os);

	static inline void _raise(std::ostream&) {
	}

	template<typename TArg, typename ... TArgs>
	static inline void _raise(std::ostream& os, TArg arg, TArgs ... args) {
		os << arg;
		_raise(os, args...);
	}

};

}

#endif
