/*
 * Arena.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_ARENA_HPP
#define JNIF_ARENA_HPP

namespace jnif {

    class Arena {
    public:

        Arena();
        ~Arena();

        // Arena(const Arena&) = delete;
        // Arena(Arena&) = delete;
        // Arena(const Arena&&) = delete;

        void* alloc(int size);

        template<typename T, typename ... TArgs>
        T* create(const TArgs& ... args) {
            void* buf = alloc(sizeof(T));
            return new (buf) T(args ...);
        }

        template<typename T>
        T* newArray(int size) {
            void* buf = alloc(sizeof(T) * size);
            return new (buf) T[size];
        }

    private:

        class Block;

        Block* _head;

    };

}

#endif
