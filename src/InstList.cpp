/*
 * InstList.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */
#include "jnif.hpp"

namespace jnif {

Inst* InstList::Iterator::operator*() {
	Error::assert(position != NULL, "Dereferencing * on NULL");
	return position;
}

Inst* InstList::Iterator::operator->() const {
	Error::assert(position != NULL, "Dereferencing -> on NULL");

	return position;
}

InstList::Iterator& InstList::Iterator::operator++() {
	Error::assert(position != NULL, "Doing ++ at NULL");
	position = position->next;

	return *this;
}

InstList::Iterator& InstList::Iterator::operator--() {
	if (position == NULL) {
		position = last;
	} else {
		position = position->prev;
	}

	Error::assert(position != NULL, "Doing -- at NULL after last");

	return *this;
}

InstList::~InstList() {
	for (Inst* inst = first; inst != NULL;) {
		Inst* next = inst->next;
		if (!inst->isLabel()) {
			delete inst;
		}
		inst = next;
	}

	for (LabelInst* inst : _labelPool) {
		delete inst;
	}
}

void InstList::addInst(Inst* inst, Inst* pos) {
	Error::assert((first == NULL) == (last == NULL),
			"Invalid head/tail/size: head: ", first, ", tail: ", last,
			", size: ", _size);

	Error::assert((first == NULL) == (_size == 0),
			"Invalid head/tail/size: head: ", first, ", tail: ", last,
			", size: ", _size);

	Inst* p;
	Inst* n;
	if (first == NULL) {
		Error::assert(pos == NULL, "Invalid pos");

		p = NULL;
		n = NULL;
		//first = inst;
		//last = inst;
	} else {
		if (pos == NULL) {
			p = last;
			n = NULL;
			//last = inst;
		} else {
			p = pos->prev;
			n = pos;
		}
	}

	inst->prev = p;
	inst->next = n;

	if (inst->prev != NULL) {
		inst->prev->next = inst;
	} else {
		first = inst;
	}

	if (inst->next != NULL) {
		inst->next->prev = inst;
	} else {
		last = inst;
	}

	_size++;
}

}

