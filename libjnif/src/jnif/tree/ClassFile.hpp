#ifndef JNIF_CLASSFILE_HPP
#define JNIF_CLASSFILE_HPP

#include "../base.hpp"
#include <vector>

namespace jnif {

/**
 * Represents a Java Class File following version 7.
 *
 * Definitions taken from Chapter 4. The class File Format from the
 * Java Virtual Machine Specification
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 */
class ClassFile {
public:
	u4 magic;
	u2 minor;
	u2 major;
	ConstPool cp;
	u2 accessFlags;
	u2 thisClassIndex;
	u2 superClassIndex;
	vector<u2> interfaces;
	Members fields;
	Members methods;
	Attrs attrs;
};

}

#endif
