/*
 * SizeWriter.hpp
 *
 *  Created on: Apr 4, 2014
 *      Author: luigi
 */

#ifndef JNIF_SIZEWRITER_HPP
#define JNIF_SIZEWRITER_HPP

namespace jnif {

/**
 *
 */
class SizeWriter {
public:

	SizeWriter() :
			offset(0) {
	}

	void writeu1(u1) {
		offset += 1;
	}

	void writeu2(u2) {
		offset += 2;
	}

	void writeu4(u4) {
		offset += 4;
	}

	void writecount(const void*, int count) {
		offset += count;
	}

	int getOffset() const {
		return offset;
	}

private:
	int offset;
};

}

#endif
