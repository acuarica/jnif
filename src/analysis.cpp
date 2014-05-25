/*
 * analysis.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: luigi
 */
#include "jnif.hpp"
#include "jnifex.hpp"

#include <iostream>

using namespace std;

namespace jnif {

class JsrRetNotSupported {

};

class SmtBuilder {
public:

	SmtBuilder(Frame& frame, const ConstPool& cp) :
			frame(frame), cp(cp) {
	}

	static bool isAssignable(const Type& subt, const Type& supt) {
		if (subt == supt) {
			return true;
		}

		if (supt.isTop()) {
			return true;
		}

		if (subt.isNull() && supt.isObject()) {
			return true;
		}

//		if (subt.isArray() && supt.isArray()) {
//			return true;
//		}

		return false;
	}

	static String getCommonSuperClass(const String& classLeft,
			const String& classRight, IClassPath* classPath) {
		if (classLeft == "java/lang/Object"
				|| classRight == "java/lang/Object") {
			return "java/lang/Object";
		}

		return classPath->getCommonSuperClass(classLeft, classRight);
	}

	static bool assign(Type& t, Type o, IClassPath* classPath) {
//		check(isAssignable(t, o) || isAssignable(o, t), "Invalid assign type: ",
//				t, " <> ", o, " @ frame: ", *this);

		if (!isAssignable(t, o) && !isAssignable(o, t)) {
			if (t.isClass() && o.isClass()) {
				String clazz1 = t.getClassName();
				String clazz2 = o.getClassName();

				String res = getCommonSuperClass(clazz1, clazz2, classPath);

				Type superClass = Type::objectType(res);
				Error::assert((superClass == t) == (res == clazz1),
						"Invalid super class: ", superClass, t, o);

				if (superClass == t) {
					return false;
				}

				t = superClass;
				return true;
			}

			if (t.isArray() && o.isArray()) {
				if (t.getDims() != o.getDims()) {
					t = Type::objectType("java/lang/Object");
					return true;
				}

				Type st = t.stripArrayType();
				Type so = o.stripArrayType();

				//bool change =
				assign(st, so, classPath);
//				Error::assert(change, "Assigning types between ", t,
//						" (with stripped array type ", st, ") and ", o,
//						" (with stripped array type ", so,
//						") should have change the assign result.");

//				Error::assert(!st.isTop(), "Assigning types between ", t,
//						" and ", o,
//						" should have not change assign result to Top.");
				if (st.isTop()) {
					t = Type::objectType("java/lang/Object");
					return true;
				}

				t = Type::arrayType(st, t.getDims());
				return true;
			}

			if ((t.isClass() && o.isArray()) || (t.isArray() && o.isClass())) {
				t = Type::objectType("java/lang/Object");
				return true;
			}

//			Error::raise("We arrived here, and we returning top: ", t, " and ",
//					o);

			t = Type::topType();
			return true;
		}

		if (isAssignable(t, o)) {
			if (t == o) {
				return false;
			}

			t = o;
			return true;
		}

		Error::assert(isAssignable(o, t), "Invalid assign type: ", t, " <> ",
				o);

		return false;
	}

	static bool join(Frame& frame, Frame& how, IClassPath* classPath) {
		//cerr << frame << " join " << how << endl;

		Error::check(frame.stack.size() == how.stack.size(),
				"Different stack sizes: ", frame.stack.size(), " != ",
				how.stack.size(), ": #", frame, " != #", how);

		if (frame.lva.size() < how.lva.size()) {
			frame.lva.resize(how.lva.size(), Type::topType());
		} else if (how.lva.size() < frame.lva.size()) {
			how.lva.resize(frame.lva.size(), Type::topType());
		}

		Error::assert(frame.lva.size() == how.lva.size(), "%ld != %ld",
				frame.lva.size(), how.lva.size());

		bool change = false;

		for (u4 i = 0; i < frame.lva.size(); i++) {
			change = change || assign(frame.lva[i], how.lva[i], classPath);
		}

		std::list<Type>::iterator i = frame.stack.begin();
		std::list<Type>::iterator j = how.stack.begin();

		for (; i != frame.stack.end(); i++, j++) {
			change = change || assign(*i, *j, classPath);
		}

		return change;
	}

