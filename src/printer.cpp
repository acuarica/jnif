#include "jnif.hpp"
#include "jniferr.hpp"

#include <ostream>
#include <iomanip>

#include <map>
#include <set>

#include "Cfg.hpp"

namespace jnif {

/**
 * OPCODES names definition
 *
 */
struct AccessFlagsPrinter {
	AccessFlagsPrinter(u2 value, const char* sep = " ") :
			value(value), sep(sep) {
	}

	friend ostream& operator<<(ostream& out, AccessFlagsPrinter self) {
		bool empty = true;

		auto check = [&](AccessFlags accessFlags, const char* name) {
			if (self.value & accessFlags) {
				out << (empty ? "" : self.sep) << name;
				empty = false;
			}
		};

		check(ACC_PUBLIC, "public");
		check(ACC_PRIVATE, "private");
		check(ACC_PROTECTED, "protected");
		check(ACC_STATIC, "static");
		check(ACC_FINAL, "final");
		check(ACC_SYNCHRONIZED, "synchronized");
		check(ACC_BRIDGE, "bridge");
		check(ACC_VARARGS, "varargs");
		check(ACC_NATIVE, "native");
		check(ACC_ABSTRACT, "abstract");
		check(ACC_STRICT, "strict");
		check(ACC_SYNTHETIC, "synthetic");

		return out;
	}

private:
	const u2 value;
	const char* const sep;
};

template<typename THandler>
static void parseFieldDesc(const char*& fieldDesc, THandler& h) {
	const char* originalFieldDesc = fieldDesc;

	int ds = 0;
	while (*fieldDesc == '[') {
		CHECK(*fieldDesc != '\0',
				"Reach end of string while searching for array. Field descriptor: \"%s\"",
				originalFieldDesc);
		fieldDesc++;
		ds++;
	}

	if (ds > 0) {
		h.arrayType(ds);
	}

	CHECK(*fieldDesc != '\0', "");

	switch (*fieldDesc) {
		case 'Z':
		case 'B':
		case 'C':
		case 'S':
		case 'I':
			h.intType();
			break;
		case 'D':
			h.doubleType();
			break;
		case 'F':
			h.floatType();
			break;
		case 'J':
			h.longType();
			break;
		case 'L': {
			fieldDesc++;

			//const char* className = fieldDesc;
			int len = 0;
			while (*fieldDesc != ';') {
				CHECK(*fieldDesc != '\0', "");
				fieldDesc++;
				len++;
			}
			h.refType();

			break;
		}
		default:
			EXCEPTION("Invalid field desc %s", originalFieldDesc);
	}

	fieldDesc++;
}

template<typename THandler>
static void parseMethodDesc(const char* methodDesc, THandler& h, int lvstart) {
	const char* originalMethodDesc = methodDesc;

	CHECK(*methodDesc == '(', "Invalid beginning of method descriptor: %s",
			originalMethodDesc);
	methodDesc++;

	int lvindex = lvstart;
	while (*methodDesc != ')') {
		CHECK(*methodDesc != '\0', "Reached end of string: %s",
				originalMethodDesc);

		struct ParseMethodAdapter {
			THandler& h;
			int lvindex;
			int dims;
			ParseMethodAdapter(THandler& h, int lvindex) :
					h(h), lvindex(lvindex), dims(0) {
			}
			void intType() {
				if (this->dims == 0) {
					this->h.setIntVar(this->lvindex);
				}
			}
			void longType() {
				if (this->dims == 0) {
					this->h.setLongVar(this->lvindex);
				}
			}
			void floatType() {
				if (this->dims == 0) {
					this->h.setFloatVar(this->lvindex);
				}
			}
			void doubleType() {
				if (this->dims == 0) {
					this->h.setDoubleVar(this->lvindex);
				}
			}
			void refType() {
				if (this->dims == 0) {
					this->h.setRefVar(this->lvindex);
				}
			}
			void arrayType(int ds) {
				this->dims = ds;
				this->h.setRefVar(this->lvindex);
			}
		} a(h, lvindex);

		parseFieldDesc(methodDesc, a);
		lvindex++;
	}

	CHECK(*methodDesc == ')', "Expected ')' in method descriptor: %s",
			originalMethodDesc);
	methodDesc++;

	CHECK(*methodDesc != '\0', "Reached end of string: \"%s\"",
			originalMethodDesc);

	if (*methodDesc == 'V') {
		methodDesc++;
		//h.typeVoid();
	} else {
		struct {
			void intType() {
			}
			void longType() {
			}
			void floatType() {
			}
			void doubleType() {
			}
			void refType() {
			}
			void arrayType(int) {
			}
		} e;
		parseFieldDesc(methodDesc, e);
	}

	CHECK(*methodDesc == '\0', "Expected end of string: %s",
			originalMethodDesc);
}

template<typename TFrame>
static void computeFrame(Inst& inst, const ConstPool& cp, TFrame& h) {

	auto xaload = [&]() {
		h.pop();
		h.pop();
	};
	auto istore = [&](int lvindex) {
		h.pop();
		h.setIntVar(lvindex);
	};
	auto lstore = [&](int lvindex) {
		h.pop();
		h.setLongVar(lvindex);
	};
	auto fstore = [&](int lvindex) {
		h.pop();
		h.setFloatVar(lvindex);
	};
	auto dstore = [&](int lvindex) {
		h.pop();
		h.setDoubleVar(lvindex);
	};
	auto astore = [&](int lvindex) {
		h.pop();
		h.setRefVar(lvindex);
	};
	auto xastore = [&]() {
		h.pop();
		h.pop();
		h.pop();
	};

	switch (inst.opcode) {
		case OPCODE_nop:
			break;
		case OPCODE_aconst_null:
			h.pushNull();
			break;
		case OPCODE_iconst_m1:
		case OPCODE_iconst_0:
		case OPCODE_iconst_1:
		case OPCODE_iconst_2:
		case OPCODE_iconst_3:
		case OPCODE_iconst_4:
		case OPCODE_iconst_5:
		case OPCODE_bipush:
		case OPCODE_sipush:
			h.pushInt();
			break;
		case OPCODE_lconst_0:
		case OPCODE_lconst_1:
			h.pushLong();
			break;
		case OPCODE_fconst_0:
		case OPCODE_fconst_1:
		case OPCODE_fconst_2:
			h.pushFloat();
			break;
		case OPCODE_dconst_0:
		case OPCODE_dconst_1:
			h.pushDouble();
			break;
		case OPCODE_ldc:
		case OPCODE_ldc_w:
		case OPCODE_ldc2_w: {

//			cp.accept(2, [&](ConstPool::Class i) {
//				h.pushDouble();
//			}, [&](ConstPool::FieldRef i) {
//				h.pushLong();
//			});

			ConstTag tag = cp.getTag(inst.ldc.valueIndex);
			switch (tag) {
				case CONSTANT_Integer:
					h.pushInt();
					break;
				case CONSTANT_Float:
					h.pushFloat();
					break;
				case CONSTANT_Long:
					h.pushLong();
					break;
				case CONSTANT_Double:
					h.pushLong();
					break;
				default:
					h.pushRef();
					break;
			}
			break;
		}
		case OPCODE_iload:
		case OPCODE_iload_0:
		case OPCODE_iload_1:
		case OPCODE_iload_2:
		case OPCODE_iload_3:
			h.pushInt();
			break;
		case OPCODE_lload:
		case OPCODE_lload_0:
		case OPCODE_lload_1:
		case OPCODE_lload_2:
		case OPCODE_lload_3:
			h.pushLong();
			break;
		case OPCODE_fload:
		case OPCODE_fload_0:
		case OPCODE_fload_1:
		case OPCODE_fload_2:
		case OPCODE_fload_3:
			h.pushFloat();
			break;
		case OPCODE_dload:
		case OPCODE_dload_0:
		case OPCODE_dload_1:
		case OPCODE_dload_2:
		case OPCODE_dload_3:
			h.pushDouble();
			break;
		case OPCODE_aload:
		case OPCODE_aload_0:
		case OPCODE_aload_1:
		case OPCODE_aload_2:
		case OPCODE_aload_3:
			h.pushRef();
			break;
		case OPCODE_iaload:
		case OPCODE_baload:
		case OPCODE_caload:
		case OPCODE_saload:
			xaload();
			h.pushInt();
			break;
		case OPCODE_laload:
			xaload();
			h.pushLong();
			break;
		case OPCODE_faload:
			xaload();
			h.pushFloat();
			break;
		case OPCODE_daload:
			xaload();
			h.pushDouble();
			break;
		case OPCODE_aaload:
			xaload();
			h.pushRef();
			break;
		case OPCODE_istore:
			istore(inst.var.lvindex);
			break;
		case OPCODE_lstore:
			lstore(inst.var.lvindex);
			break;
		case OPCODE_fstore:
			fstore(inst.var.lvindex);
			break;
		case OPCODE_dstore:
			dstore(inst.var.lvindex);
			break;
		case OPCODE_astore:
			astore(inst.var.lvindex);
			break;
		case OPCODE_istore_0:
			istore(0);
			break;
		case OPCODE_istore_1:
			istore(1);
			break;
		case OPCODE_istore_2:
			istore(2);
			break;
		case OPCODE_istore_3:
			istore(3);
			break;
		case OPCODE_lstore_0:
			lstore(0);
			break;
		case OPCODE_lstore_1:
			lstore(1);
			break;
		case OPCODE_lstore_2:
			lstore(2);
			break;
		case OPCODE_lstore_3:
			lstore(3);
			break;
		case OPCODE_fstore_0:
			fstore(0);
			break;
		case OPCODE_fstore_1:
			fstore(1);
			break;
		case OPCODE_fstore_2:
			fstore(2);
			break;
		case OPCODE_fstore_3:
			fstore(3);
			break;
		case OPCODE_dstore_0:
			dstore(0);
			break;
		case OPCODE_dstore_1:
			dstore(1);
			break;
		case OPCODE_dstore_2:
			dstore(2);
			break;
		case OPCODE_dstore_3:
			dstore(3);
			break;
		case OPCODE_astore_0:
			astore(0);
			break;
		case OPCODE_astore_1:
			astore(1);
			break;
		case OPCODE_astore_2:
			astore(2);
			break;
		case OPCODE_astore_3:
			astore(3);
			break;
		case OPCODE_iastore:
			xastore();
			break;
		case OPCODE_lastore:
			xastore();
			break;
		case OPCODE_fastore:
			xastore();
			break;
		case OPCODE_dastore:
			xastore();
			break;
		case OPCODE_aastore:
			xastore();
			break;
		case OPCODE_bastore:
			xastore();
			break;
		case OPCODE_castore:
			xastore();
			break;
		case OPCODE_sastore:
			xastore();
			break;
		case OPCODE_pop:
			h.pop();
			break;
		case OPCODE_pop2:
			h.pop();
			h.pop();
			break;
		case OPCODE_dup: {
			auto t1 = h.pop();
			h.push(t1);
			h.push(t1);
			break;
		}
		case OPCODE_dup_x1: {
			auto t1 = h.pop();
			auto t2 = h.pop();
			h.push(t1);
			h.push(t2);
			h.push(t1);
			break;
		}
		case OPCODE_dup_x2: {
			auto t1 = h.pop();
			auto t2 = h.pop();
			auto t3 = h.pop();
			h.push(t1);
			h.push(t3);
			h.push(t2);
			h.push(t1);
			break;
		}
		case OPCODE_dup2: {
			auto t1 = h.pop();
			auto t2 = h.pop();
			h.push(t2);
			h.push(t1);
			h.push(t2);
			h.push(t1);
			break;
		}
		case OPCODE_dup2_x1: {
			auto t1 = h.pop();
			auto t2 = h.pop();
			auto t3 = h.pop();
			h.push(t2);
			h.push(t1);
			h.push(t3);
			h.push(t2);
			h.push(t1);
			break;
		}
		case OPCODE_dup2_x2: {
			auto t1 = h.pop();
			auto t2 = h.pop();
			auto t3 = h.pop();
			auto t4 = h.pop();
			h.push(t2);
			h.push(t1);
			h.push(t4);
			h.push(t3);
			h.push(t2);
			h.push(t1);
			break;
		}
		case OPCODE_swap: {
			auto t1 = h.pop();
			auto t2 = h.pop();
			h.push(t1);
			h.push(t2);
			break;
		}
		case OPCODE_iadd:
		case OPCODE_fadd:
		case OPCODE_isub:
		case OPCODE_fsub:
		case OPCODE_imul:
		case OPCODE_fmul:
		case OPCODE_idiv:
		case OPCODE_fdiv:
		case OPCODE_irem:
		case OPCODE_frem:
		case OPCODE_ishl:
		case OPCODE_ishr:
		case OPCODE_iushr:
		case OPCODE_iand:
		case OPCODE_ior:
		case OPCODE_ixor: {
			auto t1 = h.pop();
			h.pop();
			h.push(t1);
			break;
		}
		case OPCODE_ladd:
		case OPCODE_lsub:
		case OPCODE_lmul:
		case OPCODE_ldiv:
		case OPCODE_lrem:
		case OPCODE_lshl:
		case OPCODE_lshr:
		case OPCODE_lushr:
		case OPCODE_land:
		case OPCODE_lor:
		case OPCODE_lxor: {
			h.pop();
			h.pop();
			h.pop();
			h.pop();
			h.pushLong();
			break;
		}
		case OPCODE_dadd:
		case OPCODE_dsub:
		case OPCODE_dmul:
		case OPCODE_ddiv:
		case OPCODE_drem: {
			h.pop();
			h.pop();
			h.pop();
			h.pop();
			h.pushDouble();
			break;
		}
		case OPCODE_ineg:
		case OPCODE_fneg: {
			auto t1 = h.pop();
			h.push(t1);
			break;
		}
		case OPCODE_lneg: {
			h.pop();
			h.pop();
			h.pushLong();
			break;
		}
		case OPCODE_dneg: {
			h.pop();
			h.pop();
			h.pushDouble();
			break;
		}
		case OPCODE_iinc:
			h.setIntVar(inst.iinc.index);
			break;
		case OPCODE_i2l:
			h.pop();
			h.pushLong();
			break;
		case OPCODE_i2f:
			h.pop();
			h.pushFloat();
			break;
		case OPCODE_i2d:
			h.pop();
			h.pushDouble();
			break;
		case OPCODE_l2i:
			h.pop();
			h.pop();
			h.pushInt();
			break;
		case OPCODE_l2f:
			h.pop();
			h.pop();
			h.pushFloat();
			break;
		case OPCODE_l2d:
			h.pop();
			h.pop();
			h.pushDouble();
			break;
		case OPCODE_f2i:
			h.pop();
			h.pushInt();
			break;
		case OPCODE_f2l:
			h.pop();
			h.pushLong();
			break;
		case OPCODE_f2d:
			h.pop();
			h.pushDouble();
			break;
		case OPCODE_d2i:
			h.pop();
			h.pop();
			h.pushInt();
			break;
		case OPCODE_d2l:
			h.pop();
			h.pop();
			h.pushLong();
			break;
		case OPCODE_d2f:
			h.pop();
			h.pop();
			h.pushFloat();
			break;
		case OPCODE_i2b:
		case OPCODE_i2c:
		case OPCODE_i2s:
			h.pop();
			h.pushInt();
			break;
		case OPCODE_lcmp:
			h.pop();
			h.pop();
			h.pop();
			h.pop();
			h.pushInt();
			break;
		case OPCODE_fcmpl:
		case OPCODE_fcmpg:
			h.pop();
			h.pop();
			h.pushInt();
			break;
		case OPCODE_dcmpl:
		case OPCODE_dcmpg:
			h.pop();
			h.pop();
			h.pop();
			h.pop();
			h.pushInt();
			break;
		case OPCODE_ifeq:
		case OPCODE_ifne:
		case OPCODE_iflt:
		case OPCODE_ifge:
		case OPCODE_ifgt:
		case OPCODE_ifle:
			h.pop();
			break;
		case OPCODE_if_icmpeq:
		case OPCODE_if_icmpne:
		case OPCODE_if_icmplt:
		case OPCODE_if_icmpge:
		case OPCODE_if_icmpgt:
		case OPCODE_if_icmple:
			h.pop();
			h.pop();
			break;
		case OPCODE_if_acmpeq:
		case OPCODE_if_acmpne:
			h.pop();
			h.pop();
			break;
		case OPCODE_goto:
			break;
		case OPCODE_jsr:
			ASSERT(false, "jsr not implemented");
			break;
		case OPCODE_ret:
			ASSERT(false, "jsr not implemented");
			break;
		case OPCODE_tableswitch:
		case OPCODE_lookupswitch:
			h.pop();
			break;
		case OPCODE_ireturn:
			h.pop();
			break;
		case OPCODE_lreturn:
			h.pop();
			h.pop();
			break;
		case OPCODE_freturn:
			h.pop();
			break;
		case OPCODE_dreturn:
			h.pop();
			h.pop();
			break;
		case OPCODE_areturn:
			h.pop();
			break;
		case OPCODE_return:
			break;
		case OPCODE_getstatic:
		case OPCODE_putstatic:
		case OPCODE_getfield:
		case OPCODE_putfield:
		case OPCODE_invokevirtual:
		case OPCODE_invokespecial:
		case OPCODE_invokestatic:
		case OPCODE_invokeinterface:
		case OPCODE_invokedynamic:
			//ASSERT(false, "instances not implemented");
			break;
		case OPCODE_new:
			h.pushRef();
			break;
		case OPCODE_newarray:
			h.pop();
			h.pushRef();
			break;
		case OPCODE_anewarray:
			h.pop();
			h.pushRef();
			break;
		case OPCODE_arraylength:
			h.pop();
			h.pushInt();
			break;
		case OPCODE_athrow:
		case OPCODE_checkcast:
		case OPCODE_instanceof:
		case OPCODE_monitorenter:
		case OPCODE_monitorexit:
			ASSERT(false, "athrow checkcast instanceof me, me not implemented");
			break;
		case OPCODE_wide:
		case OPCODE_multianewarray:
		case OPCODE_ifnull:
		case OPCODE_ifnonnull:
		case OPCODE_goto_w:
		case OPCODE_jsr_w:
		case OPCODE_breakpoint:
		case OPCODE_impdep1:
		case OPCODE_impdep2:
			ASSERT(false, "extended not implemented");
			break;

		default:
			break;
	}
}

struct H {
	enum T {
		Top, Int, Long, Float, Double, Ref
	};

