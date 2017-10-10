//
// Created by Luigi on 07.10.17.
//

#ifndef JNIF_ICLASSPATH_HPP
#define JNIF_ICLASSPATH_HPP

#include <string>

namespace jnif::model {

    using std::string;

    class IClassPath {
    public:

        virtual ~IClassPath() {
        }

        virtual string getCommonSuperClass(const string& className1,
                                                const string& className2) = 0;

    };

}

#endif //JNIF_ICLASSPATH_HPP