	static void visitCatch(const BasicBlock& bb, InstList& instList,
			const ClassFile& cf, const CodeAttr* code, IClassPath* classPath,
			bool useIn) {
		if (bb.start->isLabel()) {
			for (auto ex : code->exceptions) {
				if (ex.startpc->label()->id == bb.start->label()->id) {
					BasicBlock* handlerBb = bb.cfg->findBasicBlockOfLabel(
							ex.handlerpc->label()->id);

					Type exType = [&]() {
						if (ex.catchtype != ConstPool::NULLENTRY) {
							const string& className = cf.getClassName(
									ex.catchtype);
							return Type::fromConstClass(className);
						} else {
							return Type::objectType("java/lang/Throwable");
						}
					}();

					Frame frame = useIn ? bb.in : bb.out;
					//Frame frame = bb.out;
					frame.clearStack();
					frame.push(exType);

					computeState(*handlerBb, frame, instList, cf, code,
							classPath);
				}
			}
		}
	}

	static void computeState(BasicBlock& bb, Frame& how, InstList& instList,
			const ClassFile& cf, const CodeAttr* code, IClassPath* classPath) {
		if (bb.start == instList.end()) {
			Error::assert(bb.name == "Exit" && bb.exit == instList.end(),
					"exit bb");
			return;
		}

		Error::assert(how.valid, "how valid");
		Error::assert(bb.in.valid == bb.out.valid, "");

		bool change = [&]() {
			if (!bb.in.valid) {
				bb.in = how;
				//bb.out = bb.in;
				return true;
			} else {
				//cerr
				return join(bb.in, how, classPath);
			}
		}();

		if (change) {
			bb.out = bb.in;

			SmtBuilder builder(bb.out, cf);
			for (auto it = bb.start; it != bb.exit; ++it) {
				Inst* inst = *it;

				//cerr << "after cf" << *inst << endl;

				builder.processInst(*inst);
			}

			//cerr << "finished process inst"   << endl;

			Frame h = bb.out;

			for (BasicBlock* nid : bb) {
				computeState(*nid, h, instList, cf, code, classPath);
			}

			visitCatch(bb, instList, cf, code, classPath, true);
			visitCatch(bb, instList, cf, code, classPath, false);
		}
	}