	friend ostream& operator<<(ostream& os, H::T t) {
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
		CHECK(stack.size() > 0, "Trying to pop in an empty stack.");

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
		CHECK(lvindex < 256, "");

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

	ostream& print(ostream& os) {
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
		os << " ]" << endl;

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
		CHECK(isAssignable(t, o) || isAssignable(o, t), "");

		if (isAssignable(t, o)) {
			if (t == o) {
				return false;
			}

			t = o;
			return true;
		}

		ASSERT(isAssignable(o, t), "");

		return false;
	}

	bool join(H& how, ostream& os) {
		this->print(os);
		how.print(os);

		CHECK(stack.size() == how.stack.size(), "");

		if (lva.size() < how.lva.size()) {
			lva.resize(how.lva.size());
		} else if (how.lva.size() < lva.size()) {
			how.lva.resize(lva.size());
		}

		ASSERT(lva.size() == how.lva.size(), "%ld != %ld", lva.size(),
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

ostream& operator<<(ostream& os, Version version) {
	return os << version.getMajor() << "." << version.getMinor();
}

struct ClassPrinter {

	static const char* OPCODES[];

	static const char* ConstNames[];

	ClassPrinter(ClassFile& cf, ostream& os, int tabs) :
			cf(cf), os(os), tabs(tabs) {
	}

	void print() {
		line() << AccessFlagsPrinter(cf.accessFlags) << " class "
				<< cf.getThisClassName() << "#" << cf.thisClassIndex << endl;

		inc();
		line() << "Version: " << cf.getVersion() << endl;

		inc();

		line() << "Constant Pool Items [" << ((ConstPool) cf).size() << "]"
				<< endl;
		inc();
		printConstPool(cf);
		dec();

		line() << "accessFlags: " << cf.accessFlags << endl;
		line() << "thisClassIndex: " << cf.getThisClassName() << "#"
				<< cf.thisClassIndex << endl;

		if (cf.superClassIndex != 0) {
			line() << "superClassIndex: " << cf.getClassName(cf.superClassIndex)
					<< "#" << cf.superClassIndex << endl;
		}

		for (u2 interIndex : cf.interfaces) {
			line() << "Interface '" << cf.getClassName(interIndex) << "'#"
					<< interIndex << endl;
		}

		for (Field& f : cf.fields) {
			line() << "Field " << cf.getUtf8(f.nameIndex) << ": "
					<< AccessFlagsPrinter(f.accessFlags) << " #" << f.nameIndex
					<< ": " << cf.getUtf8(f.descIndex) << "#" << f.descIndex
					<< endl;

			printAttrs(f);
		}

		for (Method& m : cf.methods) {
			line() << "+Method " << AccessFlagsPrinter(m.accessFlags) << " "
					<< cf.getUtf8(m.nameIndex) << ": " << " #" << m.nameIndex
					<< ": " << cf.getUtf8(m.descIndex) << "#" << m.descIndex
					<< endl;

			printAttrs(m, &m);
		}

		printAttrs(cf);

		dec();
	}

	void printConstPool(ConstPool& cp) {
		line() << "#0 [null entry]: -" << endl;

		for (ConstPool::Iterator it = cp.iterator(); it.hasNext(); it++) {
			ConstPool::Index i = *it;
			ConstTag tag = cp.getTag(i);

			line() << "#" << i << " [" << ConstNames[tag] << "]: ";

			const ConstPool::Entry* entry = &cp.entries[i];

			cp.get(i, [&](ConstPool::Class e) {
				os << cp.getClassName(i) << "#" << e.nameIndex;
			}, [&](ConstPool::FieldRef e) {
				string clazzName, name, desc;
				cp.getFieldRef(i, &clazzName, &name, &desc);

				os << clazzName << "#" << e.classIndex << "."
				<< name << ":" << desc << "#"
				<< e.nameAndTypeIndex;
			}, [&](ConstPool::MethodRef e) {
				string clazzName, name, desc;
				cp.getMethodRef(i, &clazzName, &name, &desc);

				os << clazzName << "#" << e.classIndex << "."
				<< name << ":" << desc << "#"
				<< e.nameAndTypeIndex;
			}, [&](ConstPool::InterMethodRef e) {
				string clazzName, name, desc;
				cp.getInterMethodRef(i, &clazzName, &name, &desc);

				os << clazzName << "#" << e.classIndex << "."
				<< name << ":" << desc << "#"
				<< e.nameAndTypeIndex;
			});

			switch (tag) {
				case CONSTANT_Class:
//					os << cp.getClassName(i) << "#" << entry->clazz.nameIndex;
//					break;
				case CONSTANT_Fieldref: //{
//					string clazzName, name, desc;
//					cp.getFieldRef(i, &clazzName, &name, &desc);
//
//					os << clazzName << "#" << entry->memberref.classIndex << "."
//							<< name << ":" << desc << "#"
//							<< entry->memberref.nameAndTypeIndex;
//					break;
//				}
//
				case CONSTANT_Methodref: //{
//					string clazzName, name, desc;
//					cp.getMethodRef(i, &clazzName, &name, &desc);
//
//					os << clazzName << "#" << entry->memberref.classIndex << "."
//							<< name << ":" << desc << "#"
//							<< entry->memberref.nameAndTypeIndex;
//					break;
//				}
//
				case CONSTANT_InterfaceMethodref: //{
//					string clazzName, name, desc;
//					cp.getInterMethodRef(i, &clazzName, &name, &desc);
//
//					os << clazzName << "#" << entry->memberref.classIndex << "."
//							<< name << ":" << desc << "#"
//							<< entry->memberref.nameAndTypeIndex;
					break;
//				}
				case CONSTANT_String:
					os << cp.getUtf8(entry->s.stringIndex) << "#"
							<< entry->s.stringIndex;
					break;
				case CONSTANT_Integer:
					os << entry->i.value;
					break;
				case CONSTANT_Float:
					os << entry->f.value;
					break;
				case CONSTANT_Long:
					os << cp.getLong(i);
					//i++;
					break;
				case CONSTANT_Double:
					os << cp.getDouble(i);
					//i++;
					break;
				case CONSTANT_NameAndType:
					os << "#" << entry->nameandtype.nameIndex << ".#"
							<< entry->nameandtype.descriptorIndex;
					break;
				case CONSTANT_Utf8:
					os << entry->utf8.str;
					break;
				case CONSTANT_MethodHandle:
					os << entry->methodhandle.referenceKind << " #"
							<< entry->methodhandle.referenceIndex;
					break;
				case CONSTANT_MethodType:
					os << "#" << entry->methodtype.descriptorIndex;
					break;
				case CONSTANT_InvokeDynamic:
					os << "#" << entry->invokedynamic.bootstrapMethodAttrIndex
							<< ".#" << entry->invokedynamic.nameAndTypeIndex;
					break;
			}

			os << endl;
		}
	}

	void printAttrs(Attrs& attrs, void* args = nullptr) {
		for (Attr* attrp : attrs) {
			Attr& attr = *attrp;

			switch (attr.kind) {
				case ATTR_UNKNOWN:
					printUnknown((UnknownAttr&) attr);
					break;
				case ATTR_SOURCEFILE:
					printSourceFile((SourceFileAttr&) attr);
					break;
				case ATTR_CODE:
					printCode((CodeAttr&) attr, (Method*) args);
					break;
				case ATTR_EXCEPTIONS:
					printExceptions((ExceptionsAttr&) attr);
					break;
				case ATTR_LVT:
					printLvt((LvtAttr&) attr);
					break;
				case ATTR_LVTT:
					printLvt((LvtAttr&) attr);
					break;
				case ATTR_LNT:
					printLnt((LntAttr&) attr);
					break;
				case ATTR_SMT:
					printSmt((SmtAttr&) attr);
					break;
			}
		}
	}

	void printSourceFile(SourceFileAttr& attr) {
		const string& sourceFileName = cf.getUtf8(attr.sourceFileIndex);
		line() << "Source file: " << sourceFileName << "#"
				<< attr.sourceFileIndex << endl;
	}

	void printUnknown(UnknownAttr& attr) {
		const string& attrName = cf.getUtf8(attr.nameIndex);

		line() << "  Attribute unknown '" << attrName << "' # "
				<< attr.nameIndex << "[" << attr.len << "]" << endl;

	}

	struct State {
		H in;
		H out;
	};

	void printCode(CodeAttr& c, Method* m) {
		line(1) << "maxStack: " << c.maxStack << ", maxLocals: " << c.maxLocals
				<< endl;

		line(1) << "Code length: " << c.codeLen << endl;

		inc();

		int id = 1;
		for (Inst* inst : c.instList) {
			if (inst->kind == KIND_LABEL) {
				inst->label.id = id;
				id++;
			}
		}

		H h;

		int lvstart = [&]() {
			if (m->accessFlags & ACC_STATIC) {
				return 0;
			} else {
				h.setRefVar(0); // this argument
				return 1;
			}
		}();

		const char* methodDesc = cf.getUtf8(m->descIndex);
		parseMethodDesc(methodDesc, h, lvstart);

		H initState = h;

		for (Inst* inst : c.instList) {
			printInst(*inst);
			computeFrame(*inst, cf, h);
			os << setw(10);
			h.print(os);
		}

		ControlFlowGraph cfg(c.instList);
		printCfg(cfg);

		std::vector<State> states(cfg.nodeCount());
		initState.valid = true;
		states[cfg.entry._index] = {initState, initState};

		auto to = *cfg.outEdges(cfg.entry).begin();
		computeState(to, initState, cfg, states, c.instList);

		for (auto nkey : cfg) {
			InstList::iterator b;
			InstList::iterator e;
			std::string name;
			std::tie(b, e, name) = cfg.getNode(nkey);

			os << name << ": ";
			State& s = states[nkey._index];
			s.in.print(os);
			s.out.print(os);
			os << endl;
		}

		for (CodeExceptionEntry& e : c.exceptions) {
			line(1) << "exception entry: startpc: " << e.startpc->label.id
					<< ", endpc: " << e.endpc->label.offset << ", handlerpc: "
					<< e.handlerpc->label.offset << ", catchtype: "
					<< e.catchtype << endl;
		}

		printAttrs(c.attrs);

		dec();
	}

	void computeState(ControlFlowGraph::NodeKey to, H& how,
			ControlFlowGraph& cfg, std::vector<State>& states,
			InstList& instList) {

		InstList::iterator b;
		InstList::iterator e;
		std::string name;
		std::tie(b, e, name) = cfg.getNode(to);
		os << "computing " << name << endl;

		if (b == instList.end()) {
			ASSERT(name == "Exit", "");
			ASSERT(e == instList.end(), "");
			return;
		}

		ASSERT(how.valid, "");

		State& s = states[to._index];

		ASSERT(s.in.valid == s.out.valid, "");

		bool change;
		if (!s.in.valid) {
			s.in = how;
			s.out = s.in;
			change = true;
		} else {
			change = s.in.join(how, os);
		}

		if (change) {
			for (auto it = b; it != e; it++) {
				Inst* inst = *it;
				computeFrame(*inst, cf, s.out);
			}

			os << "  - in: ";
			s.in.print(os);
			os << "  - out: ";
			s.out.print(os);

			H h = s.out;

			for (auto nid : cfg.outEdges(to)) {
//				fprintf(stderr, "asf");

				InstList::iterator b;
				InstList::iterator e;
				std::string name;
				std::tie(b, e, name) = cfg.getNode(nid);
//
//				if (b == instList.end()) {
//					ASSERT(name == "Entry" || name == "Exit", "");
//					ASSERT(e == instList.end(), "");
//					continue;
//				}

//				H h = outState;
//				for (auto it = b; it != e; it++) {
//					Inst* inst = *it;
//					computeFrame(*inst, cf, h);
//				}

//				h.print(os);
//				os << "  going to " << name << endl;

				computeState(nid, h, cfg, states, instList);
			}
		}
	}

	void printCfg(ControlFlowGraph& cfg) {
		for (auto nid : cfg) {
			InstList::iterator b;
			InstList::iterator e;
			std::string name;
			std::tie(b, e, name) = cfg.getNode(nid);

			os << "* " << name;

			auto printEdges =
					[&](ControlFlowGraph::EdgeIterable edges, const char* kind, const char* arrow) {
						os << " @" << kind << " { ";
						for (auto eid : edges) {
							auto bb = cfg.getNode(eid);
							os << arrow << std::get<2>(bb) << ", ";
						}
						os << "} ";
					};

			printEdges(cfg.outEdges(nid), "Out", "->");
			printEdges(cfg.inEdges(nid), "In", "<-");
			os << endl;

			for (auto it = b; it != e; it++) {
				Inst* inst = *it;
				printInst(*inst);
				os << endl;
			}
		}

		os << endl;
	}

	void printInst(Inst& inst) {
		int offset = inst._offset;

		if (inst.kind == KIND_LABEL) {
			os << "   label: " << inst.label.id;
			return;
		}

		line() << setw(4) << offset << ": (" << setw(3) << (int) inst.opcode
				<< ") " << OPCODES[inst.opcode] << " ";

		ostream& instos = os;

		switch (inst.kind) {
			case KIND_ZERO:
				//instos << endl;
				break;
			case KIND_BIPUSH:
				instos << int(inst.push.value);
				break;
			case KIND_SIPUSH:
				instos << int(inst.push.value);
				break;
			case KIND_LDC:
				instos << "#" << int(inst.ldc.valueIndex);
				break;
			case KIND_VAR:
				instos << int(inst.var.lvindex);
				break;
			case KIND_IINC:
				instos << int(inst.iinc.index) << " " << int(inst.iinc.value);
				break;
			case KIND_JUMP:
				instos << "label: " << inst.jump.label2->label.id;
				break;
			case KIND_TABLESWITCH:
				instos << "default: " << inst.ts.def->label.id << ", from: "
						<< inst.ts.low << " " << inst.ts.high << ":";

				for (int i = 0; i < inst.ts.high - inst.ts.low + 1; i++) {
					Inst* l = inst.ts.targets[i];
					os << " " << l->label.id;
				}

				//instos;

				break;
			case KIND_LOOKUPSWITCH:
				instos << inst.ls.defbyte->label.id << " " << inst.ls.npairs
						<< ":";

				for (u4 i = 0; i < inst.ls.npairs; i++) {
					u4 k = inst.ls.keys[i];
					Inst* l = inst.ls.targets[i];
					instos << " " << k << " -> " << l->label.id;
				}

				//instos << endl;
				break;
			case KIND_FIELD: {
				string className, name, desc;
				cf.getFieldRef(inst.field.fieldRefIndex, &className, &name,
						&desc);

				instos << className << name << desc;

				break;
			}
			case KIND_INVOKE: {
				string className, name, desc;
				cf.getMethodRef(inst.invoke.methodRefIndex, &className, &name,
						&desc);

				instos << className << "." << name << ": " << desc;

				break;
			}
			case KIND_INVOKEINTERFACE: {
				string className, name, desc;
				cf.getInterMethodRef(inst.invokeinterface.interMethodRefIndex,
						&className, &name, &desc);

				instos << className << "." << name << ": " << desc << "("
						<< inst.invokeinterface.count << ")";
				break;
			}
			case KIND_INVOKEDYNAMIC:
				EXCEPTION("FrParseInvokeDynamicInstr not implemented");
				break;
			case KIND_TYPE: {
				string className = cf.getClassName(inst.type.classIndex);

				instos << className;

				break;
			}
			case KIND_NEWARRAY:
				instos << int(inst.newarray.atype);

				break;
			case KIND_MULTIARRAY: {
				string className = cf.getClassName(inst.multiarray.classIndex);

				instos << className << " " << inst.multiarray.dims;

				break;
			}
			case KIND_PARSE4TODO:
				EXCEPTION("FrParse4__TODO__Instr not implemented");
				break;
			case KIND_RESERVED:
				EXCEPTION("FrParseReservedInstr not implemented");
				break;
			default:
				EXCEPTION("should not arrive here!");
		}
	}

	void printExceptions(ExceptionsAttr& attr) {
		for (u4 i = 0; i < attr.es.size(); i++) {
			u2 exceptionIndex = attr.es[i];

			const string& exceptionName = cf.getClassName(exceptionIndex);

			line() << "  Exceptions entry: '" << exceptionName << "'#"
					<< exceptionIndex << endl;
		}
	}

	void printLnt(LntAttr& attr) {
		for (LntAttr::LnEntry e : attr.lnt) {
			line() << "  LocalNumberTable entry: startpc: " << e.startpc
					<< ", lineno: " << e.lineno << endl;
		}
	}

	void printLvt(LvtAttr& attr) {
		for (LvtAttr::LvEntry e : attr.lvt) {
			line() << "  LocalVariable(or Type)Table  entry: start: "
					<< e.startPc << ", len: " << e.len << ", varNameIndex: "
					<< e.varNameIndex << ", varDescIndex: " << e.varDescIndex
					<< ", index: " << endl;
		}
	}

	void printSmt(SmtAttr& smt) {

		auto parseTs =
				[&](std::vector<SmtAttr::VerType> locs) {
					line(2) << "["<<locs.size()<<"] ";
					for (u1 i = 0; i < locs.size(); i++) {
						SmtAttr::VerType& vt = locs[i];
						u1 tag = vt.tag;

						switch (tag) {
							case ITEM_Top:
							os << "top" << " | ";
							break;
							case ITEM_Integer:
							os << "integer" << " | ";
							break;
							case ITEM_Float :
							os << "float" << " | ";
							break;
							case ITEM_Long :
							os << "long" << " | ";
							break;
							case ITEM_Double:
							os << "double" << " | ";
							break;
							case ITEM_Null :
							os << "null" << " | ";
							break;
							case ITEM_UninitializedThis :
							os << "UninitializedThis" << " | ";
							break;
							case ITEM_Object:
							os << "Object: cpindex = " << vt.Object_variable_info.cpool_index << " | ";
							break;
							case ITEM_Uninitialized: {
								os << "Uninitialized: offset = " << vt.Uninitialized_variable_info.offset << " | ";
								break;
							}
						}

					}

					os << endl;
				};

		line() << "Stack Map Table: " << endl;

		int toff = -1;
		for (SmtAttr::Entry& e : smt.entries) {
			line(1) << "frame type (" << e.frameType << ") ";

			u1 frameType = e.frameType;

			if (0 <= frameType && frameType <= 63) {
				toff += frameType + 1;
				os << "offset = " << toff << " ";

				os << "same frame" << endl;
			} else if (64 <= frameType && frameType <= 127) {
				toff += frameType - 64 + 1;
				os << "offset = " << toff << " ";

				os << "sameLocals_1_stack_item_frame. ";
				parseTs(e.sameLocals_1_stack_item_frame.stack);
			} else if (frameType == 247) {
				toff += e.same_locals_1_stack_item_frame_extended.offset_delta
						+ 1;
				os << "offset = " << toff << " ";

				os << "same_locals_1_stack_item_frame_extended. ";
				os << e.same_locals_1_stack_item_frame_extended.offset_delta
						<< endl;
				parseTs(e.same_locals_1_stack_item_frame_extended.stack);
			} else if (248 <= frameType && frameType <= 250) {
				toff += e.chop_frame.offset_delta + 1;
				os << "offset = " << toff << " ";

				os << "chop_frame, ";
				os << "offset_delta = " << e.chop_frame.offset_delta << endl;
			} else if (frameType == 251) {
				toff += e.same_frame_extended.offset_delta + 1;
				os << "offset = " << toff << " ";

				os << "same_frame_extended. ";
				os << e.same_frame_extended.offset_delta << endl;
			} else if (252 <= frameType && frameType <= 254) {
				toff += e.append_frame.offset_delta + 1;
				os << "offset = " << toff << " ";

				os << "append_frame, ";
				os << "offset_delta = " << e.append_frame.offset_delta << endl;
				parseTs(e.append_frame.locals);
			} else if (frameType == 255) {
				toff += e.full_frame.offset_delta + 1;
				os << "offset = " << toff << " ";

				os << "full_frame. ";
				os << e.full_frame.offset_delta << endl;
				parseTs(e.full_frame.locals);
				parseTs(e.full_frame.stack);
			}
		}
	}

private:

	ClassFile& cf;

	ostream& os;

	int tabs;

	inline void inc() {
		tabs++;
	}

	inline void dec() {
		tabs--;
	}

	inline ostream& line(int moretabs = 0) {
		return tab(os, moretabs);
	}

	inline ostream& tab(ostream& os, int moretabs = 0) {
		for (int _ii = 0; _ii < tabs + moretabs; _ii++) {
			os << "  ";
		}

		return os;
	}
};

const char* ClassPrinter::ConstNames[] = { "**** 0 ****", // 0
		"Utf8",			// 1
		"**** 2 ****",	// 2
		"Integer",		// 3
		"Float",		// 4
		"Long",			// 5
		"Double",		// 6
		"Class",		// 7
		"String",		// 8
		"Fieldref",		// 9
		"Methodref",		// 10
		"InterfaceMethodref",	// 11
		"NameAndType",		// 12
		"**** 13 ****",	// 13
		"**** 14 ****",	// 14
		"MethodHandle",	// 15
		"MethodType",	// 16
		"**** 17 ****",	// 17
		"InvokeDynamic",	// 18
		};

const char* ClassPrinter::OPCODES[] = { "nop", "aconst_null", "iconst_m1",
		"iconst_0", "iconst_1", "iconst_2", "iconst_3", "iconst_4", "iconst_5",
		"lconst_0", "lconst_1", "fconst_0", "fconst_1", "fconst_2", "dconst_0",
		"dconst_1", "bipush", "sipush", "ldc", "ldc_w", "ldc2_w", "iload",
		"lload", "fload", "dload", "aload", "iload_0", "iload_1", "iload_2",
		"iload_3", "lload_0", "lload_1", "lload_2", "lload_3", "fload_0",
		"fload_1", "fload_2", "fload_3", "dload_0", "dload_1", "dload_2",
		"dload_3", "aload_0", "aload_1", "aload_2", "aload_3", "iaload",
		"laload", "faload", "daload", "aaload", "baload", "caload", "saload",
		"istore", "lstore", "fstore", "dstore", "astore", "istore_0",
		"istore_1", "istore_2", "istore_3", "lstore_0", "lstore_1", "lstore_2",
		"lstore_3", "fstore_0", "fstore_1", "fstore_2", "fstore_3", "dstore_0",
		"dstore_1", "dstore_2", "dstore_3", "astore_0", "astore_1", "astore_2",
		"astore_3", "iastore", "lastore", "fastore", "dastore", "aastore",
		"bastore", "castore", "sastore", "pop", "pop2", "dup", "dup_x1",
		"dup_x2", "dup2", "dup2_x1", "dup2_x2", "swap", "iadd", "ladd", "fadd",
		"dadd", "isub", "lsub", "fsub", "dsub", "imul", "lmul", "fmul", "dmul",
		"idiv", "ldiv", "fdiv", "ddiv", "irem", "lrem", "frem", "drem", "ineg",
		"lneg", "fneg", "dneg", "ishl", "lshl", "ishr", "lshr", "iushr",
		"lushr", "iand", "land", "ior", "lor", "ixor", "lxor", "iinc", "i2l",
		"i2f", "i2d", "l2i", "l2f", "l2d", "f2i", "f2l", "f2d", "d2i", "d2l",
		"d2f", "i2b", "i2c", "i2s", "lcmp", "fcmpl", "fcmpg", "dcmpl", "dcmpg",
		"ifeq", "ifne", "iflt", "ifge", "ifgt", "ifle", "if_icmpeq",
		"if_icmpne", "if_icmplt", "if_icmpge", "if_icmpgt", "if_icmple",
		"if_acmpeq", "if_acmpne", "goto", "jsr", "ret", "tableswitch",
		"lookupswitch", "ireturn", "lreturn", "freturn", "dreturn", "areturn",
		"return", "getstatic", "putstatic", "getfield", "putfield",
		"invokevirtual", "invokespecial", "invokestatic", "invokeinterface",
		"invokedynamic", "new", "newarray", "anewarray", "arraylength",
		"athrow", "checkcast", "instanceof", "monitorenter", "monitorexit",
		"wide", "multianewarray", "ifnull", "ifnonnull", "goto_w", "jsr_w",
		"breakpoint", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "impdep1",
		"impdep2" };

void printClassFile(ClassFile& cf, ostream& os, int tabs) {
	ClassPrinter cp(cf, os, tabs);
	cp.print();
}

}
