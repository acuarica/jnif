#ifndef JNIF_CLASSPRINTERVISITOR_HPP
#define JNIF_CLASSPRINTERVISITOR_HPP

#include <ostream>
#include <iomanip>

#include "base.hpp"

namespace jnif {

/**
 * OPCODES names definition
 *
 */
static const char* OPCODES[] = { "nop", "aconst_null", "iconst_m1", "iconst_0",
		"iconst_1", "iconst_2", "iconst_3", "iconst_4", "iconst_5", "lconst_0",
		"lconst_1", "fconst_0", "fconst_1", "fconst_2", "dconst_0", "dconst_1",
		"bipush", "sipush", "ldc", "ldc_w", "ldc2_w", "iload", "lload", "fload",
		"dload", "aload", "iload_0", "iload_1", "iload_2", "iload_3", "lload_0",
		"lload_1", "lload_2", "lload_3", "fload_0", "fload_1", "fload_2",
		"fload_3", "dload_0", "dload_1", "dload_2", "dload_3", "aload_0",
		"aload_1", "aload_2", "aload_3", "iaload", "laload", "faload", "daload",
		"aaload", "baload", "caload", "saload", "istore", "lstore", "fstore",
		"dstore", "astore", "istore_0", "istore_1", "istore_2", "istore_3",
		"lstore_0", "lstore_1", "lstore_2", "lstore_3", "fstore_0", "fstore_1",
		"fstore_2", "fstore_3", "dstore_0", "dstore_1", "dstore_2", "dstore_3",
		"astore_0", "astore_1", "astore_2", "astore_3", "iastore", "lastore",
		"fastore", "dastore", "aastore", "bastore", "castore", "sastore", "pop",
		"pop2", "dup", "dup_x1", "dup_x2", "dup2", "dup2_x1", "dup2_x2", "swap",
		"iadd", "ladd", "fadd", "dadd", "isub", "lsub", "fsub", "dsub", "imul",
		"lmul", "fmul", "dmul", "idiv", "ldiv", "fdiv", "ddiv", "irem", "lrem",
		"frem", "drem", "ineg", "lneg", "fneg", "dneg", "ishl", "lshl", "ishr",
		"lshr", "iushr", "lushr", "iand", "land", "ior", "lor", "ixor", "lxor",
		"iinc", "i2l", "i2f", "i2d", "l2i", "l2f", "l2d", "f2i", "f2l", "f2d",
		"d2i", "d2l", "d2f", "i2b", "i2c", "i2s", "lcmp", "fcmpl", "fcmpg",
		"dcmpl", "dcmpg", "ifeq", "ifne", "iflt", "ifge", "ifgt", "ifle",
		"if_icmpeq", "if_icmpne", "if_icmplt", "if_icmpge", "if_icmpgt",
		"if_icmple", "if_acmpeq", "if_acmpne", "goto", "jsr", "ret",
		"tableswitch", "lookupswitch", "ireturn", "lreturn", "freturn",
		"dreturn", "areturn", "return", "getstatic", "putstatic", "getfield",
		"putfield", "invokevirtual", "invokespecial", "invokestatic",
		"invokeinterface", "invokedynamic", "new", "newarray", "anewarray",
		"arraylength", "athrow", "checkcast", "instanceof", "monitorenter",
		"monitorexit", "wide", "multianewarray", "ifnull", "ifnonnull",
		"goto_w", "jsr_w", "breakpoint", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED", "RESERVED",
		"impdep1", "impdep2", };

struct PrinterData {
	std::ostream& os;

	ConstPool cp;

	int tabs;

	void inc() {
		tabs++;
	}

	void dec() {
		tabs--;
	}

	std::ostream& line(int moretabs = 0) {
		return tab(os, moretabs);
	}

	std::ostream& tab(std::ostream& os, int moretabs = 0) {
		for (int _ii = 0; _ii < tabs + moretabs; _ii++) {
			os << "  ";
		}

		return os;
	}
};

struct BasePrinter {
	PrinterData& pd;

