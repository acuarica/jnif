#ifndef JNIF_BASE_HPP
#define JNIF_BASE_HPP

/**
 * The jnif namespace contains all type definitions, constants, enumerations
 * and classes of the jnif framework.
 *
 * The most relevant types and classes of the framework are ClassParser
 * (using ClassBaseParser) and
 * ClassWriterVisitor which it allows to parser Java Class File and write back
 * them with the ability to inject transformers (class visitors in jnif
 * parlance) to modify these classes.
 */
namespace jnif {

using namespace std;

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

/**
 * Defines the Java Class File signature. This magic value must be binary
 * serialized as a u4 value.
 */
enum Magic {

	/**
	 * The only allowed value for the signature.
	 */
	CLASSFILE_MAGIC = 0xcafebabe
};

}

#endif
