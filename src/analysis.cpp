/*
 * analysis.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: luigi
 */
#include "jnif.hpp"

using namespace std;

namespace jnif {

struct ControlFlowGraphBuilder: private Error {

	static void setBranchTargets(InstList& instList) {
		for (Inst* inst : instList) {
			if (inst->isJump()) {
				inst->jump.label2->label.isBranchTarget = true;
			} else if (inst->isTableSwitch()) {
				inst->ts.def->label.isBranchTarget = true;
				for (Inst* target : inst->ts.targets) {
					target->label.isBranchTarget = true;
				}
			} else if (inst->isLookupSwitch()) {
				inst->ls.defbyte->label.isBranchTarget = true;
				for (Inst* target : inst->ls.targets) {
					target->label.isBranchTarget = true;
				}
			}
		}
	}

	static void buildBasicBlocks(InstList& instList, ControlFlowGraph& cfg) {
		setBranchTargets(instList);

		int bbid = 0;
		auto beginBb = instList.begin();

		auto getBasicBlockName = [&](int bbid) {
			stringstream ss;
			ss << "BB" << bbid;

			return ss.str();
		};

		auto addBasicBlock2 = [&](InstList::iterator eit) {
			if (beginBb != eit) {
				string name = getBasicBlockName(bbid);
				cfg.addBasicBlock(beginBb, eit, name);

				beginBb = eit;
				bbid++;
			}
		};

		for (auto it = instList.begin(); it != instList.end(); it++) {
			Inst* inst = *it;

			if (inst->isLabel()
					&& (inst->label.isBranchTarget || inst->label.isTryStart)) {
				addBasicBlock2(it);
			}

			if (inst->isBranch()) {
				auto eit = it;
				eit++;
				addBasicBlock2(eit);
			}

			if (inst->isExit()) {
				auto eit = it;
				eit++;
				addBasicBlock2(eit);
			}
		}
	}

	static BasicBlock* findBasicBlockOfLabel(int labelId, InstList& instList,
			ControlFlowGraph& cfg) {
		for (BasicBlock* bb : cfg) {
			if (bb->start == instList.end()) {
				assert(bb->name == "Entry" || bb->name == "Exit", "");
				assert(bb->exit == instList.end(), "");
				continue;
			}

			Inst* inst = *bb->start;
			if (inst->kind == KIND_LABEL && inst->label.id == labelId) {
				return bb;
			}
		}

		raise("se pudrio el chorran");
	}

	static void buildCfg(InstList& instList, ControlFlowGraph& cfg) {
		buildBasicBlocks(instList, cfg);

		auto addTarget2 = [&] (BasicBlock* bb, Inst* inst) {
			assert(inst->isLabel(), "Expected label instruction");
			int labelId = inst->label.id;
			BasicBlock* tbbid = findBasicBlockOfLabel(labelId, instList,
					cfg);
			bb->addTarget(tbbid);
		};

		for (BasicBlock* bb : cfg) {
			if (bb->start == instList.end()) {
				assert(bb->name == "Entry" || bb->name == "Exit", "");
				assert(bb->exit == instList.end(), "");
				continue;
			}

			auto e = bb->exit;
			e--;
			assert(e != instList.end(), "");

			Inst* last = *e;

			if (bb->start == instList.begin()) {
				cfg.entry->addTarget(bb);
			}

			if (last->isJump()) {
				addTarget2(bb, last->jump.label2);

				if (last->opcode != OPCODE_goto) {
					assert(bb->next != nullptr, "next bb is null");
					bb->addTarget(bb->next);
				}
			} else if (last->isTableSwitch()) {
				addTarget2(bb, last->ts.def);

				for (Inst* target : last->ts.targets) {
					addTarget2(bb, target);
				}
			} else if (last->isLookupSwitch()) {
				addTarget2(bb, last->ls.defbyte);

				for (Inst* target : last->ls.targets) {
					addTarget2(bb, target);
				}
			} else if (last->isExit()) {
				bb->addTarget(cfg.exit);
			} else {
				assert(bb->next != nullptr, "next bb is null");
				bb->addTarget(bb->next);
			}
		}
	}
};

ControlFlowGraph::ControlFlowGraph(InstList& instList) :
		entry(addConstBb(instList, "Entry")), exit(addConstBb(instList, "Exit")) {
	ControlFlowGraphBuilder::buildCfg(instList, *this);
}

struct DescParser: protected Error {