	BasePrinter(PrinterData& pd) :
			pd(pd) {
	}
};

struct AccessFlagsPrinter {
	AccessFlagsPrinter(u2 value, const char* sep = " ") :
			value(value), sep(sep) {
	}

	friend std::ostream& operator<<(std::ostream& out,
			AccessFlagsPrinter self) {
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

struct ExceptionsAttrPrinter {

	struct Printer: BasePrinter {

		Printer(PrinterData& pd) :
				BasePrinter(pd) {
		}

		void visitExceptionCount(u2 count) {
			pd.os << "Excep count: " << count << std::endl;
		}

		void visitExceptionEntry(u2 exceptionIndex) {
			const std::string& exceptionName = pd.cp.getClazzName(
					exceptionIndex);

			pd.line() << "  Exceptions entry: '" << exceptionName << "'#"
					<< exceptionIndex << std::endl;
		}
	};
};

struct ClassPrinter {

	ClassPrinter(std::ostream& os) :
			cv(cv), os(os), tabs(0) {
		line() << "Class file " << className << " [file size: " << fileImageLen
				<< "]" << std::endl;
		inc();
	}

	class Field {
	public:
		typename TForward::Field fv;
		ClassPrinter& cpv;

		inline Field(typename TForward::Field& fv, ClassPrinter& cpv) :
				fv(fv), cpv(cpv) {
		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			fv.visitAttr(nameIndex, len, data);

			const std::string& attrName = cpv.cp.getUtf8(nameIndex);

			line() << "  Attribute unknown '" << attrName << "' #" << nameIndex
					<< "[" << len << "]" << std::endl;
		}

	private:
		inline std::ostream& line() {
			return cpv.line();
		}
	};

	class Method {
	public:
		typename TForward::Method mv;
		ClassPrinter& cpv;

		inline Method(typename TForward::Method& mv, ClassPrinter& cpv) :
				mv(mv), cpv(cpv) {
		}

		class Code {
		public:
			typename TForward::Method::Code bv;

			inline Code(ClassPrinter& cpv, typename TForward::Method::Code& bv) :
					bv(std::move(bv)), cpv(cpv) {
			}

			inline void enter(u2 maxStack, u2 maxLocals) {
				line(1) << "maxStack: " << maxStack << ", maxLocals: "
						<< maxLocals << std::endl;
				cpv.inc();

				bv.enter(maxStack, maxLocals);
			}

			inline void exit() {
				bv.exit();

				cpv.dec();
			}

			inline void visitExceptionEntry(u2 startpc, u2 endpc, u2 handlerpc,
					u2 catchtype) {
				line(1) << "exception entry: startpc: " << startpc
						<< ", endpc: " << endpc << ", handlerpc: " << handlerpc
						<< ", catchtype: " << catchtype << std::endl;

				bv.visitExceptionEntry(startpc, endpc, handlerpc, catchtype);
			}

			void visitZero(int offset, u1 opcode) {
				bv.visitZero(offset, opcode);

				line(offset, opcode) << "" << std::endl;
			}

			void visitField(int offset, u1 opcode, u2 fieldRefIndex,
					const std::string& className, const std::string& name,
					const std::string& desc) {
				bv.visitField(offset, opcode, fieldRefIndex, className, name,
						desc);

				line(offset, opcode) << className << name << desc << std::endl;
			}

			void visitBiPush(int offset, u1 opcode, u1 bytevalue) {
				bv.visitBiPush(offset, opcode, bytevalue);

				line(offset, opcode) << int(bytevalue) << std::endl;
			}

			void visitSiPush(int offset, u1 opcode, u2 shortvalue) {
				bv.visitSiPush(offset, opcode, shortvalue);

				line(offset, opcode) << int(shortvalue) << std::endl;
			}

			void visitNewArray(int offset, u1 opcode, u1 atype) {
#define T_BOOLEAN	4
#define 				T_CHAR	5
#define 		T_FLOAT	6
#define 		T_DOUBLE	7
#define 		T_BYTE	8
#define 		T_SHORT	9
#define 		T_INT	10
#define 		T_LONG	11

				bv.visitNewArray(offset, opcode, atype);

				line(offset, opcode) << int(atype) << std::endl;
			}

			void visitType(int offset, u1 opcode, u2 classIndex,
					const std::string& className) {
				bv.visitType(offset, opcode, classIndex, className);

				line(offset, opcode) << className << std::endl;
			}

			void visitJump(int offset, u1 opcode, u2 targetOffset) {
				bv.visitJump(offset, opcode, targetOffset);

				line(offset, opcode) << offset + targetOffset << std::endl;
			}

			void visitMultiArray(int offset, u1 opcode, u2 classIndex,
					const std::string& className, u4 dims) {
				bv.visitMultiArray(offset, opcode, classIndex, className, dims);

				line(offset, opcode) << className << " " << dims << std::endl;
			}

			void visitIinc(int offset, u1 opcode, u1 index, u1 value) {
				bv.visitIinc(offset, opcode, index, value);

				line(offset, opcode) << int(index) << " " << int(value)
						<< std::endl;
			}

			void visitLdc(int offset, u1 opcode, u2 arg) {
				bv.visitLdc(offset, opcode, arg);

				line(offset, opcode) << arg << std::endl;
			}

			void visitInvokeInterface(int offset, u1 opcode,
					u2 interMethodrefIndex, const std::string& className,
					const std::string& name, const std::string& desc,
					u1 count) {
				bv.visitInvokeInterface(offset, opcode, interMethodrefIndex,
						className, name, desc, count);

				line(offset, opcode) << className << "." << name << ": " << desc
						<< "(" << count << ")" << std::endl;
			}

			void visitInvoke(int offset, u1 opcode, u2 methodrefIndex,
					const std::string&, const std::string&,
					const std::string&) {

				std::string className, name, desc;
				cpv.cp.getMemberRef(methodrefIndex, &className, &name, &desc,
						CONSTANT_Methodref);

				line(offset, opcode) << className << "." << name << ": " << desc
						<< std::endl;

				bv.visitInvoke(offset, opcode, methodrefIndex, className, name,
						desc);
			}

			void visitVar(int offset, u1 opcode, u2 lvindex) {
				bv.visitVar(offset, opcode, lvindex);

				line(offset, opcode) << lvindex << std::endl;
			}

			void visitTableSwitch(int offset, u1 opcode, int def, int low,
					int high, const std::vector<u4>& targets) {
				bv.visitTableSwitch(offset, opcode, def, low, high, targets);

				line(offset, opcode) << def << " " << " " << low << " " << high
						<< ":";

				for (int i = 0; i < high - low + 1; i++) {
					os() << " " << targets[i];
				}

				os() << std::endl;
			}

			void visitLookupSwitch(int offset, u1 opcode, u4 defbyte, u4 npairs,
					const std::vector<u4>& keys,
					const std::vector<u4>& targets) {
				bv.visitLookupSwitch(offset, opcode, defbyte, npairs, keys,
						targets);

				line(offset, opcode) << defbyte << " " << npairs << ":";

				for (u4 i = 0; i < npairs; i++) {
					cpv.os << " " << keys[i] << " -> " << targets[i];
				}

				os() << std::endl;
			}

			inline void visitLnt(u2 nameIndex, u2 startpc, u2 lineno) {
				bv.visitLnt(nameIndex, startpc, lineno);

				line() << "  LocalNumberTable entry: startpc: " << startpc
						<< ", lineno: " << lineno << std::endl;
			}

			inline void visitLvt(u2 startPc, u2 len, u2 varNameIndex,
					u2 varDescIndex, u2 index) {
				bv.visitLvt(startPc, len, varNameIndex, varDescIndex, index);

				line() << "  LocalVariableTable entry: start: " << startPc
						<< ", len: " << len << ", varNameIndex: "
						<< varNameIndex << ", varDescIndex: " << varDescIndex
						<< ", index: " << std::endl;
			}

			inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
				bv.visitAttr(nameIndex, len, data);

				const std::string& attrName = cpv.cp.getUtf8(nameIndex);

				line() << "  Attribute unknown '" << attrName << "' # "
						<< nameIndex << "[" << len << "]" << std::endl;
			}

			inline void visitFrameSame(u1 frameType) {
				line() << "Stack entry SAME: " << int(frameType) << std::endl;
			}

			inline void visitFrameSameLocals1StackItem(u1 frameType) {
				line() << "Stack entry SAME LOCALS 1 STACK ITEM: "
						<< int(frameType) << std::endl;
			}

		private:
			ClassPrinter& cpv;

			inline std::ostream& os() {
				return cpv.os;
			}

			inline std::ostream& line(int moretabs = 0) {
				return cpv.line(moretabs);
			}

			inline std::ostream& line(int offset, u1 opcode, int moretabs = 0) {
				return cpv.line(moretabs) << std::setw(4) << offset << ": ("
						<< std::setw(3) << (int) opcode << ") "
						<< OPCODES[opcode] << " ";
			}
		};

		inline Code visitCode(u2 nameIndex) {
			typename TForward::Method::Code bv = mv.visitCode(nameIndex);

			return Code(cpv, bv);
		}

//		inline void visitException(u2 nameIndex, std::vector<u2>& es) {
//			mv.visitException(nameIndex, es);
//
//			for (u4 i = 0; i < es.size(); i++) {
//				u2 exceptionIndex = es[i];
//
//				const std::string& exceptionName = cpv.cp.getClazzName(
//						exceptionIndex);
//
//				line() << "  Exceptions entry: '" << exceptionName << "'#"
//						<< exceptionIndex << std::endl;
//			}
//		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			mv.visitAttr(nameIndex, len, data);

			const std::string& attrName = cpv.cp.getUtf8(nameIndex);

			line() << "  Attribute unknown '" << attrName << "' # " << nameIndex
					<< "[" << len << "]" << std::endl;
		}

	private:

		inline std::ostream& os() {
			return cpv.os;
		}

		inline std::ostream& line(int moretabs = 0) {
			return cpv.line(moretabs);
		}

		inline std::ostream& line(int offset, u1 opcode, int moretabs = 0) {
			return cpv.line(moretabs) << std::setw(4) << offset << ": ("
					<< std::setw(3) << (int) opcode << ") " << OPCODES[opcode]
					<< " ";
		}
	};

