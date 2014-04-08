/*
 * Frame.hpp
 *
 *  Created on: Apr 7, 2014
 *      Author: luigi
 */

#ifndef JNIF_FRAME_HPP
#define JNIF_FRAME_HPP

namespace jnif {

struct H: private ErrorManager {
	enum T {
		Top, Int, Long, Float, Double, Ref
	};

	friend std::ostream& operator<<(std::ostream& os, H::T t) {
		switch (t) {
			case H::Top:
				os << "Top";
				break;
			case H::Int:
				os << "Int";
				break;
			case H::Long:
				os << "Long";
				break;
			case H::Float:
				os << "Float";
				break;
			case H::Double:
				os << "Double";
				break;
			case H::Ref:
				os << "Ref";
				break;
		}

		return os;
	}

	H() :
			valid(false) {
	}

	T pop() {
		check(stack.size() > 0, "Trying to pop in an empty stack.");

		T t = stack.front();
		stack.pop_front();
		return t;
	}
	void push(const T& t) {
		stack.push_front(t);
	}
	void pushInt() {
		push(Int);
	}
	void pushLong() {
		push(Top);
		push(Long);
	}
	void pushFloat() {
		push(Float);
	}
	void pushDouble() {
		push(Top);
		push(Double);
	}
	void pushRef() {
		push(Ref);
	}
	void pushNull() {
		push(Ref);
	}
	void setVar(u4 lvindex, T t) {
		check(lvindex < 256, "");

		if (lvindex >= lva.size()) {
			lva.resize(lvindex + 1, Top);
		}

		lva[lvindex] = t;
	}

	void setIntVar(int lvindex) {
		setVar(lvindex, Int);
	}
	void setLongVar(int lvindex) {
		setVar(lvindex, Long);
	}
	void setFloatVar(int lvindex) {
		setVar(lvindex, Float);
	}
	void setDoubleVar(int lvindex) {
		setVar(lvindex, Double);
	}
	void setRefVar(int lvindex) {
		setVar(lvindex, Ref);
	}

	std::ostream& print(std::ostream& os) {
		os << "{ ";
		for (u4 i = 0; i < lva.size(); i++) {
			os << (i == 0 ? "" : ", ") << i << ": " << lva[i];
		}
		os << " } [ ";
		int i = 0;
		for (auto t : stack) {
			os << (i == 0 ? "" : " | ") << t;
			i++;
		}
		os << " ]" << std::endl;

		return os;
	}

	static bool isAssignable(T subt, T supt) {
		if (subt == supt) {
			return true;
		}

		if (supt == Top) {
			return true;
		}

		return false;
	}

	static bool assign(T& t, T o) {
		check(isAssignable(t, o) || isAssignable(o, t), "");

		if (isAssignable(t, o)) {
			if (t == o) {
				return false;
			}

			t = o;
			return true;
		}

		assert(isAssignable(o, t), "");

		return false;
	}

	bool join(H& how, std::ostream& os) {
		this->print(os);
		how.print(os);

		check(stack.size() == how.stack.size(), "Different stack sizes: ",
				stack.size(), " != ", how.stack.size());

		if (lva.size() < how.lva.size()) {
			lva.resize(how.lva.size());
		} else if (how.lva.size() < lva.size()) {
			how.lva.resize(lva.size());
		}

		assert(lva.size() == how.lva.size(), "%ld != %ld", lva.size(),
				how.lva.size());

		bool change = false;

		for (u4 i = 0; i < lva.size(); i++) {
			assign(lva[i], how.lva[i]);
		}

		std::list<T>::iterator i = stack.begin();
		std::list<T>::iterator j = how.stack.begin();

		for (; i != stack.end(); i++, j++) {
			assign(*i, *j);
		}

		return change;
	}

	std::vector<T> lva;
	std::list<T> stack;
	bool valid;
};

}

#endif
