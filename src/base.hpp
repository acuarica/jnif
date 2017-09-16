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
            message(message), stackTrace(stackTrace) {
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
        String message;

        /**
         * the stack trace where this exception happened.
         */

        String stackTrace;


//	template<typename T>
//	JnifException& operator<<(const T& arg) {
//		_message << arg;
//		return *this;
//	}
        /**
         * Shows this.
         */
        friend std::ostream& operator<<(std::ostream& os, const JnifException& ex);

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