	static Type parseConstClass(const std::string& className) {
		Error::assert(!className.empty(), "Invalid string class");

		if (className[0] == '[') {
			const char* classNamePtr = className.c_str();
			Type arrayType = parseFieldDesc(classNamePtr);
			Error::assert(arrayType.isArray(), "Not an array: ", arrayType);
			return arrayType;
		} else {
			return Type::objectType(className);
		}
	}

	static Type parseFieldDesc(const char*& fieldDesc) {
		const char* originalFieldDesc = fieldDesc;

		int dims = 0;
		while (*fieldDesc == '[') {
			check(*fieldDesc != '\0',
					"Reach end of string while searching for array. Field descriptor: ",
					originalFieldDesc);
			fieldDesc++;
			dims++;
		}

		check(*fieldDesc != '\0', "");

		auto parseBaseType = [&] () {
			switch (*fieldDesc) {
				case 'Z':
				return Type::booleanType();
				case 'B':
				return Type::byteType();
				case 'C':
				return Type::charType();
				case 'S':
				return Type::shortType();
				case 'I':
				return Type::intType();
				case 'D':
				return Type::doubleType();
				case 'F':
				return Type::floatType();
				case 'J':
				return Type::longType();
				case 'L': {
					fieldDesc++;

					const char* classNameStart = fieldDesc;
					int len = 0;
					while (*fieldDesc != ';') {
						check(*fieldDesc != '\0', "");
						fieldDesc++;
						len++;
					}

					string className (classNameStart, len);
					return Type::objectType(className);
				}
				default:
				raise("Invalid field desc ", originalFieldDesc);
			}};

		Type t = [&]() {
			Type baseType = parseBaseType();
			if (dims == 0) {
				return baseType;
			} else {
				return Type::arrayType(baseType, dims);
			}
		}();

		fieldDesc++;

		return t;
	}

	static Type parseMethodDesc(const char* methodDesc,
			vector<Type>* argsType) {
		const char* originalMethodDesc = methodDesc;

		check(*methodDesc == '(', "Invalid beginning of method descriptor: ",
				originalMethodDesc);
		methodDesc++;

		while (*methodDesc != ')') {
			check(*methodDesc != '\0', "Reached end of string: ",
					originalMethodDesc);

			Type t = parseFieldDesc(methodDesc);
			argsType->push_back(t);
		}

		check(*methodDesc == ')', "Expected ')' in method descriptor: ",
				originalMethodDesc);
		methodDesc++;

		check(*methodDesc != '\0', "Reached end of string: ",
				originalMethodDesc);

		Type returnType = [&]() {
			if (*methodDesc == 'V') {
				methodDesc++;
				return Type::voidType();
			} else {
				return parseFieldDesc(methodDesc);
			}
		}();

		check(*methodDesc == '\0', "Expected end of string: %s",
				originalMethodDesc);

		return returnType;
	}
};

class SmtBuilder: private DescParser {
public:

