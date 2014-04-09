#include "jnif.hpp"

#include <ostream>
#include <iomanip>

#include <map>
#include <set>

#include "analysis/Cfg.hpp"
#include "analysis/Frame.hpp"
#include "analysis/DescParser.hpp"
#include "analysis/SmtBuilder.hpp"
#include "analysis/State.hpp"

namespace jnif {

class AccessFlagsPrinter {
public:

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

std::ostream& operator<<(std::ostream& os, Version version) {
	return os << version.getMajor() << "." << version.getMinor();
}

struct ClassPrinter: private ErrorManager {

	static const char* OPCODES[];

	static const char* ConstNames[];

	ClassPrinter(ClassFile& cf, std::ostream& os, int tabs) :
			cf(cf), os(os), tabs(tabs) {
	}

	void print() {
		line() << AccessFlagsPrinter(cf.accessFlags) << " class "
				<< cf.getThisClassName() << "#" << cf.thisClassIndex
				<< std::endl;

		inc();
		line() << "Version: " << cf.getVersion() << std::endl;

		inc();

		line() << "Constant Pool Items [" << ((ConstPool) cf).size() << "]"
				<< std::endl;
		inc();
		printConstPool(cf);
		dec();

		line() << "accessFlags: " << cf.accessFlags << std::endl;
		line() << "thisClassIndex: " << cf.getThisClassName() << "#"
				<< cf.thisClassIndex << std::endl;

		if (cf.superClassIndex != 0) {
			line() << "superClassIndex: " << cf.getClassName(cf.superClassIndex)
					<< "#" << cf.superClassIndex << std::endl;
		}

		for (u2 interIndex : cf.interfaces) {
			line() << "Interface '" << cf.getClassName(interIndex) << "'#"
					<< interIndex << std::endl;
		}

		for (Field& f : cf.fields) {
			line() << "Field " << cf.getUtf8(f.nameIndex) << ": "
					<< AccessFlagsPrinter(f.accessFlags) << " #" << f.nameIndex
					<< ": " << cf.getUtf8(f.descIndex) << "#" << f.descIndex
					<< std::endl;

			printAttrs(f);
		}

		for (Method& m : cf.methods) {
			line() << "+Method " << AccessFlagsPrinter(m.accessFlags) << " "
					<< cf.getUtf8(m.nameIndex) << ": " << " #" << m.nameIndex
					<< ": " << cf.getUtf8(m.descIndex) << "#" << m.descIndex
					<< std::endl;

			printAttrs(m, &m);
		}

		printAttrs(cf);

		dec();
	}

