#ifndef JNIF_MEMBER_HPP
#define JNIF_MEMBER_HPP

#include "../base.hpp"

namespace jnif {

/**
 *
 */
class Member {
	friend class Members;
	Member(const Member&) = delete;

public:
	Member(Member&&) = default;

	u2 accessFlags;
	u2 nameIndex;
	u2 descIndex;
	Attrs attrs;

private:

	inline Member(u2 accessFlags, u2 nameIndex, u2 descIndex) :
			accessFlags(accessFlags), nameIndex(nameIndex), descIndex(descIndex) {
	}
};

}

#endif