	static void computeState(BasicBlock& bb, Frame& how, InstList& instList,
			ClassFile& cf, CodeAttr* code, Type pushType = Type::topType()) {
		if (bb.start == instList.end()) {
			assert(bb.name == "Exit" && bb.exit == instList.end(), "");
			return;
		}

		assert(how.valid, "how valid");
		assert(bb.in.valid == bb.out.valid, "");

		bool change;
		if (!bb.in.valid) {
			bb.in = how;
			bb.out = bb.in;
			change = true;
		} else {
			change = bb.in.join(how);
		}

		if (change) {
			if ((*bb.start)->isLabel()) {
				for (auto ex : code->exceptions) {
					if (ex.startpc->label.id == (*bb.start)->label.id) {
						BasicBlock* handlerBb =
								ControlFlowGraphBuilder::findBasicBlockOfLabel(
										ex.handlerpc->label.id, instList,
										*bb.cfg);

						Type exType = [&]() {
							if (ex.catchtype != ConstPool::NULLENTRY) {
								const string& className = cf.getClassName(
										ex.catchtype);
								return parseConstClass(className);
							} else {
								return Type::objectType("java/lang/Throwable");
							}
						}();

						Frame frame = bb.in;
						frame.clearStack();
						frame.push(exType);

						computeState(*handlerBb, frame, instList, cf, code,
								exType);
					}
				}
			}

//			if (!pushType.isTop()) {
////				Frame& frame = bb.out;
//				//			frame.push(pushType);
//
////				bb.in.push(pushType);
//				bb.out.push(pushType);
//			}

			for (auto it = bb.start; it != bb.exit; it++) {
				Inst* inst = *it;
				computeFrame(*inst, cf, bb.out);
			}

			Frame h = bb.out;

			for (BasicBlock* nid : bb) {
				computeState(*nid, h, instList, cf, code);
			}
		}
	}

	static void aload(u4 lvindex, Frame& h) {
		const Type& type = h.getVar(lvindex);
		check(type.isObject() || type.isNull(), "Bad ref var at index[",
				lvindex, "]: ", type, " @ frame: ", h);
		h.pushType(type);
	}

	static void invoke(const string& desc, bool popThis, Frame& h) {
		const char* d = desc.c_str();
		vector<Type> argsType;
		Type returnType = parseMethodDesc(d, &argsType);

		for (int i = argsType.size() - 1; i >= 0; i--) {
			const Type& argType = argsType[i];
			assert(argType.isOneOrTwoWord(), "Invalid arg type in method");
//				if (argType.isOneWord()) {
//					h.popOneWord();
//				} else {
//					h.popTwoWord();
//				}
			h.popType(argType);
		}

		if (popThis) {
			h.popRef();
		}

		if (!returnType.isVoid()) {
			assert(returnType.isOneOrTwoWord(), "Ret type: ", returnType);
			//assert(!returnType.isTwoWord(), "Two word in return type");
			h.pushType(returnType);
			//h.push(returnType);
		}
	}
	static void invokeMethod(u2 methodRefIndex, bool popThis, Frame& h,
			const ConstPool& cp) {
		string className, name, desc;
		cp.getMethodRef(methodRefIndex, &className, &name, &desc);
		invoke(desc, popThis, h);
	}

	static inline void invokeInterface(u2 interMethodRefIndex, Frame& h,
			const ConstPool& cp) {
		string className, name, desc;
		cp.getInterMethodRef(interMethodRefIndex, &className, &name, &desc);
		invoke(desc, true, h);
	}

	static inline void xastore(Frame& h) {
		h.popInt();
		h.popArray();
	}

	static inline Type getArrayBaseType(int atype) {
		switch (atype) {
			case NEWARRAYTYPE_BOOLEAN:
				return Type::booleanType();
			case NEWARRAYTYPE_CHAR:
				return Type::charType();
			case NEWARRAYTYPE_BYTE:
				return Type::byteType();
			case NEWARRAYTYPE_SHORT:
				return Type::shortType();
			case NEWARRAYTYPE_INT:
				return Type::intType();
			case NEWARRAYTYPE_FLOAT:
				return Type::floatType();
			case NEWARRAYTYPE_LONG:
				return Type::longType();
			case NEWARRAYTYPE_DOUBLE:
				return Type::doubleType();
		}

		raise("invalid atype: ", atype);
	}

	static Type fieldType(const Inst& inst, const ConstPool& cp) {
		string className, name, desc;
		cp.getFieldRef(inst.field.fieldRefIndex, &className, &name, &desc);

		const char* d = desc.c_str();
		auto t = parseFieldDesc(d);

		//assert(!t.isTwoWord(), "Two word in field");
		return t;
	}