	void printConstPool(ConstPool& cp) {
		line() << "#0 [null entry]: -" << std::endl;

		for (ConstPool::Iterator it = cp.iterator(); it.hasNext(); it++) {
			ConstPool::Index i = *it;
			ConstPool::Tag tag = cp.getTag(i);

			line() << "#" << i << " [" << ConstNames[tag] << "]: ";

			const ConstPool::Entry* entry = &cp.entries[i];

			cp.get(i, [&](ConstPool::Class e) {
				os << cp.getClassName(i) << "#" << e.nameIndex;
			}, [&](ConstPool::FieldRef e) {
				std::string clazzName, name, desc;
				cp.getFieldRef(i, &clazzName, &name, &desc);

				os << clazzName << "#" << e.classIndex << "."
				<< name << ":" << desc << "#"
				<< e.nameAndTypeIndex;
			}, [&](ConstPool::MethodRef e) {
				std::string clazzName, name, desc;
				cp.getMethodRef(i, &clazzName, &name, &desc);

				os << clazzName << "#" << e.classIndex << "."
				<< name << ":" << desc << "#"
				<< e.nameAndTypeIndex;
			}, [&](ConstPool::InterMethodRef e) {
				std::string clazzName, name, desc;
				cp.getInterMethodRef(i, &clazzName, &name, &desc);

				os << clazzName << "#" << e.classIndex << "."
				<< name << ":" << desc << "#"
				<< e.nameAndTypeIndex;
			});

			switch (tag) {
				case ConstPool::CLASS:
//					os << cp.getClassName(i) << "#" << entry->clazz.nameIndex;
//					break;
				case ConstPool::FIELDREF: //{
//					string clazzName, name, desc;
//					cp.getFieldRef(i, &clazzName, &name, &desc);
//
//					os << clazzName << "#" << entry->memberref.classIndex << "."
//							<< name << ":" << desc << "#"
//							<< entry->memberref.nameAndTypeIndex;
//					break;
//				}
//
				case ConstPool::METHODREF: //{
//					string clazzName, name, desc;
//					cp.getMethodRef(i, &clazzName, &name, &desc);
//
//					os << clazzName << "#" << entry->memberref.classIndex << "."
//							<< name << ":" << desc << "#"
//							<< entry->memberref.nameAndTypeIndex;
//					break;
//				}
//
				case ConstPool::INTERFACEMETHODREF: //{
//					string clazzName, name, desc;
//					cp.getInterMethodRef(i, &clazzName, &name, &desc);
//
//					os << clazzName << "#" << entry->memberref.classIndex << "."
//							<< name << ":" << desc << "#"
//							<< entry->memberref.nameAndTypeIndex;
					break;
//				}
				case ConstPool::STRING:
					os << cp.getUtf8(entry->s.stringIndex) << "#"
							<< entry->s.stringIndex;
					break;
				case ConstPool::INTEGER:
					os << entry->i.value;
					break;
				case ConstPool::FLOAT:
					os << entry->f.value;
					break;
				case ConstPool::LONG:
					os << cp.getLong(i);
					//i++;
					break;
				case ConstPool::DOUBLE:
					os << cp.getDouble(i);
					//i++;
					break;
				case ConstPool::NAMEANDTYPE:
					os << "#" << entry->nameandtype.nameIndex << ".#"
							<< entry->nameandtype.descriptorIndex;
					break;
				case ConstPool::UTF8:
					os << entry->utf8.str;
					break;
				case ConstPool::METHODHANDLE:
					os << entry->methodhandle.referenceKind << " #"
							<< entry->methodhandle.referenceIndex;
					break;
				case ConstPool::METHODTYPE:
					os << "#" << entry->methodtype.descriptorIndex;
					break;
				case ConstPool::INVOKEDYNAMIC:
					os << "#" << entry->invokedynamic.bootstrapMethodAttrIndex
							<< ".#" << entry->invokedynamic.nameAndTypeIndex;
					break;
			}

			os << std::endl;
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
		const std::string& sourceFileName = cf.getUtf8(attr.sourceFileIndex);
		line() << "Source file: " << sourceFileName << "#"
				<< attr.sourceFileIndex << std::endl;
	}

	void printUnknown(UnknownAttr& attr) {
		const std::string& attrName = cf.getUtf8(attr.nameIndex);

		line() << "  Attribute unknown '" << attrName << "' # "
				<< attr.nameIndex << "[" << attr.len << "]" << std::endl;

	}

	void printCode(CodeAttr& c, Method* m) {
		line(1) << "maxStack: " << c.maxStack << ", maxLocals: " << c.maxLocals
				<< std::endl;

		line(1) << "Code length: " << c.codeLen << std::endl;

		inc();

		int id = 1;
		for (Inst* inst : c.instList) {
			if (inst->kind == KIND_LABEL) {
				inst->label.id = id;
				id++;
			}
		}

		H h;

		int lvindex = [&]() {
			if (m->accessFlags & ACC_STATIC) {
				return 0;
			} else {
				h.setRefVar(0); // this argument
				return 1;
			}
		}();

		const char* methodDesc = cf.getUtf8(m->descIndex);
		std::vector<Type> argsType;
		DescParser::parseMethodDesc(methodDesc, &argsType);

		for (Type t : argsType) {
			h.setVar(lvindex, t);
			lvindex++;
		}

		H initState = h;

		for (Inst* inst : c.instList) {
			printInst(*inst);
			os << std::endl;
			//SmtBuilder::computeFrame(*inst, cf, h);
			//os << std::setw(10);
			//h.print(os);
		}

		ControlFlowGraph cfg(c.instList);
		printCfg(cfg);

		std::vector<State> states(cfg.nodeCount());
		initState.valid = true;
		states[cfg.entry._index] = {initState, initState};

		auto to = *cfg.outEdges(cfg.entry).begin();
		SmtBuilder::computeState(to, initState, cfg, states, c.instList, os,
				cf);

		for (auto nkey : cfg) {
			InstList::iterator b;
			InstList::iterator e;
			std::string name;
			std::tie(b, e, name) = cfg.getNode(nkey);

			os << name << ": ";
			State& s = states[nkey._index];
			s.in.print(os);
			s.out.print(os);
			os << std::endl;
		}

		for (CodeExceptionEntry& e : c.exceptions) {
			line(1) << "exception entry: startpc: " << e.startpc->label.id
					<< ", endpc: " << e.endpc->label.offset << ", handlerpc: "
					<< e.handlerpc->label.offset << ", catchtype: "
					<< e.catchtype << std::endl;
		}

		printAttrs(c.attrs);

		dec();
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
			os << std::endl;

			for (auto it = b; it != e; it++) {
				Inst* inst = *it;
				printInst(*inst);
				os << std::endl;
			}
		}

		os << std::endl;
	}

