/*
 * BufferReader.hpp
 *
 *  Created on: Jun 18, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_BUFFERREADER_HPP
#define JNIF_PARSER_BUFFERREADER_HPP

#include "../Error.hpp"

namespace jnif {

/**
 * Implements a memory buffer reader in big-endian encoding.
 */
class BufferReader {
public:

	/**
	 * Constructs a BufferReader from a memory buffer and its size.
	 * The buffer must be an accessible and readable memory location
	 * at least of the specified size.
	 *
	 * @param buffer The memory buffer to read from.
	 * @param size The size of the buffer in bytes.
	 *
	 */
	BufferReader(const u1* buffer, u4 size) :
			buffer(buffer), _size(size), off(0) {
	}

	/**
	 * When this buffer reader finishes, it will check whether the end has
	 * been reached, i.e., all bytes from buffer were read or skipped.
	 * In other words, when the buffer reader br is destroyed, the condition
	 *
	 * @f[ br.offset() = br.size() @f]
	 *
	 * must hold.
	 */
	~BufferReader() {
		//Error::check(off == _size, "Expected end of buffer");
	}

	int size() const {
		return _size;
	}

	u1 readu1() {
		Error::check(off + 1 <= _size, "Invalid read");

		u1 result = buffer[off];

		off += 1;

		return result;
	}

	u2 readu2() {
		Error::check(off + 2 <= _size, "Invalid read 2");

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];

		u2 result = r0 << 8 | r1;

		off += 2;

		return result;
	}

	u4 readu4() {
		Error::check(off + 4 <= _size, "Invalid read 4");
		//if (off >= 256 ) Error::raise()

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];
		u1 r2 = buffer[off + 2];
		u1 r3 = buffer[off + 3];

		u4 result = r0 << 24 | r1 << 16 | r2 << 8 | r3;

		off += 4;

		return result;
	}

	void skip(int count) {
		const char* const m = "Invalid read: %d (offset: %d)";
		Error::check(off + count <= _size, m, count, off);

		off += count;
	}

	int offset() const {
		return off;
	}

	const u1* pos() const {
		return buffer + off;
	}

	bool eor() const {
		return off == _size;
	}

private:

	const u1 * const buffer;
	const int _size;
	int off;
};

}

#endif
