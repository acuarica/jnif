#ifndef JNIF_MEMBERS_HPP
#define JNIF_MEMBERS_HPP

#include <vector>

#include "../base.hpp"

namespace jnif {

/**
 * Represents a collection of members within a class file, i.e.,
 * fields or methods.
 */
class Members {

	friend class ClassFile;
	Members(const Members&) = delete;

public:

	inline Member& add(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		Member member(accessFlags, nameIndex, descIndex);
		members.push_back(std::move(member));

		return members.back();
	}

	inline u2 size() const {
		return members.size();
	}

	inline Member& operator[](u2 index) {
		return members[index];
	}

private:

	inline Members() {
	}

	std::vector<Member> members;
};

}

#endif
