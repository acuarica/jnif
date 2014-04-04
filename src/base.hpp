/*
 * base.hpp
 *
 *  Created on: Apr 3, 2014
 *      Author: luigi
 */

#ifndef JNIF_BASE_HPP
#define JNIF_BASE_HPP

namespace jnif {

/**
 * Represents a byte inside the Java Class File.
 * The sizeof(u1) must be equal to 1.
 */
typedef unsigned char u1;

/**
 * Represents two bytes inside the Java Class File.
 * The sizeof(u2) must be equal to 2.
 */
typedef unsigned short u2;

/**
 * Represents four bytes inside the Java Class File.
 * The sizeof(u4) must be equal to 4.
 */
typedef unsigned int u4;

}

#endif
