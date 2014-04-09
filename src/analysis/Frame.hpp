/*
 * Frame.hpp
 *
 *  Created on: Apr 7, 2014
 *      Author: luigi
 */

#ifndef JNIF_FRAME_HPP
#define JNIF_FRAME_HPP

namespace jnif {

std::ostream& operator<<(std::ostream& os, const Type& t) {
	switch (t.tag) {
		case Type::TYPE_TOP:
			os << "Top";
			break;
		case Type::TYPE_INTEGER:
			os << "Int";
			break;
		case Type::TYPE_LONG:
			os << "Long";
			break;
		case Type::TYPE_FLOAT:
			os << "Float";
			break;
		case Type::TYPE_DOUBLE:
			os << "Double";
			break;
		case Type::TYPE_OBJECT:
			os << "Ref";
			break;
		default:
			os << "UNKNOWN TYPE!!!";
	}

	return os;
}

class H: private ErrorManager {
public:

	enum TEnum {
		Top = Type::TYPE_TOP,
		Int = Type::TYPE_INTEGER,
		Long = Type::TYPE_LONG,
		Float = Type::TYPE_FLOAT,
		Double = Type::TYPE_DOUBLE,
		Ref = Type::TYPE_OBJECT
	};

	typedef Type T;

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
		push(Type::intt());
	}
	void pushLong() {
		push(Type::top());
		push(Type::longt());
	}
	void pushFloat() {
		push(Type::floatt());
	}
	void pushDouble() {
		push(Type::top());
		push(Type::doublet());
	}
	void pushRef() {
		push(Type::objectt(-25));
	}
	void pushNull() {
		push(Type::objectt(-26));
	}
	void setVar(u4 lvindex, T t) {
		check(lvindex < 256, "");

		if (lvindex >= lva.size()) {
			lva.resize(lvindex + 1, Type::top());
		}

		lva[lvindex] = t;
	}

	void setIntVar(int lvindex) {
		setVar(lvindex, Type::intt());
	}
	void setLongVar(int lvindex) {
		setVar(lvindex, Type::longt());
	}
	void setFloatVar(int lvindex) {
		setVar(lvindex, Type::floatt());
	}
	void setDoubleVar(int lvindex) {
		setVar(lvindex, Type::doublet());
	}
	void setRefVar(int lvindex) {
		setVar(lvindex, Type::objectt(-27));
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

		if (supt.isTop()) {
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
			lva.resize(how.lva.size(), Type::top());
		} else if (how.lva.size() < lva.size()) {
			how.lva.resize(lva.size(), Type::top());
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
