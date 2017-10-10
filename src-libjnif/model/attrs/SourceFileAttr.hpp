#ifndef JNIF_MODEL_ATTRS_SOURCEFILEATTR_HPP
#define JNIF_MODEL_ATTRS_SOURCEFILEATTR_HPP

#include "../ConstPool.hpp"
#include "Attr.hpp"

namespace jnif::model {

    class SourceFileAttr : public Attr {
    public:

        const ConstPool::Index sourceFileIndex;

        SourceFileAttr(ConstPool::Index nameIndex, ConstPool::Index sourceFileIndex,
                       ClassFile* constPool) :
                Attr(ATTR_SOURCEFILE, nameIndex, 2, constPool), sourceFileIndex(sourceFileIndex) {
        }

        const char* sourceFile() const {
            return constPool->getUtf8(sourceFileIndex);
        }

    };

}

#endif
