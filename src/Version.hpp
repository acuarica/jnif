/*
 * Version.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_VERSION_HPP
#define JNIF_VERSION_HPP

#include "base.hpp"

namespace jnif {

/**
 * The Version class is a tuple containing a major and minor
 * version numbers fields.
 *
 * The values of the major and minor fields are the minor
 * and major version numbers of this class file.
 * Together, a major and a minor version number determine the version of the
 * class file format.
 * If a class file has major version number M and minor version number m,
 * we denote the version of its class file format as M.m.
 * Thus, class file format versions may be ordered lexicographically,
 * for example, 1.5 < 2.0 < 2.1.
 *
 * A Java Virtual Machine implementation can support a class file format of
 * version v if and only if v lies in some contiguous range Mi.0 <= v <= Mj.m.
 * The release level of the Java SE platform to which a Java Virtual Machine
 * implementation conforms is responsible for determining the range.
 */
class Version {
public:

	/**
	 * Using default 51, which is supported by JDK 1.7.
	 */
	Version(u2 majorVersion = 51, u2 minorVersion = 0) :
			_major(majorVersion), _minor(minorVersion) {
	}

	/**
	 * The major version number.
	 */
	u2 majorVersion() const;

	/**
	 * The minor version number.
	 */
	u2 minorVersion() const;

	/**
	 * Taken from the oficial JVM specification.
	 *
	 * Oracle's Java Virtual Machine implementation in JDK release 1.0.2
	 * supports class file format versions 45.0 through 45.3 inclusive.
	 * JDK releases 1.1.* support class file format versions in the
	 * range 45.0 through 45.65535 inclusive.
	 * For k >= 2, JDK release 1.k supports class file format versions in
	 * the range 45.0 through 44+k.0 inclusive.
	 */
	String supportedByJdk() const;

	/**
	 * Equals comparator.
	 */
	friend bool operator==(const Version& lhs, const Version& rhs);

	/**
	 * Less comparator.
	 */
	friend bool operator<(const Version& lhs, const Version& rhs);

	/**
	 * Less or equal comparator.
	 */
	friend bool operator<=(const Version& lhs, const Version& rha);

	friend std::ostream& operator<<(std::ostream& os, const Version& version);

private:

	u2 _major;
	u2 _minor;

};

}

#endif