	void processInst(Inst& inst) {
		switch (inst.opcode) {
			case OPCODE_nop:
				break;
			case OPCODE_aconst_null:
				frame.pushNull();
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
				frame.pushInt();
				break;
			case OPCODE_lconst_0:
			case OPCODE_lconst_1:
				frame.pushLong();
				break;
			case OPCODE_fconst_0:
			case OPCODE_fconst_1:
			case OPCODE_fconst_2:
				frame.pushFloat();
				break;
			case OPCODE_dconst_0:
			case OPCODE_dconst_1:
				frame.pushDouble();
				break;
			case OPCODE_ldc:
			case OPCODE_ldc_w: {
				ConstTag tag = cp.getTag(inst.ldc()->valueIndex);
				switch (tag) {
					case CONST_INTEGER:
						frame.pushInt();
						break;
					case CONST_FLOAT:
						frame.pushFloat();
						break;
					case CONST_CLASS:
						frame.pushRef(cp.getClassName(inst.ldc()->valueIndex));
						break;
					case CONST_STRING:
						frame.pushRef("java/lang/String");
						break;
					default:
						Error::raise("Invalid tag entry: ", tag);
				}
				break;
			}
			case OPCODE_ldc2_w: {
				ConstTag tag = cp.getTag(inst.ldc()->valueIndex);
				switch (tag) {
					case CONST_LONG:
						frame.pushLong();
						break;
					case CONST_DOUBLE:
						frame.pushDouble();
						break;
					default:
						Error::raise("Invalid constant for ldc2_w");
				}
				break;
			}
			case OPCODE_iload:
			case OPCODE_iload_0:
			case OPCODE_iload_1:
			case OPCODE_iload_2:
			case OPCODE_iload_3:
				frame.pushInt();
				break;
			case OPCODE_lload:
			case OPCODE_lload_0:
			case OPCODE_lload_1:
			case OPCODE_lload_2:
			case OPCODE_lload_3:
				frame.pushLong();
				break;
			case OPCODE_fload:
			case OPCODE_fload_0:
			case OPCODE_fload_1:
			case OPCODE_fload_2:
			case OPCODE_fload_3:
				frame.pushFloat();
				break;
			case OPCODE_dload:
			case OPCODE_dload_0:
			case OPCODE_dload_1:
			case OPCODE_dload_2:
			case OPCODE_dload_3:
				frame.pushDouble();
				break;
			case OPCODE_aload:
				aload(inst.var()->lvindex);
				break;
			case OPCODE_aload_0:
				aload(0);
				break;
			case OPCODE_aload_1:
				aload(1);
				break;
			case OPCODE_aload_2:
				aload(2);
				break;
			case OPCODE_aload_3:
				aload(3);
				break;
			case OPCODE_iaload:
			case OPCODE_baload:
			case OPCODE_caload:
			case OPCODE_saload:
				frame.popInt();
				frame.popArray();
				frame.pushInt();
				break;
			case OPCODE_laload:
				frame.popInt();
				frame.popArray();
				frame.pushLong();
				break;
			case OPCODE_faload:
				frame.popInt();
				frame.popArray();
				frame.pushFloat();
				break;
			case OPCODE_daload: {
				frame.popInt();
				Type arrayType = frame.popArray();
				//Error::check(arrayType.isArray(), "Not array: ", arrayType);
				//Error::check(arrayType.elementType().isDouble(), "Not array: ", arrayType);
				frame.pushDouble();
				break;
			}
			case OPCODE_aaload: {
				frame.popInt();
				Type arrayType = frame.popArray();
				if (arrayType.isNull()) {
					frame.pushNull();
				} else {
					//h.pushRef(arrayType.getClassName());
					Type elementType = arrayType.elementType();
					Error::check(elementType.isObject(), "Not an object:",
							elementType);
					frame.push(elementType);
				}
				break;
			}
			case OPCODE_istore:
				istore(inst.var()->lvindex);
				break;
			case OPCODE_lstore:
				lstore(inst.var()->lvindex);
				break;
			case OPCODE_fstore:
				fstore(inst.var()->lvindex);
				break;
			case OPCODE_dstore:
				dstore(inst.var()->lvindex);
				break;
			case OPCODE_astore:
				astore(inst.var()->lvindex);
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
				frame.popInt();
				xastore();
				break;
			case OPCODE_lastore:
				frame.popLong();
				xastore();
				break;
			case OPCODE_fastore:
				frame.popFloat();
				xastore();
				break;
			case OPCODE_dastore:
				frame.popDouble();
				xastore();
				break;
			case OPCODE_aastore:
				frame.popRef();
				xastore();
				break;
			case OPCODE_pop:
				frame.popOneWord();
				break;
			case OPCODE_pop2:
				frame.popTwoWord();
				break;
			case OPCODE_dup: {
				auto t1 = frame.popOneWord();
				frame.push(t1);
				frame.push(t1);
				break;
			}
			case OPCODE_dup_x1: {
				auto t1 = frame.pop();
				auto t2 = frame.pop();
				frame.push(t1);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_dup_x2: {
				auto t1 = frame.pop();
				auto t2 = frame.pop();
				auto t3 = frame.pop();
				frame.push(t1);
				frame.push(t3);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_dup2: {
				auto t1 = frame.pop();
				auto t2 = frame.pop();
				frame.push(t2);
				frame.push(t1);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_dup2_x1: {
				auto t1 = frame.pop();
				auto t2 = frame.pop();
				auto t3 = frame.pop();
				frame.push(t2);
				frame.push(t1);
				frame.push(t3);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_dup2_x2: {
				auto t1 = frame.pop();
				auto t2 = frame.pop();
				auto t3 = frame.pop();
				auto t4 = frame.pop();
				frame.push(t2);
				frame.push(t1);
				frame.push(t4);
				frame.push(t3);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_swap: {
				auto t1 = frame.pop();
				auto t2 = frame.pop();
				frame.push(t1);
				frame.push(t2);
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
				auto t1 = frame.pop();
				frame.pop();
				frame.push(t1);
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
				frame.popLong();
				frame.popLong();
				frame.pushLong();
				break;
			case OPCODE_lshl:
			case OPCODE_lshr:
			case OPCODE_lushr:
				frame.popInt();
				frame.popLong();
				frame.pushLong();
				break;
			case OPCODE_dadd:
			case OPCODE_dsub:
			case OPCODE_dmul:
			case OPCODE_ddiv:
			case OPCODE_drem: {
				frame.pop();
				frame.pop();
				frame.pop();
				frame.pop();
				frame.pushDouble();
				break;
			}
			case OPCODE_ineg:
			case OPCODE_fneg: {
				auto t1 = frame.pop();
				frame.push(t1);
				break;
			}
			case OPCODE_lneg: {
				//h.pop();
				//h.pop();
				frame.popLong();
				frame.pushLong();
				break;
			}
			case OPCODE_dneg: {
				//h.pop();
				//h.pop();
				frame.popDouble();
				frame.pushDouble();
				break;
			}
			case OPCODE_iinc:
				iinc(inst.iinc()->index);
				break;
			case OPCODE_i2l:
				frame.popInt();
				frame.pushLong();
				break;
			case OPCODE_i2f:
				frame.popInt();
				frame.pushFloat();
				break;
			case OPCODE_i2d:
				frame.popInt();
				frame.pushDouble();
				break;
			case OPCODE_l2i:
				//h.pop();
				//h.pop();
				frame.popLong();
				frame.pushInt();
				break;
			case OPCODE_l2f:
				//h.pop();
				//h.pop();
				frame.popLong();
				frame.pushFloat();
				break;
			case OPCODE_l2d:
				//h.pop();
				//h.pop();
				frame.popLong();
				frame.pushDouble();
				break;
			case OPCODE_f2i:
				frame.popFloat();
				frame.pushInt();
				break;
			case OPCODE_f2l:
				frame.popFloat();
				frame.pushLong();
				break;
			case OPCODE_f2d:
				frame.popFloat();
				frame.pushDouble();
				break;
			case OPCODE_d2i:
				//h.pop();
				//h.pop();
				frame.popDouble();
				frame.pushInt();
				break;
			case OPCODE_d2l:
				//h.pop();
				//h.pop();
				frame.popDouble();
				frame.pushLong();
				break;
			case OPCODE_d2f:
				//h.pop();
				//h.pop();
				frame.popDouble();
				frame.pushFloat();
				break;
			case OPCODE_i2b:
			case OPCODE_i2c:
			case OPCODE_i2s:
				frame.popInt();
				frame.pushInt();
				break;
			case OPCODE_lcmp:
				frame.pop();
				frame.pop();
				frame.pop();
				frame.pop();
				frame.pushInt();
				break;
			case OPCODE_fcmpl:
			case OPCODE_fcmpg:
				frame.pop();
				frame.pop();
				frame.pushInt();
				break;
			case OPCODE_dcmpl:
			case OPCODE_dcmpg:
				frame.pop();
				frame.pop();
				frame.pop();
				frame.pop();
				frame.pushInt();
				break;
			case OPCODE_ifeq:
			case OPCODE_ifne:
			case OPCODE_iflt:
			case OPCODE_ifge:
			case OPCODE_ifgt:
			case OPCODE_ifle:
				frame.pop();
				break;
			case OPCODE_if_icmpeq:
			case OPCODE_if_icmpne:
			case OPCODE_if_icmplt:
			case OPCODE_if_icmpge:
			case OPCODE_if_icmpgt:
			case OPCODE_if_icmple:
				frame.pop();
				frame.pop();
				break;
			case OPCODE_if_acmpeq:
			case OPCODE_if_acmpne:
				frame.pop();
				frame.pop();
				break;
			case OPCODE_goto:
				break;
			case OPCODE_jsr:
				throw JsrRetNotSupported();
				//Error::assert(false, "jsr not implemented");
				break;
			case OPCODE_ret:
				throw JsrRetNotSupported();
				//Error::assert(false, "jsr not implemented");
				break;
			case OPCODE_tableswitch:
			case OPCODE_lookupswitch:
				frame.pop();
				break;
			case OPCODE_ireturn:
				frame.pop();
				break;
			case OPCODE_lreturn:
				frame.pop();
				frame.pop();
				break;
			case OPCODE_freturn:
				frame.pop();
				break;
			case OPCODE_dreturn:
				frame.pop();
				frame.pop();
				break;
			case OPCODE_areturn:
				frame.pop();
				break;
			case OPCODE_return:
				break;
			case OPCODE_getstatic: {
				auto t = fieldType(inst);
				frame.pushType(t);
				break;
			}
			case OPCODE_putstatic: {
				auto t = fieldType(inst);
				frame.popType(t);
				break;
			}
			case OPCODE_getfield: {
				auto t = fieldType(inst);
				frame.popRef();
				frame.pushType(t);
				break;
			}
			case OPCODE_putfield: {
				auto t = fieldType(inst);
				frame.popType(t);
				frame.popRef();
				break;
			}
			case OPCODE_invokevirtual:
			case OPCODE_invokespecial:
				invokeMethod(inst.invoke()->methodRefIndex, true);
				break;
			case OPCODE_invokestatic:
				invokeMethod(inst.invoke()->methodRefIndex, false);
				break;
			case OPCODE_invokeinterface:
				invokeInterface(inst.invokeinterface()->interMethodRefIndex);
				break;
			case OPCODE_invokedynamic:
				Error::raise("invoke dynamic instances not implemented");
				break;
			case OPCODE_new: {
				//h.pushRef();
				const string& className = cp.getClassName(
						inst.type()->classIndex);
				Type t = Type::fromConstClass(className);
				Error::check(!t.isArray(), "New with array: ", t);
				frame.push(t);
				break;
			}
			case OPCODE_newarray:
				frame.popInt();
				frame.pushArray(getArrayBaseType(inst.newarray()->atype), 1);
				break;
			case OPCODE_anewarray: {
				frame.popInt();
//				string className = cp.getClassName(inst.type.classIndex);
				const string& className = cp.getClassName(
						inst.type()->classIndex);
				Type t = Type::fromConstClass(className);
				//			h.push(parseConstClass(className));
				//Type refType = Type::objectType(className);
				frame.pushArray(t, t.getDims() + 1);
				break;
			}
			case OPCODE_arraylength:
				frame.pop();
				frame.pushInt();
				break;
			case OPCODE_athrow: {
				auto t = frame.popRef();
				frame.clearStack();
				frame.push(t);
				break;
			}
			case OPCODE_checkcast: {
				frame.popRef();
				const string& className = cp.getClassName(
						inst.type()->classIndex);
				frame.push(Type::fromConstClass(className));
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
				frame.popRef();
				frame.pushInt();
				break;
			case OPCODE_monitorenter:
			case OPCODE_monitorexit:
				frame.popRef();
				break;
			case OPCODE_wide:
				switch (inst.wide()->subOpcode) {
					case OPCODE_iload:
						frame.pushInt();
						break;
					case OPCODE_lload:
						frame.pushLong();
						break;
					case OPCODE_fload:
						frame.pushFloat();
						break;
					case OPCODE_dload:
						frame.pushDouble();
						break;
					case OPCODE_istore:
						istore(inst.wide()->var.lvindex);
						break;
					case OPCODE_fstore:
						fstore(inst.wide()->var.lvindex);
						break;
					case OPCODE_lstore:
						lstore(inst.wide()->var.lvindex);
						break;
					case OPCODE_dstore:
						dstore(inst.wide()->var.lvindex);
						break;
					case OPCODE_iinc:
						iinc(inst.wide()->iinc.index);
						break;
					default:
						Error::raise("Unsupported wide opcode: ",
								inst.wide()->subOpcode);
				}

				break;
			case OPCODE_multianewarray:
				multianewarray(inst);
				break;
			case OPCODE_ifnull:
			case OPCODE_ifnonnull:
				frame.pop();
				break;
			case OPCODE_goto_w:
			case OPCODE_jsr_w:
			case OPCODE_breakpoint:
			case OPCODE_impdep1:
			case OPCODE_impdep2:
				Error::raise("goto_w, jsr_w breakpoint not implemented");
				break;
			default:
				Error::raise("unknown opcode not implemented: ", inst.opcode);
		}
	}

private:

	void istore(int lvindex) {
		frame.popInt();
		frame.setIntVar(lvindex);
	}

	void lstore(int lvindex) {
		frame.popLong();
		frame.setLongVar(lvindex);
	}

	void fstore(int lvindex) {
		frame.popFloat();
		frame.setFloatVar(lvindex);
	}

	void dstore(int lvindex) {
		frame.popDouble();
		frame.setDoubleVar(lvindex);
	}

	void astore(int lvindex) {
		Type refType = frame.popRef();
		Error::check(!refType.isTop(), "astore: Setting variable index ",
				lvindex, " to Top", refType, " in frame ", frame);

		frame.setRefVar(lvindex, refType);
	}

	void aload(u4 lvindex) {
		const Type& type = frame.getVar(lvindex);
		Error::check(type.isObject() || type.isNull(), "Bad ref var at index[",
				lvindex, "]: ", type, " @ frame: ", frame);
		frame.pushType(type);
	}

	void xastore() {
		frame.popInt();
		frame.popArray();
	}

	void iinc(u2 index) {
		frame.setIntVar(index);
	}

	void invokeMethod(u2 methodRefIndex, bool popThis) {
		string className, name, desc;
		cp.getMethodRef(methodRefIndex, &className, &name, &desc);
		invoke(desc, popThis);
	}

	void invokeInterface(u2 interMethodRefIndex) {
		string className, name, desc;
		cp.getInterMethodRef(interMethodRefIndex, &className, &name, &desc);
		invoke(desc, true);
	}

	void invoke(const string& desc, bool popThis) {
		const char* d = desc.c_str();
		vector<Type> argsType;
		Type returnType = Type::fromMethodDesc(d, &argsType);

		for (int i = argsType.size() - 1; i >= 0; i--) {
			const Type& argType = argsType[i];
			Error::check(argType.isOneOrTwoWord(),
					"Invalid arg type in method");
			frame.popType(argType);
		}

		if (popThis) {
			frame.popRef();
		}

		if (!returnType.isVoid()) {
			Error::assert(returnType.isOneOrTwoWord(), "Ret type: ",
					returnType);
			//assert(!returnType.isTwoWord(), "Two word in return type");
			frame.pushType(returnType);
			//h.push(returnType);
		}
	}

	Type fieldType(Inst& inst) {
		string className, name, desc;
		cp.getFieldRef(inst.field()->fieldRefIndex, &className, &name, &desc);

		const char* d = desc.c_str();
		auto t = Type::fromFieldDesc(d);

		//assert(!t.isTwoWord(), "Two word in field");
		return t;
	}

	void multianewarray(Inst& inst) {
		u1 dims = inst.multiarray()->dims;
		Error::check(dims >= 1, "invalid dims: ", dims);

		for (int i = 0; i < dims; i++) {
			frame.popInt();
		}

		string arrayClassName = cp.getClassName(inst.multiarray()->classIndex);
		const char* d = arrayClassName.c_str();
		Type arrayType = Type::fromFieldDesc(d);

		frame.pushType(arrayType);
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

		Error::raise("invalid atype: ", atype);
	}

	Frame& frame;
	const ConstPool& cp;
};

class Compute: private Error {
public:

	static void setCpIndex(Type& type, ConstPool& cp) {
		if (type.isObject()) {
			const string& className = type.getClassName();

			ConstIndex utf8index = cp.putUtf8(className.c_str());
			ConstIndex index = cp.addClass(utf8index);
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
			ClassFile* cf, ConstIndex* attrIndex, IClassPath* classPath) {

		for (auto it = code->attrs.begin(); it != code->attrs.end(); it++) {
			Attr* attr = *it;
			if (attr->kind == ATTR_SMT) {
				code->attrs.attrs.erase(it);
				delete attr;
				break;
			}
		}

		if (!code->instList.hasBranches() && !code->hasTryCatch()) {
			return;
		}

		if (*attrIndex == ConstPool::NULLENTRY) {
			*attrIndex = cf->putUtf8("StackMapTable");
		}

		//code->instList.setLabelIds();

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
		Type::fromMethodDesc(methodDesc, &argsType);

		for (Type t : argsType) {
			initFrame.setVar(&lvindex, t);
		}

		ControlFlowGraph* cfgp = new ControlFlowGraph(code->instList);
		ControlFlowGraph& cfg = *cfgp;

		initFrame.valid = true;
		BasicBlock* bbe = cfg.entry;
		bbe->in = initFrame;
		bbe->out = initFrame;

		BasicBlock* to = *cfg.entry->begin();
		SmtBuilder::computeState(*to, initFrame, code->instList, *cf, code,
				classPath);

		SmtAttr* smt = new SmtAttr(*attrIndex, cf);

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
				if (start->isLabel()
						&& (start->label()->isBranchTarget
								|| start->label()->isCatchHandler)) {
					Frame& current = bb->in;
					current.cleanTops();

					setCpIndexes(current, *cf);

					SmtAttr::Entry e;

					e.label = start;

					//Inst* fi = new Inst(KIND_FRAME);
					//fi->frame.frame = bb->in;
					//code->instList.insert(bb->start, fi);

					totalOffset += 1;
					int offsetDelta = start->label()->offset - totalOffset;

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

						list<Type> rs = current.stack;
						rs.reverse();
						for (const Type& t : rs) {
							e.full_frame.stack.push_back(t);
							//e.full_frame.stack.push_front(t);
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

void ClassFile::computeFrames(IClassPath* classPath) {
	computeSize();

	ConstIndex attrIndex = ConstPool::NULLENTRY;

	for (Method* method : methods) {
		CodeAttr* code = method->codeAttr();

		if (code != nullptr) {

//			bool hasJsrOrRet = false;
//			for (Inst* inst : code->instList) {
//				if (inst->isJsrOrRet()) {
//					cerr << "JSR/RET in compute frames!" << endl;
//					hasJsrOrRet = true;
//				}
//			}
//
//			Error::assert(hasJsrOrRet == code->instList.hasJsrOrRet());

			bool hasJsrOrRet = code->instList.hasJsrOrRet();
			if (hasJsrOrRet) {
				return;
			}

			Compute::computeFramesMethod(code, method, this, &attrIndex,
					classPath);
		}
	}
}

}