	void printInst(Inst& inst) {
		int offset = inst._offset;

		if (inst.kind == KIND_LABEL) {
			os << "   label: " << inst.label.id;
			return;
		}

		line() << std::setw(4) << offset << ": (" << std::setw(3)
				<< (int) inst.opcode << ") " << OPCODES[inst.opcode] << " ";

		std::ostream& instos = os;

		switch (inst.kind) {
			case KIND_ZERO:
				//instos << std::endl;
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

				//instos << std::endl;
				break;
			case KIND_FIELD: {
				std::string className, name, desc;
				cf.getFieldRef(inst.field.fieldRefIndex, &className, &name,
						&desc);

				instos << className << name << desc;

				break;
			}
			case KIND_INVOKE: {
				std::string className, name, desc;
				cf.getMethodRef(inst.invoke.methodRefIndex, &className, &name,
						&desc);

				instos << className << "." << name << ": " << desc;

				break;
			}
			case KIND_INVOKEINTERFACE: {
				std::string className, name, desc;
				cf.getInterMethodRef(inst.invokeinterface.interMethodRefIndex,
						&className, &name, &desc);

				instos << className << "." << name << ": " << desc << "("
						<< inst.invokeinterface.count << ")";
				break;
			}
			case KIND_INVOKEDYNAMIC:
				raise("FrParseInvokeDynamicInstr not implemented");
				break;
			case KIND_TYPE: {
				std::string className = cf.getClassName(inst.type.classIndex);

				instos << className;

				break;
			}
			case KIND_NEWARRAY:
				instos << int(inst.newarray.atype);

				break;
			case KIND_MULTIARRAY: {
				std::string className = cf.getClassName(
						inst.multiarray.classIndex);

				instos << className << " " << inst.multiarray.dims;

				break;
			}
			case KIND_PARSE4TODO:
				raise("FrParse4__TODO__Instr not implemented");
				break;
			case KIND_RESERVED:
				raise("FrParseReservedInstr not implemented");
				break;
			default:
				raise("should not arrive here!");
		}
	}

	void printExceptions(ExceptionsAttr& attr) {
		for (u4 i = 0; i < attr.es.size(); i++) {
			u2 exceptionIndex = attr.es[i];

			const std::string& exceptionName = cf.getClassName(exceptionIndex);

			line() << "  Exceptions entry: '" << exceptionName << "'#"
					<< exceptionIndex << std::endl;
		}
	}

