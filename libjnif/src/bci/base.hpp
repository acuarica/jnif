#ifndef __BCI__BASE_HPP__
#define	__BCI__BASE_HPP__

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <utility>

using namespace std;

#define JNIFNS jnif

namespace JNIFNS {

/**
 *
 * Definitions taken from Chapter 4. The class File Format from the
 * Java Virtual Machine Specification
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 *
 */

/**
 * Base data types defined by the java class specification.
 */

typedef unsigned char u1;

typedef unsigned short u2;

typedef unsigned int u4;

}

#endif
