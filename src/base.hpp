/*
 * base.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */
#ifndef JNIF_BASE_HPP
#define JNIF_BASE_HPP

#include <string>
#include <ostream>

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

/**
 * We use STL string.
 */
typedef std::string String;

/**
 * Represents the base exception that jnif can throw.
 */
class JnifException {
public:

	/**
	 * Creates an exception given the message and the stack trace.
	 *
	 * @param message contains information about exceptional situation.
	 * @param stackTrace the stack trace where this exception happened.
	 */
	JnifException(const String& message, const String& stackTrace = "") :
			_message(message), _stackTrace(stackTrace) {
	}

//	JnifException() {
//	}
//
//	JnifException(JnifException&& ex) :
//			_message(std::move(ex._message)) {
//	}

	/**
	 * Returns information about the exceptional situation.
	 */
	const String& message() const {
		return _message;
	}

	const String& stackTrace() const {
		return _stackTrace;
	}

	/**
	 * the stack trace where this exception happened.
	 */

//	template<typename T>
//	JnifException& operator<<(const T& arg) {
//		_message << arg;
//		return *this;
//	}
	/**
	 * Shows this.
	 */
	friend std::ostream& operator<<(std::ostream& os, const JnifException& ex);

private:

	String _message;

	String _stackTrace;

	//std::stringstream _message;

};

/**
 *
 */
class ParserException: public JnifException {
public:
};

class WriterException: public JnifException {
public:

	WriterException(const String& message, const String& stackTrace) :
			JnifException(message, stackTrace) {
	}
};

class InvalidMethodLengthException: public WriterException {
public:
	InvalidMethodLengthException(const String& message,
			const String& stackTrace) :
			WriterException(message, stackTrace) {
	}
};



/**
 * Access flags used by fields.
 */
enum FieldFlags {

	/**
	 * Declared public; may be accessed from outside its package.
	 */
	FIELD_PUBLIC = 0x0001,

	/**
	 * Declared private; usable only within the defining class.
	 */
	FIELD_PRIVATE = 0x0002,

	/**
	 * Declared protected; may be accessed within subclasses.
	 */
	FIELD_PROTECTED = 0x0004,

	/**
	 * Declared static.
	 */
	FIELD_STATIC = 0x0008,
	/**
	 * Declared final;
	 * never directly assigned to after object construction (JLS $17.5).
	 */
	FIELD_FINAL = 0x0010,

	/**
	 * Declared volatile; cannot be cached.
	 */
	FIELD_VOLATILE = 0x0040,

	/**
	 * Declared transient; not written or read by a persistent object manager.
	 */
	FIELD_TRANSIENT = 0x0080,

	/**
	 * Declared synthetic; not present in the source code.
	 */
	FIELD_SYNTHETIC = 0x1000,

	/**
	 * Declared as an element of an enum.
	 */
	FIELD_ENUM = 0x4000
};

/**
 *
 */
enum NewArrayType {
	NEWARRAYTYPE_BOOLEAN = 4,
	NEWARRAYTYPE_CHAR = 5,
	NEWARRAYTYPE_FLOAT = 6,
	NEWARRAYTYPE_DOUBLE = 7,
	NEWARRAYTYPE_BYTE = 8,
	NEWARRAYTYPE_SHORT = 9,
	NEWARRAYTYPE_INT = 10,
	NEWARRAYTYPE_LONG = 11
};

/**
 *
 */
enum AttrKind {
	ATTR_UNKNOWN,
	ATTR_SOURCEFILE,
	ATTR_SIGNATURE,
	ATTR_CODE,
	ATTR_EXCEPTIONS,
	ATTR_LVT,
	ATTR_LVTT,
	ATTR_LNT,
	ATTR_SMT
};

}

#endif
