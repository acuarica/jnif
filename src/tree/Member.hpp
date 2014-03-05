#ifndef JNIF_MEMBER_HPP
#define JNIF_MEMBER_HPP

#include "../base.hpp"

namespace jnif {

/**
 *
 */
struct Member: Attrs {
	friend struct Members;
	Member(const Member&) = delete;

	Member(Member&&) = default;

	u2 accessFlags;
	u2 nameIndex;
	u2 descIndex;

private:

	Member(u2 accessFlags, u2 nameIndex, u2 descIndex) :
			accessFlags(accessFlags), nameIndex(nameIndex), descIndex(descIndex) {
	}
};

}

#endif
