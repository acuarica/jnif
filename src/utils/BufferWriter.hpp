#ifndef JNIF_BUFFERWRITER_HPP
#define JNIF_BUFFERWRITER_HPP

#include "../base.hpp"
#include <algorithm>

namespace jnif {

class BufferWriter {
public:
	inline BufferWriter(u1* buffer, int len) :
			buffer(buffer), len(len), offset(0) {
	}

	~BufferWriter() {
		end();
	}

	inline void writeu1(u1 value) {
		ASSERT(offset + 1 <= len, "Invalid write");

		buffer[offset] = value;

		offset += 1;
	}

	inline void writeu2(u2 value) {
		ASSERT(offset + 2 <= len, "Invalid write");

		buffer[offset + 0] = ((u1*) &value)[1];
		buffer[offset + 1] = ((u1*) &value)[0];

		offset += 2;
	}

	inline void writeu4(u4 value) {
		ASSERT(offset + 4 <= len, "Invalid write");

		buffer[offset + 0] = ((u1*) &value)[3];
		buffer[offset + 1] = ((u1*) &value)[2];
		buffer[offset + 2] = ((u1*) &value)[1];
		buffer[offset + 3] = ((u1*) &value)[0];

		offset += 4;
	}

	inline void writecount(const void* source, int count) {
		ASSERT(offset + count <= len, "Invalid write count");

		std::copy((u1*) source, (u1*) source + count, buffer + offset);

		offset += count;
	}

	inline int offset2() {
		return offset;
	}

	inline u1* pos() {
		return buffer + offset;
	}

	inline void skip(int count) {
		ASSERT(offset + count <= len, "Invalid skip count");

		offset += count;
	}

private:

	inline void end() {
		ASSERT(offset == len,
				"%d != %d. End of buffer writer not reached while expecting end of buffer",
				offset, len);
	}

	u1* const buffer;
	const int len;
	int offset;

};

}

#endif
