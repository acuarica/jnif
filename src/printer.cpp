#include "jnif.hpp"

#include <ostream>
#include <iomanip>

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

struct ClassPrinter {

	static const char* OPCODES[];

	static const char* ConstNames[];

	ClassPrinter(ClassFile& cf, ostream& os, int tabs) :
			cf(cf), os(os), tabs(tabs) {
	}

	void print() {
		line() << "Version = minor: " << cf.minor << ", major: " << cf.major
				<< endl;

		printConstPool(cf.cp);

		line() << "accessFlags: " << cf.accessFlags << endl;
		line() << "thisClassIndex: " << cf.cp.getClazzName(cf.thisClassIndex)
				<< "#" << cf.thisClassIndex << endl;

		if (cf.superClassIndex != 0) {
			line() << "superClassIndex: "
					<< cf.cp.getClazzName(cf.superClassIndex) << "#"
					<< cf.superClassIndex << endl;
		}

		for (u2 interIndex : cf.interfaces) {
			line() << "Interface '" << cf.cp.getClazzName(interIndex) << "'#"
					<< interIndex << endl;
		}

		for (Field* fp : cf.fields) {
			Field& f = *fp;
			line() << "Field " << cf.cp.getUtf8(f.nameIndex) << ": "
					<< AccessFlagsPrinter(f.accessFlags) << " #" << f.nameIndex
					<< ": " << cf.cp.getUtf8(f.descIndex) << "#" << f.descIndex
					<< endl;

			printAttrs(f);
		}

		for (Method* mp : cf.methods) {
			Method& m = *mp;

			line() << "+Method " << AccessFlagsPrinter(m.accessFlags) << " "
					<< cf.cp.getUtf8(m.nameIndex) << ": " << " #" << m.nameIndex
					<< ": " << cf.cp.getUtf8(m.descIndex) << "#" << m.descIndex
					<< endl;

			printAttrs(m);
		}

		printAttrs(cf);
	}

	void printConstPool(ConstPool& cp) {
		for (u4 i = 1; i < cp.entries.size(); i++) {
			const ConstPoolEntry* entry = &cp.entries[i];

			line() << "#" << i << " [" << ConstNames[entry->tag] << "]: ";

			switch (entry->tag) {
				case CONSTANT_Class:
					os << cp.getClazzName(i) << "#" << entry->clazz.name_index;
					break;
				case CONSTANT_Fieldref:
				case CONSTANT_Methodref:
				case CONSTANT_InterfaceMethodref: {
					string clazzName, name, desc;
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

			os << endl;
		}
	}

	void printAttrs(Attrs& attrs) {
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
					printCode((CodeAttr&) attr);
					break;
				case ATTR_EXCEPTIONS:
					printExceptions((ExceptionsAttr&) attr);
					break;
				case ATTR_LVT:
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
		const string& sourceFileName = cf.cp.getUtf8(attr.sourceFileIndex);
		line() << "Source file: " << sourceFileName << "#"
				<< attr.sourceFileIndex << endl;
	}

	void printUnknown(UnknownAttr& attr) {
		const string& attrName = cf.cp.getUtf8(attr.nameIndex);

		line() << "  Attribute unknown '" << attrName << "' # "
				<< attr.nameIndex << "[" << attr.len << "]" << endl;

	}

	void printCode(CodeAttr& c) {
		line(1) << "maxStack: " << c.maxStack << ", maxLocals: " << c.maxLocals
				<< endl;
		inc();

		for (Inst& inst : c.instList) {
			printInst(inst);
		}

		for (CodeExceptionEntry& e : c.exceptions) {
			line(1) << "exception entry: startpc: " << e.startpc << ", endpc: "
					<< e.endpc << ", handlerpc: " << e.handlerpc
					<< ", catchtype: " << e.catchtype << endl;
		}

		dec();
	}

	void printInst(Inst& inst) {
		int offset = -1;

		line() << setw(4) << offset << ": (" << setw(3) << (int) inst.opcode
				<< ") " << OPCODES[inst.opcode] << " ";

		ostream& instos = os;

		switch (inst.kind) {
			case KIND_ZERO:
				break;
			case KIND_BIPUSH:
				instos << int(inst.push.value) << endl;
				break;
			case KIND_SIPUSH:
				instos << int(inst.push.value) << endl;
				break;
			case KIND_LDC:
				instos << inst.ldc.valueIndex << endl;
				break;
			case KIND_VAR:
				instos << inst.var.lvindex << endl;
				break;
			case KIND_IINC:
				instos << int(inst.iinc.index) << " " << int(inst.iinc.value)
						<< endl;
				break;
			case KIND_JUMP:
				instos << offset + inst.jump.label << endl;
				break;
			case KIND_TABLESWITCH:
				instos << inst.ts.def << " " << " " << inst.ts.low << " "
						<< inst.ts.high << ":";

				for (int i = 0; i < inst.ts.high - inst.ts.low + 1; i++) {
					os << " " << inst.ts.targets[i];
				}

				instos << endl;

				break;
			case KIND_LOOKUPSWITCH:
				instos << inst.ls.defbyte << " " << inst.ls.npairs << ":";

				for (u4 i = 0; i < inst.ls.npairs; i++) {
					instos << " " << inst.ls.keys[i] << " -> "
							<< inst.ls.targets[i];
				}

				instos << endl;
				break;
			case KIND_FIELD: {
				string className, name, desc;
				cf.cp.getMemberRef(inst.field.fieldRefIndex, &className, &name,
						&desc, CONSTANT_Fieldref);

				instos << className << name << desc << endl;

				break;
			}
			case KIND_INVOKE: {
				string className, name, desc;
				cf.cp.getMemberRef(inst.invoke.methodRefIndex, &className,
						&name, &desc, CONSTANT_Methodref);

				instos << className << "." << name << ": " << desc << endl;

				break;
			}
			case KIND_INVOKEINTERFACE: {
				string className, name, desc;
				cf.cp.getMemberRef(inst.invokeinterface.interMethodRefIndex,
						&className, &name, &desc, CONSTANT_Methodref);

				instos << className << "." << name << ": " << desc << "("
						<< inst.invokeinterface.count << ")" << endl;
				break;
			}
			case KIND_INVOKEDYNAMIC:
				EXCEPTION("FrParseInvokeDynamicInstr not implemented");
				break;
			case KIND_TYPE: {
				string className = cf.cp.getClazzName(inst.type.classIndex);

				instos << className << endl;

				break;
			}
			case KIND_NEWARRAY:
				instos << int(inst.newarray.atype) << endl;

				break;
			case KIND_MULTIARRAY: {
				string className = cf.cp.getClazzName(
						inst.multiarray.classIndex);

				instos << className << " " << inst.multiarray.dims << endl;

				break;
			}
			case KIND_PARSE4TODO:
				EXCEPTION("FrParse4__TODO__Instr not implemented");
				break;
			case KIND_RESERVED:
				EXCEPTION("FrParseReservedInstr not implemented");
				break;
		}
	}

	void printExceptions(ExceptionsAttr& attr) {
		for (u4 i = 0; i < attr.es.size(); i++) {
			u2 exceptionIndex = attr.es[i];

			const string& exceptionName = cf.cp.getClazzName(exceptionIndex);

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
			line() << "  LocalVariableTable entry: start: " << e.startPc
					<< ", len: " << e.len << ", varNameIndex: "
					<< e.varNameIndex << ", varDescIndex: " << e.varDescIndex
					<< ", index: " << endl;
		}
	}

	void printSmt(SmtAttr& attr) {

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
