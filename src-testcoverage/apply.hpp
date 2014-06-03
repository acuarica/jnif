/*
 * apply.hpp
 *
 *  Created on: Jun 2, 2014
 *      Author: luigi
 */

#ifndef APPLY_HPP
#define APPLY_HPP

#include <jnif.hpp>

static inline int _exception(int) __attribute__((noreturn));

static inline int _exception(int) {
	exit(1);
}

#define ASSERT(cond, format, ...) ( (cond) ? 0 : _exception(fprintf(stderr, \
			"ASSERT | '" #cond "' failed | " format "\n", ##__VA_ARGS__ )))

class JavaFile {
public:

	const jnif::u1* const data;
	const int len;
	const jnif::String name;

};

typedef void (TestFunc)(const JavaFile& jf, jnif::ClassFile& cf);

void apply(std::ostream& os, const std::list<JavaFile>& tests, TestFunc instr);

#endif