	inline void visitVersion(Magic magic, u2 minor, u2 major) {
		cv.visitVersion(magic, minor, major);

		line() << "Version = minor: " << minor << ", major: " << major
				<< std::endl;
	}

	inline void visitConstPool(ConstPool& cp) {
		cv.visitConstPool(cp);

		this->cp = cp;

		static const char* ConstNames[] = { "**** 0 ****", // 0
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

		for (u4 i = 1; i < cp.entries.size(); i++) {
			const ConstPool::Entry* entry = &cp.entries[i];

			line() << "#" << i << " [" << ConstNames[entry->tag] << "]: ";

			switch (entry->tag) {
				case CONSTANT_Class:
					os << cp.getClazzName(i) << "#" << entry->clazz.name_index;
					break;
				case CONSTANT_Fieldref:
				case CONSTANT_Methodref:
				case CONSTANT_InterfaceMethodref: {
					std::string clazzName, name, desc;
					cp.getMemberRef(i, &clazzName, &name, &desc, entry->tag);

					os << clazzName << "#" << entry->memberref.class_index
							<< "." << name << ":" << desc << "#"
							<< entry->memberref.name_and_type_index;
					break;
				}
				case CONSTANT_String:
					os << cp.getUtf8(entry->s.string_index) << "#"
							<< entry->s.string_index;
					break;
				case CONSTANT_Integer:
					os << entry->i.value;
					break;
				case CONSTANT_Float:
					os << entry->f.value;
					break;
				case CONSTANT_Long:
					os << entry->l.high_bytes << " " << entry->l.low_bytes;
					i++;
					break;
				case CONSTANT_Double:
					os << entry->d.high_bytes << " " << entry->d.low_bytes;
					i++;
					break;
				case CONSTANT_NameAndType:
					os << "#" << entry->nameandtype.name_index << ".#"
							<< entry->nameandtype.descriptor_index;
					break;
				case CONSTANT_Utf8:
					os << entry->utf8.str;
					break;
				case CONSTANT_MethodHandle:
					os << entry->methodhandle.reference_kind << " #"
							<< entry->methodhandle.reference_index;
					break;
				case CONSTANT_MethodType:
					os << "#" << entry->methodtype.descriptor_index;
					break;
				case CONSTANT_InvokeDynamic:
					os << "#"
							<< entry->invokedynamic.bootstrap_method_attr_index
							<< ".#" << entry->invokedynamic.name_and_type_index;
					break;
			}

			os << std::endl;
		}
	}

	void visitThis(u2 accessFlags, u2 thisClassIndex, u2 superClassIndex) {
		cv.visitThis(accessFlags, thisClassIndex, superClassIndex);

		line() << "accessFlags: " << accessFlags << std::endl;
		line() << "thisClassIndex: " << cp.getClazzName(thisClassIndex) << "#"
				<< thisClassIndex << std::endl;

		if (superClassIndex != 0) {
			line() << "superClassIndex: " << cp.getClazzName(superClassIndex)
					<< "#" << superClassIndex << std::endl;
		}
	}

	void visitInterface(u2 interIndex) {
		cv.visitInterface(interIndex);

		line() << "Interface '" << cp.getClazzName(interIndex) << "'#"
				<< interIndex << std::endl;
	}

	Field visitField(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		typename TForward::Field fv = cv.visitField(accessFlags, nameIndex,
				descIndex);

		line() << "Field " << cp.getUtf8(nameIndex) << ": "
				<< AccessFlagsPrinter(accessFlags) << " #" << nameIndex << ": "
				<< cp.getUtf8(descIndex) << "#" << descIndex << std::endl;

		return Field(fv, *this);
	}

	Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		typename TForward::Method mv = cv.visitMethod(accessFlags, nameIndex,
				descIndex);

		line() << "+Method " << AccessFlagsPrinter(accessFlags) << " "
				<< cp.getUtf8(nameIndex) << ": " << " #" << nameIndex << ": "
				<< cp.getUtf8(descIndex) << "#" << descIndex << std::endl;

		return Method(mv, *this);
	}

	inline void visitSourceFile(u2 nameIndex, u2 sourceFileIndex) {
		cv.visitSourceFile(nameIndex, sourceFileIndex);

		const std::string& sourceFileName = cp.getUtf8(sourceFileIndex);
		line() << "Source file: " << sourceFileName << "#" << sourceFileIndex
				<< std::endl;
	}

	inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
		cv.visitAttr(nameIndex, len, data);

		line() << "Class attribute unknown '" << cp.getUtf8(nameIndex) << "' #"
				<< nameIndex << "[" << len << "]" << std::endl;
	}

private:

	inline void inc() {
		tabs++;
	}

	inline void dec() {
		tabs--;
	}

	inline std::ostream& line(int moretabs = 0) {
		return tab(os, moretabs);
	}

	inline std::ostream& tab(std::ostream& os, int moretabs = 0) {
		for (int _ii = 0; _ii < tabs + moretabs; _ii++) {
			os << "  ";
		}

		return os;
	}

	TForward& cv;

	ConstPool cp;
	std::ostream& os;
	int tabs;
};

}

#endif
