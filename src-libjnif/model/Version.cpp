/*
 * Version.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */
#include "Version.hpp"
#include <sstream>

namespace jnif::model {

    using std::stringstream;

    Version::Version(u2 majorVersion, u2 minorVersion) : _major(majorVersion), _minor(minorVersion) {
    }

    u2 Version::majorVersion() const {
        return _major;
    }

    u2 Version::minorVersion() const {
        return _minor;
    }

    std::string Version::supportedByJdk() const {
        if (Version(45, 3) <= *this && *this < Version(45, 0)) {
            return "1.0.2";
        } else if (Version(45, 0) <= *this && *this <= Version(45, 65535)) {
            return "1.1.*";
        } else {
            u2 k = _major - 44;
            stringstream ss;
            ss << "1." << k;
            return ss.str();
        }
    }

    bool operator==(const Version& lhs, const Version& rhs) {
        return lhs._major == rhs._major && lhs._major == rhs._major;
    }

    bool operator<(const Version& lhs, const Version& rhs) {
        return lhs._major < rhs._major
               || (lhs._major == rhs._major && lhs._minor < rhs._minor);
    }

    bool operator<=(const Version& lhs, const Version& rha) {
        return lhs < rha || lhs == rha;
    }

}
