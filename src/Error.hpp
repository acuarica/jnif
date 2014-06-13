/*
 * Error.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_ERROR_HPP
#define JNIF_ERROR_HPP

#include "base.hpp"

#include <sstream>

namespace jnif {

/**
 * This class contains static method to facilitate error handling mechanism.
 */
class Error {
public:

	template<typename ... TArgs>
	static void raise(const TArgs& ... args) __attribute__((noreturn));

//	template<typename ... TArgs>
//	static void raise(const TArgs& ... args) __attribute__((noreturn)) {
//		std::stringstream message;
//		_raise(message, args...);
//
//		std::stringstream stackTrace;
//		_backtrace(stackTrace);
//
//		throw JnifException(message.str(), stackTrace.str());
//	}

	template<typename ... TArgs>
	static inline void assert(bool cond, const TArgs& ... args) {
		if (!cond) {
			raise(args...);
		}
	}

	template<typename T, typename ... TArgs>
	static inline void assertEquals(const T& expected, const T& actual,
			const TArgs& ... args) {
		assert(expected == actual, "assertEqual failed: expected=", expected,
				", actual=", actual, ", message: ", args...);
	}

	template<typename ... TArgs>
	static inline void check(bool cond, const TArgs& ... args) {
		if (!cond) {
			raise(args...);
		}
	}

private:

	static void _backtrace(std::ostream& os);

	static inline void _raise(std::ostream&) {
	}

	template<typename TArg, typename ... TArgs>
	static inline void _raise(std::ostream& os, const TArg& arg,
			const TArgs& ... args) {
		os << arg;
		_raise(os, args...);
	}

};

template<typename ... TArgs>
void Error::raise(const TArgs& ... args) {
	std::stringstream message;
	_raise(message, args...);

	std::stringstream stackTrace;
	_backtrace(stackTrace);

	throw JnifException(message.str(), stackTrace.str());
}

}

#endif
