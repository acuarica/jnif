/*
 * Arena.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */
#include "Arena.hpp"
#include "../Error.hpp"
#include <stdlib.h>
#include <stdio.h>

constexpr int BLOCK_SIZE = 1024 * 1024;

namespace jnif {

    class Arena::Block {
    public:

        Block(Block *next) :
                _next(next), _buffer(malloc(BLOCK_SIZE)), _position(0) {
            JnifError::check(_buffer != nullptr, "Block alloc is NULL");
        }

        ~Block() {
            JnifError::trace("Block::~Block");
            free(_buffer);
        }

        void *alloc(int size) {
            JnifError::assert(size <= BLOCK_SIZE, "Size too large for a block: ", size);

            if (_position + size <= BLOCK_SIZE) {
                void *offset = (char *) _buffer + _position;
                _position += size;
                return offset;
            }

            return nullptr;
        }

        Block *_next;
        void *_buffer;
        int _position;
    };

    Arena::Arena() :
            _head(new Block(nullptr)) {
    }

    Arena::~Arena() {
        for (Block *block = _head; block != nullptr;) {
            Block *next = block->_next;
            delete block;
            block = next;
        }
    }

    void *Arena::alloc(int size) {
        void *res = _head->alloc(size);
        if (res == NULL) {
            _head = new Block(_head);
            res = _head->alloc(size);
        }

        JnifError::assert(res != NULL, "alloc == NULL");

        return res;
    }

}
