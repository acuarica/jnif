//
// Created by Luigi on 09.10.17.
//

#include "ClassFile.hpp"
#include "../Error.hpp"

namespace jnif::model {

    Member::Member(u2 accessFlags, ConstPool::Index nameIndex, ConstPool::Index descIndex, const ConstPool& constPool) :
            accessFlags(accessFlags),
            nameIndex(nameIndex),
            descIndex(descIndex),
            constPool(constPool),
            sig(&attrs) {
        JnifError::check(constPool.isUtf8(nameIndex));
        JnifError::check(constPool.isUtf8(descIndex));
    }

}