	static void multianewarray(const Inst& inst, const ConstPool& cp,
			Frame& h) {
		u1 dims = inst.multiarray.dims;
		check(dims >= 1, "invalid dims: ", dims);

		for (int i = 0; i < dims; i++) {
			h.popInt();
		}

		string arrayClassName = cp.getClassName(inst.multiarray.classIndex);
		const char* d = arrayClassName.c_str();
		Type arrayType = parseFieldDesc(d);

		h.pushType(arrayType);
	}

	static void computeFrame(Inst& inst, const ConstPool& cp, Frame& h) {
		auto istore = [&](int lvindex) {
			h.popInt();
			h.setIntVar(lvindex);
		};
		auto lstore = [&](int lvindex) {
			h.popLong();
			h.setLongVar(lvindex);
		};
		auto fstore = [&](int lvindex) {
			h.popFloat();
			h.setFloatVar(lvindex);
		};
		auto dstore = [&](int lvindex) {
			h.popDouble();
			h.setDoubleVar(lvindex);
		};
		auto astore = [&](int lvindex) {
			Type refType = h.popRef();
			h.setRefVar(lvindex, refType);
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
			case OPCODE_ldc_w: {
				ConstTag tag = cp.getTag(inst.ldc.valueIndex);
				switch (tag) {
					case CONST_INTEGER:
						h.pushInt();
						break;
					case CONST_FLOAT:
						h.pushFloat();
						break;
					case CONST_CLASS:
						h.pushRef(cp.getClassName(inst.ldc.valueIndex));
						break;
					case CONST_STRING:
						h.pushRef("java/lang/String");
						break;
					default:
						raise("Invalid tag entry: ", tag);
				}
				break;
			}
			case OPCODE_ldc2_w: {
				ConstTag tag = cp.getTag(inst.ldc.valueIndex);
				switch (tag) {
					case CONST_LONG:
						h.pushLong();
						break;
					case CONST_DOUBLE:
						h.pushLong();
						break;
					default:
						raise("Invalid constant for ldc2_w");
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
				aload(inst.var.lvindex, h);
				break;
			case OPCODE_aload_0:
				aload(0, h);
				break;
			case OPCODE_aload_1:
				aload(1, h);
				break;
			case OPCODE_aload_2:
				aload(2, h);
				break;
			case OPCODE_aload_3:
				aload(3, h);
				break;
			case OPCODE_iaload:
			case OPCODE_baload:
			case OPCODE_caload:
			case OPCODE_saload:
				h.popInt();
				h.popArray();
				h.pushInt();
				break;
			case OPCODE_laload:
				h.popInt();
				h.popArray();
				h.pushLong();
				break;
			case OPCODE_faload:
				h.popInt();
				h.popArray();
				h.pushFloat();
				break;
			case OPCODE_daload: {
				h.popInt();
				Type arrayType = h.popArray();
				//Error::check(arrayType.isArray(), "Not array: ", arrayType);
				//Error::check(arrayType.elementType().isDouble(), "Not array: ", arrayType);
				h.pushDouble();
				break;
			}
			case OPCODE_aaload: {
				h.popInt();
				Type arrayType = h.popArray();
				if (arrayType.isNull()) {
					h.pushNull();
				} else {
					//h.pushRef(arrayType.getClassName());
					Type elementType = arrayType.elementType();
					Error::check(elementType.isObject(), "Not an object:",
							elementType);
					h.push(elementType);
				}
				break;
			}
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
			case OPCODE_bastore:
			case OPCODE_castore:
			case OPCODE_sastore:
				h.popInt();
				xastore(h);
				break;
			case OPCODE_lastore:
				h.popLong();
				xastore(h);
				break;
			case OPCODE_fastore:
				h.popFloat();
				xastore(h);
				break;
			case OPCODE_dastore:
				h.popDouble();
				xastore(h);
				break;
			case OPCODE_aastore:
				h.popRef();
				xastore(h);
				break;
			case OPCODE_pop:
				h.popOneWord();
				break;
			case OPCODE_pop2:
				h.popTwoWord();
				break;
			case OPCODE_dup: {
				auto t1 = h.popOneWord();
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
			case OPCODE_land:
			case OPCODE_lor:
			case OPCODE_lxor:
				h.popLong();
				h.popLong();
				h.pushLong();
				break;
			case OPCODE_lshl:
			case OPCODE_lshr:
			case OPCODE_lushr:
				h.popInt();
				h.popLong();
				h.pushLong();
				break;
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
				//h.pop();
				//h.pop();
				h.popLong();
				h.pushLong();
				break;
			}
			case OPCODE_dneg: {
				//h.pop();
				//h.pop();
				h.popDouble();
				h.pushDouble();
				break;
			}
			case OPCODE_iinc:
				h.setIntVar(inst.iinc.index);
				break;
			case OPCODE_i2l:
				h.popInt();
				h.pushLong();
				break;
			case OPCODE_i2f:
				h.popInt();
				h.pushFloat();
				break;
			case OPCODE_i2d:
				h.popInt();
				h.pushDouble();
				break;
			case OPCODE_l2i:
				//h.pop();
				//h.pop();
				h.popLong();
				h.pushInt();
				break;
			case OPCODE_l2f:
				//h.pop();
				//h.pop();
				h.popLong();
				h.pushFloat();
				break;
			case OPCODE_l2d:
				//h.pop();
				//h.pop();
				h.popLong();
				h.pushDouble();
				break;
			case OPCODE_f2i:
				h.popFloat();
				h.pushInt();
				break;
			case OPCODE_f2l:
				h.popFloat();
				h.pushLong();
				break;
			case OPCODE_f2d:
				h.popFloat();
				h.pushDouble();
				break;
			case OPCODE_d2i:
				//h.pop();
				//h.pop();
				h.popDouble();
				h.pushInt();
				break;
			case OPCODE_d2l:
				//h.pop();
				//h.pop();
				h.popDouble();
				h.pushLong();
				break;
			case OPCODE_d2f:
				//h.pop();
				//h.pop();
				h.popDouble();
				h.pushFloat();
				break;
			case OPCODE_i2b:
			case OPCODE_i2c:
			case OPCODE_i2s:
				h.popInt();
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
				assert(false, "jsr not implemented");
				break;
			case OPCODE_ret:
				assert(false, "jsr not implemented");
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
			case OPCODE_getstatic: {
				auto t = fieldType(inst, cp);
				h.pushType(t);
				break;
			}
			case OPCODE_putstatic: {
				auto t = fieldType(inst, cp);
				h.popType(t);
				break;
			}
			case OPCODE_getfield: {
				auto t = fieldType(inst, cp);
				h.popRef();
				h.pushType(t);
				break;
			}
			case OPCODE_putfield: {
				auto t = fieldType(inst, cp);
				h.popType(t);
				h.popRef();
				break;
			}
			case OPCODE_invokevirtual:
			case OPCODE_invokespecial:
				invokeMethod(inst.invoke.methodRefIndex, true, h, cp);
				break;
			case OPCODE_invokestatic:
				invokeMethod(inst.invoke.methodRefIndex, false, h, cp);
				break;
			case OPCODE_invokeinterface:
				invokeInterface(inst.invokeinterface.interMethodRefIndex, h,
						cp);
				break;
			case OPCODE_invokedynamic:
				raise("invoke dynamic instances not implemented");
				break;
			case OPCODE_new: {
				//h.pushRef();
				const string& className = cp.getClassName(inst.type.classIndex);
				Type t = parseConstClass(className);
				Error::check(!t.isArray(), "New with array: ", t);
				h.push(t);
				break;
			}
			case OPCODE_newarray:
				h.popInt();
				h.pushArray(getArrayBaseType(inst.newarray.atype), 1);
				break;
			case OPCODE_anewarray: {
				h.popInt();
//				string className = cp.getClassName(inst.type.classIndex);
				const string& className = cp.getClassName(inst.type.classIndex);
				Type t = parseConstClass(className);
				//			h.push(parseConstClass(className));
				//Type refType = Type::objectType(className);
				h.pushArray(t, t.getDims() + 1);
				break;
			}
			case OPCODE_arraylength:
				h.pop();
				h.pushInt();
				break;
			case OPCODE_athrow: {
				auto t = h.popRef();
				h.clearStack();
				h.push(t);
				break;
			}
			case OPCODE_checkcast: {
				h.popRef();
				const string& className = cp.getClassName(inst.type.classIndex);
				h.push(parseConstClass(className));
//				if (className[0] == '[') {
//					const char* clsname = className.c_str();
//					Type arrayType = parseFieldDesc(clsname);
//					Error::check(arrayType.isArray(), "Not an array: ",
//							arrayType);
//					h.push(arrayType);
//				} else {
//					h.pushRef(className);
//				}
				break;
			}
			case OPCODE_instanceof:
				h.popRef();
				h.pushInt();
				break;
			case OPCODE_monitorenter:
			case OPCODE_monitorexit:
				h.popRef();
				break;
			case OPCODE_wide:
				raise("wide not implemented");
				break;
			case OPCODE_multianewarray:
				multianewarray(inst, cp, h);
				break;
			case OPCODE_ifnull:
			case OPCODE_ifnonnull:
				h.pop();
				break;
			case OPCODE_goto_w:
			case OPCODE_jsr_w:
			case OPCODE_breakpoint:
			case OPCODE_impdep1:
			case OPCODE_impdep2:
				raise("goto_w, jsr_w breakpoint not implemented");
				break;
			default:
				raise("unknown opcode not implemented");
		}
	}

};

//static void appendComputedFrames(CodeAttr& code) {
//
//}

class Compute: private Error {
public:

	static void setCpIndex(Type& type, ConstPool& cp) {
		if (type.isObject()) {
//			stringstream ss;
//			for (u4 i = 0; i < type.dims; i++) {
//				ss << "[";
//			}

			const string& className = type.getClassName();

//			if (type.isArray()) {
//				//ss << "L" << className << ";";
//				ss << className;
//			} else {
//				ss << className;
//			}

			ConstPool::Index utf8index = cp.putUtf8(className.c_str());
			ConstPool::Index index = cp.addClass(utf8index);
			type.setCpIndex(index);
		}
	}

	static void setCpIndexes(Frame& frame, ConstPool& cp) {
		for (Type& type : frame.lva) {
			setCpIndex(type, cp);
		}

		for (Type& type : frame.stack) {
			setCpIndex(type, cp);
		}
	}

	static void computeFramesMethod(CodeAttr* code, Method* method,
			ClassFile* cf, ConstPool::Index* attrIndex) {

		for (auto it = code->attrs.begin(); it != code->attrs.end(); it++) {
			auto attr = *it;
			if (attr->kind == ATTR_SMT) {
				code->attrs.attrs.erase(it);
				break;
			}
		}

		if (!code->instList.hasBranches()) {
			return;
		}

		if (*attrIndex == ConstPool::NULLENTRY) {
			*attrIndex = cf->putUtf8("StackMapTable");
		}

		code->instList.setLabelIds();

		const string& methodName = cf->getUtf8(method->nameIndex);
		cerr << "computeFramesMethod: " << cf->getThisClassName() << "."
				<< methodName << cf->getUtf8(method->descIndex) << endl;

		Frame initFrame;

		u4 lvindex = [&]() {
			if (method->isStatic()) {
				return 0;
			} else {
				string className = cf->getThisClassName();
				initFrame.setRefVar(0, className); // this argument
				return 1;
			}
		}();

		const char* methodDesc = cf->getUtf8(method->descIndex);
		vector<Type> argsType;
		DescParser::parseMethodDesc(methodDesc, &argsType);

		for (Type t : argsType) {
			initFrame.setVar(&lvindex, t);
			//lvindex++;
		}

		ControlFlowGraph* cfgp = new ControlFlowGraph(code->instList);
		ControlFlowGraph& cfg = *cfgp;

		initFrame.valid = true;
		BasicBlock* bbe = cfg.entry;
		bbe->in = initFrame;
		bbe->out = initFrame;

		BasicBlock* to = *cfg.entry->begin();
		SmtBuilder::computeState(*to, initFrame, code->instList, *cf, code);

		SmtAttr* smt = new SmtAttr(*attrIndex);

		int totalOffset = -1;

		Frame* f = &cfg.entry->out;
		f->cleanTops();

		auto isSame = [] (Frame& current, Frame& prev) {
			return current.lva == prev.lva && current.stack.size() == 0;
		};

		auto isSameLocals1StackItem = [](Frame& current, Frame& prev) {
			return current.lva == prev.lva && current.stack.size() == 1;
		};

		auto isChopAppend = [] (Frame& current, Frame& prev) -> int {
			int diff = current.lva.size() - prev.lva.size();
			bool emptyStack = current.stack.size() == 0;
			bool res = diff != 0 && diff >= -3 && diff <= 3 && emptyStack;
			return res ? diff : 0;
		};

		for (BasicBlock* bb : cfg) {
			if (bb->start != code->instList.end()) {
				Inst* start = *bb->start;
				if (start->kind == KIND_LABEL
						&& (start->label.isBranchTarget
								|| start->label.isCatchHandler)) {
					Frame& current = bb->in;
					current.cleanTops();

					setCpIndexes(current, *cf);

					SmtAttr::Entry e;

					e.label = start;

					//Inst* fi = new Inst(KIND_FRAME);
					//fi->frame.frame = bb->in;
					//code->instList.insert(bb->start, fi);

					totalOffset += 1;
					int offsetDelta = start->label.offset - totalOffset;

					int diff;

					if (isSame(current, *f)) {
						if (offsetDelta <= 63) {
							e.frameType = offsetDelta;
						} else {
							e.frameType = 251;
							e.same_frame_extended.offset_delta = offsetDelta;
						}
					} else if (isSameLocals1StackItem(current, *f)) {
						if (offsetDelta <= 63) {
							e.frameType = 64 + offsetDelta;
							auto t = current.stack.front();
							e.sameLocals_1_stack_item_frame.stack.push_back(t);
						} else {
							e.frameType = 247;
							auto t = current.stack.front();
							e.same_locals_1_stack_item_frame_extended.stack.push_back(
									t);
							e.same_locals_1_stack_item_frame_extended.offset_delta =
									offsetDelta;
						}
					} else if ((diff = isChopAppend(current, *f)) != 0) {
						assert(diff != 0 && diff >= -3 && diff <= 3);

						e.frameType = 251 + diff;
						if (diff > 0) {
							e.append_frame.offset_delta = offsetDelta;
							int size = current.lva.size();
							for (int i = 0; i < diff; i++) {
								Type t = current.lva[size - diff + i];
								e.append_frame.locals.push_back(t);
							}
						} else {
							e.chop_frame.offset_delta = offsetDelta;
						}
					} else {
						e.frameType = 255;
						e.full_frame.offset_delta = offsetDelta;
						e.full_frame.locals = current.lva;

						for (auto t : current.stack) {
							e.full_frame.stack.push_back(t);
						}
					}

					totalOffset += offsetDelta;
					smt->entries.push_back(e);
					f = &bb->in;
				}
			}
		}

		code->cfg = cfgp;

		code->attrs.add(smt);
	}
};

void ClassFile::computeFrames() {

// To compute label offsets.
	computeSize();

	u2 attrIndex = ConstPool::NULLENTRY;

	for (Method* method : methods) {
		CodeAttr* code = method->codeAttr();

		if (code != nullptr) {
			Compute::computeFramesMethod(code, method, this, &attrIndex);
		}
	}
}

}
