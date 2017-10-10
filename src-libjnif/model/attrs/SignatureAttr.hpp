//
// Created by Luigi on 07.10.17.
//

#ifndef JNIF_SIGNATUREATTR_HPP
#define JNIF_SIGNATUREATTR_HPP

#include "Attr.hpp"

namespace jnif::model {

    class SignatureAttr: public Attr {
    public:

        const ConstPool::Index signatureIndex;

        SignatureAttr(ConstPool::Index nameIndex, ConstPool::Index signatureIndex, ClassFile* constPool) :
                Attr(ATTR_SIGNATURE, nameIndex, 2, constPool), signatureIndex(signatureIndex) {
        }

        const char* signature() const;

    };

}

#endif //JNIF_SIGNATUREATTR_HPP