	void printLnt(LntAttr& attr) {
		for (LntAttr::LnEntry e : attr.lnt) {
			line() << "  LocalNumberTable entry: startpc: " << e.startpc
					<< ", lineno: " << e.lineno << std::endl;
		}
	}

	void printLvt(LvtAttr& attr) {
		for (LvtAttr::LvEntry e : attr.lvt) {
			line() << "  LocalVariable(or Type)Table  entry: start: "
					<< e.startPc << ", len: " << e.len << ", varNameIndex: "
					<< e.varNameIndex << ", varDescIndex: " << e.varDescIndex
					<< ", index: " << std::endl;
		}
	}

	void printSmt(SmtAttr& smt) {

		auto parseTs =
				[&](std::vector<Type> locs) {
					line(2) << "["<<locs.size()<<"] ";
					for (u1 i = 0; i < locs.size(); i++) {
						Type& vt = locs[i];
						Type::Tag tag = vt.tag;

						switch (tag) {
							case Type::TYPE_TOP:
							os << "top" << " | ";
							break;
							case Type::TYPE_INTEGER:
							os << "integer" << " | ";
							break;
							case Type::TYPE_FLOAT :
							os << "float" << " | ";
							break;
							case Type::TYPE_LONG :
							os << "long" << " | ";
							break;
							case Type::TYPE_DOUBLE:
							os << "double" << " | ";
							break;
							case Type::TYPE_NULL :
							os << "null" << " | ";
							break;
							case Type::TYPE_UNINITTHIS :
							os << "UninitializedThis" << " | ";
							break;
							case Type::TYPE_OBJECT:
							os << "Object: cpindex = " << vt.object.cindex << " | ";
							break;
							case Type::TYPE_UNINIT: {
								os << "Uninitialized: offset = " << vt.uninit.offset << " | ";
								break;
							}
							default:
							raise("invalid type in printing!");
						}
					}

					os << std::endl;
				};

		line() << "Stack Map Table: " << std::endl;

		int toff = -1;
		for (SmtAttr::Entry& e : smt.entries) {
			line(1) << "frame type (" << e.frameType << ") ";

			u1 frameType = e.frameType;

			if (0 <= frameType && frameType <= 63) {
				toff += frameType + 1;
				os << "offset = " << toff << " ";

				os << "same frame" << std::endl;
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
						<< std::endl;
				parseTs(e.same_locals_1_stack_item_frame_extended.stack);
			} else if (248 <= frameType && frameType <= 250) {
				toff += e.chop_frame.offset_delta + 1;
				os << "offset = " << toff << " ";

				os << "chop_frame, ";
				os << "offset_delta = " << e.chop_frame.offset_delta
						<< std::endl;
			} else if (frameType == 251) {
				toff += e.same_frame_extended.offset_delta + 1;
				os << "offset = " << toff << " ";

				os << "same_frame_extended. ";
				os << e.same_frame_extended.offset_delta << std::endl;
			} else if (252 <= frameType && frameType <= 254) {
				toff += e.append_frame.offset_delta + 1;
				os << "offset = " << toff << " ";

				os << "append_frame, ";
				os << "offset_delta = " << e.append_frame.offset_delta
						<< std::endl;
				parseTs(e.append_frame.locals);
			} else if (frameType == 255) {
				toff += e.full_frame.offset_delta + 1;
				os << "offset = " << toff << " ";

				os << "full_frame. ";
				os << e.full_frame.offset_delta << std::endl;
				parseTs(e.full_frame.locals);
				parseTs(e.full_frame.stack);
			}
		}
	}

private:

	ClassFile& cf;

	std::ostream& os;

	int tabs;

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

std::ostream& operator<<(std::ostream& os, ClassFile& cf) {
	ClassPrinter cp(cf, os, 0);
	cp.print();

	return os;
}

}
