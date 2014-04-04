/*
 * BufferWriter.hpp
 *
 *  Created on: Apr 4, 2014
 *      Author: luigi
 */

#ifndef JNIF_BUFFERWRITER_HPP
#define JNIF_BUFFERWRITER_HPP

namespace jnif {

/**
 * Implements a memory buffer writer in big-endian encoding.
 */
template<typename TErrorManager>
class BufferWriter: private TErrorManager {
	using TErrorManager::check;
	using TErrorManager::assert;
public:

	BufferWriter(u1* buffer, int len) :
			buffer(buffer), len(len), offset(0) {
	}

	~BufferWriter() {
		check(offset == len, "%d != %d. Expected end of buffer", offset, len);
	}

	void writeu1(u1 value) {
		ASSERT(offset + 1 <= len, "Invalid write");

		buffer[offset] = value;

		offset += 1;
	}

	void writeu2(u2 value) {
		ASSERT(offset + 2 <= len, "Invalid write");

		buffer[offset + 0] = ((u1*) &value)[1];
		buffer[offset + 1] = ((u1*) &value)[0];

		offset += 2;
	}

	void writeu4(u4 value) {
		check(offset + 4 <= len, "Invalid write");

		buffer[offset + 0] = ((u1*) &value)[3];
		buffer[offset + 1] = ((u1*) &value)[2];
		buffer[offset + 2] = ((u1*) &value)[1];
		buffer[offset + 3] = ((u1*) &value)[0];

		offset += 4;
	}

	void writecount(const void* source, int count) {
		check(offset + count <= len, "Invalid write count");

		copy((u1*) source, (u1*) source + count, buffer + offset);

		offset += count;
	}

	int getOffset() const {
		return offset;
	}

private:

	u1* const buffer;
	const int len;
	int offset;
};

}

#endif
