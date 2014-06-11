#ifndef JNIF_HPP
#define JNIF_HPP

#include <string>
#include <vector>
#include <list>
#include <ostream>
#include <map>
#include <sstream>

#include "base.hpp"
#include "Error.hpp"
#include "ConstPool.hpp"
#include "Version.hpp"
#include "ClassFile.hpp"
#include "ClassHierarchy.hpp"

/**
 * The jnif namespace contains all type definitions, constants, enumerations
 * and structs of the jnif framework.
 *
 * This implementation is based on Chapter 4 (The class File Format) and
 * Chapter 6 (Chapter 6. The Java Virtual Machine Instruction Set) of the
 * Java Virtual Machine Specification version 7. Portions of this
 * documentation are taken from this specification.
 *
 * For more information refer to:
 *
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 *
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html
 *
 * @see ClassFile
 *
 */
namespace jnif {


/**
 *
 */
//class IClassFinder {
//public:
//
//	virtual ~IClassFinder() {
//	}
//
//	virtual ClassFile* findClass(const String& className) = 0;
//
//};

}

#endif
