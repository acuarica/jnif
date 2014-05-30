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

}

#endif
