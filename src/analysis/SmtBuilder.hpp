/*
 * SmtBuilder.hpp
 *
 *  Created on: Jun 25, 2014
 *      Author: luigi
 */

#ifndef JNIF_ANALYSIS_SMTBUILDER_HPP
#define JNIF_ANALYSIS_SMTBUILDER_HPP

#include "../TypeFactory.hpp"

namespace jnif {

class JsrRetNotSupported {

};

class SmtBuilder {
public:

	SmtBuilder(Frame& frame, const ConstPool& cp, Method* m,
			TypeFactory& typeFactory) :
			frame(frame), cp(cp), _m(m), _typeFactory(typeFactory) {
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
			case OPCODE_ldc_w:
				ldc(inst);
				break;
			case OPCODE_ldc2_w:
				ldc2(inst);
				break;
			case OPCODE_iload:
				iload(inst.var()->lvindex);
				break;
			case OPCODE_iload_0:
				iload(0);
				break;
			case OPCODE_iload_1:
				iload(1);
				break;
			case OPCODE_iload_2:
				iload(2);
				break;
			case OPCODE_iload_3:
				iload(3);
				break;
			case OPCODE_lload:
				lload(inst.var()->lvindex);
				break;
			case OPCODE_lload_0:
				lload(0);
				break;
			case OPCODE_lload_1:
				lload(1);
				break;
			case OPCODE_lload_2:
				lload(2);
				break;
			case OPCODE_lload_3:
				lload(3);
				break;
			case OPCODE_fload:
				fload(inst.var()->lvindex);
				break;
			case OPCODE_fload_0:
				fload(0);
				break;
			case OPCODE_fload_1:
				fload(1);
				break;
			case OPCODE_fload_2:
				fload(2);
				break;
			case OPCODE_fload_3:
				fload(3);
				break;
			case OPCODE_dload:
				dload(inst.var()->lvindex);
				break;
			case OPCODE_dload_0:
				dload(0);
				break;
			case OPCODE_dload_1:
				dload(1);
				break;
			case OPCODE_dload_2:
				dload(2);
				break;
			case OPCODE_dload_3:
				dload(3);
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
				frame.popIntegral();
				frame.popArray();
				frame.pushInt();
				break;
			case OPCODE_laload:
				frame.popIntegral();
				frame.popArray();
				frame.pushLong();
				break;
			case OPCODE_faload:
				frame.popIntegral();
				frame.popArray();
				frame.pushFloat();
				break;
			case OPCODE_daload: {
				frame.popIntegral();
				Type arrayType = frame.popArray();
				frame.pushDouble();
				break;
			}
			case OPCODE_aaload:
				aaload(inst);
				break;
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
				frame.popIntegral();
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
				const Type& t1 = frame.popOneWord();
				frame.push(t1);
				frame.push(t1);
				break;
			}
			case OPCODE_dup_x1: {
				const Type& t1 = frame.popOneWord();
				const Type& t2 = frame.popOneWord();
				frame.push(t1);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_dup_x2: {
				const Type& t1 = frame.pop();
				const Type& t2 = frame.pop();
				const Type& t3 = frame.pop();
				frame.push(t1);
				frame.push(t3);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_dup2: {
				const Type& t1 = frame.pop();
				const Type& t2 = frame.pop();
				frame.push(t2);
				frame.push(t1);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_dup2_x1: {
				const Type& t1 = frame.pop();
				const Type& t2 = frame.pop();
				const Type& t3 = frame.pop();
				frame.push(t2);
				frame.push(t1);
				frame.push(t3);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_dup2_x2: {
				const Type& t1 = frame.pop();
				const Type& t2 = frame.pop();
				const Type& t3 = frame.pop();
				const Type& t4 = frame.pop();
				frame.push(t2);
				frame.push(t1);
				frame.push(t4);
				frame.push(t3);
				frame.push(t2);
				frame.push(t1);
				break;
			}
			case OPCODE_swap: {
				const Type& t1 = frame.pop();
				const Type& t2 = frame.pop();
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
				const Type& t1 = frame.pop();
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
				frame.popIntegral();
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
				const Type& t1 = frame.pop();
				frame.push(t1);
				break;
			}
			case OPCODE_lneg: {
				frame.popLong();
				frame.pushLong();
				break;
			}
			case OPCODE_dneg: {
				frame.popDouble();
				frame.pushDouble();
				break;
			}
			case OPCODE_iinc:
				iinc(inst.iinc()->index);
				break;
			case OPCODE_i2l:
				frame.popIntegral();
				frame.pushLong();
				break;
			case OPCODE_i2f:
				frame.popIntegral();
				frame.pushFloat();
				break;
			case OPCODE_i2d:
				frame.popIntegral();
				frame.pushDouble();
				break;
			case OPCODE_l2i:
				frame.popLong();
				frame.pushInt();
				break;
			case OPCODE_l2f:
				frame.popLong();
				frame.pushFloat();
				break;
			case OPCODE_l2d:
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
				frame.popDouble();
				frame.pushInt();
				break;
			case OPCODE_d2l:
				frame.popDouble();
				frame.pushLong();
				break;
			case OPCODE_d2f:
				frame.popDouble();
				frame.pushFloat();
				break;
			case OPCODE_i2b:
			case OPCODE_i2c:
			case OPCODE_i2s:
				frame.popIntegral();
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
				frame.popIntegral();
				frame.popIntegral();
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
				break;
			case OPCODE_ret:
				throw JsrRetNotSupported();
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
				const Type& t = fieldType(inst);
				frame.pushType(t);
				break;
			}
			case OPCODE_putstatic: {
				const Type& t = fieldType(inst);
				frame.popType(t);
				break;
			}
			case OPCODE_getfield: {
				const Type& t = fieldType(inst);
				frame.popRef();
				frame.pushType(t);
				break;
			}
			case OPCODE_putfield: {
				const Type& t = fieldType(inst);
				frame.popType(t);
				frame.popRef();
				break;
			}
			case OPCODE_invokevirtual:
				invokeMethod(inst.invoke()->methodRefIndex, true, false);
				break;
			case OPCODE_invokespecial:
				invokeSpecial(inst.invoke()->methodRefIndex);
				break;
			case OPCODE_invokestatic:
				invokeMethod(inst.invoke()->methodRefIndex, false, false);
				break;
			case OPCODE_invokeinterface:
				invokeInterface(inst.invokeinterface()->interMethodRefIndex);
				break;
			case OPCODE_new:
				newinst(*inst.type());
				break;
			case OPCODE_newarray:
				newarray(inst);
				break;
			case OPCODE_anewarray:
				anewarray(inst);
				break;
			case OPCODE_arraylength:
				frame.pop();
				frame.pushInt();
				break;
			case OPCODE_athrow:
				athrow(inst);
				break;
			case OPCODE_checkcast:
				checkcast(inst);
				break;
			case OPCODE_instanceof:
				frame.popRef();
				frame.pushInt();
				break;
			case OPCODE_monitorenter:
			case OPCODE_monitorexit:
				frame.popRef();
				break;
			case OPCODE_wide:
				wide(inst);
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
			case OPCODE_invokedynamic: {
				ConstIndex callSite = inst.indy()->callSite();
				const ConstInvokeDynamic& dyn = cp.getInvokeDynamic(callSite);

				String name;
				String desc;
				cp.getNameAndType(dyn.nameAndTypeIndex, &name, &desc);

				//cerr << name << endl;
				//cerr << desc << endl;

				invoke("invokedynamic call site", name, desc, false, false);

				//Error::raise("invoke dynamic instances not implemented");
				break;
			}
			default:
				Error::raise("unknown opcode not implemented: ", inst.opcode);
		}
	}

private:

	void newinst(TypeInst& inst) {
		const String& className = cp.getClassName(inst.type()->classIndex);
		const Type& t = _typeFactory.fromConstClass(className);
		t.init = false;
		t.typeId = Type::nextTypeId;
		Type::nextTypeId++;

		t.uninit.newinst = &inst;
		Error::check(!t.isArray(), "New with array: ", t);
		frame.push(t);
	}

	void newarray(Inst& inst) {
		frame.popIntegral();
		frame.pushArray(getArrayBaseType(inst.newarray()->atype), 1);
	}

	void anewarray(Inst& inst) {
		frame.popIntegral();
		const String& className = cp.getClassName(inst.type()->classIndex);
		const Type& t = _typeFactory.fromConstClass(className);
		frame.pushArray(t, t.getDims() + 1);
	}

	void aaload(Inst&) {
		frame.popIntegral();
		Type arrayType = frame.popArray();
		if (arrayType.isNull()) {
			frame.pushNull();
		} else {
			const Type& elementType = arrayType.elementType(_typeFactory);
			Error::check(elementType.isObject(), "Not an object:", elementType);
			frame.push(elementType);
		}
	}

	void ldc(Inst& inst) {
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
	}

	void ldc2(Inst& inst) {
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
	}

	void athrow(Inst&) {
		Type t = frame.popRef();
		frame.clearStack();
		frame.push(t);
	}

	void checkcast(Inst& inst) {
		frame.popRef();
		const String& className = cp.getClassName(inst.type()->classIndex);
		frame.push(_typeFactory.fromConstClass(className));
	}

	void istore(int lvindex) {
		frame.popIntegral();
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

	void iload(u4 lvindex, int offset = 0) {
		const Type& type = frame.getVar(lvindex);
		Error::check(type.isIntegral(), "iload: ", type, " at index ", lvindex,
				":offset:", offset, " for ", _m);
		frame.pushInt();
	}

	void fload(u4 lvindex) {
		const Type& type = frame.getVar(lvindex);
		Error::check(type.isFloat(), "fload: ", type, " @ ", lvindex);
		frame.pushFloat();
	}

	void lload(u4 lvindex) {
		const Type& type = frame.getVar(lvindex);
		Error::check(type.isLong(), "lload: ", type, " @ ", lvindex);
		frame.pushLong();
	}

	void dload(u4 lvindex) {
		const Type& type = frame.getVar(lvindex);
		Error::check(type.isDouble(), "dload: ", type, " @ ", lvindex);
		frame.pushDouble();
	}

	void aload(u4 lvindex) {
		const Type& type = frame.getVar(lvindex);
		Error::check(type.isObject() || type.isNull() || type.isUninitThis(),
				"Bad ref var at index[", lvindex, "]: ", type, " @ frame: ",
				frame);
		frame.pushType(type);
	}

	void xastore() {
		frame.popIntegral();
		frame.popArray();
	}

	void iinc(u2 index) {
		frame.setIntVar(index);
	}

	void invokeMethod(u2 methodRefIndex, bool popThis, bool isSpecial) {
		String className, name, desc;
		cp.getMethodRef(methodRefIndex, &className, &name, &desc);
		invoke(className, name, desc, popThis, isSpecial);
	}

	void invokeSpecial(u2 methodRefIndex) {
		invokeMethod(methodRefIndex, true, true);
	}

	void invokeInterface(u2 interMethodRefIndex) {
		String className, name, desc;
		cp.getInterMethodRef(interMethodRefIndex, &className, &name, &desc);
		invoke(className, name, desc, true, false);
	}

	void invoke(const String& className, const String& name, const String& desc,
			bool popThis, bool isSpecial) {
		const char* d = desc.c_str();
		std::vector<Type> argsType;
		const Type& returnType = _typeFactory.fromMethodDesc(d, &argsType);

		for (int i = argsType.size() - 1; i >= 0; i--) {
			const Type& argType = argsType[i];
			Error::check(argType.isOneOrTwoWord(),
					"Invalid arg type in method");
			frame.popType(argType);
		}

		if (popThis) {
			Type t = frame.popRef();
			//Error::check(t.iso)
			if (isSpecial && name == "<init>") {
				Error::check(t.typeId > 0, "inv typeId: ", t.typeId, t);
				Error::check(!t.init, "Object is already init: ", t, ", ",
						className, ".", name, desc, ", frame: ", frame);

				t.init = true;

				for (Type& tr : frame.lva) {
					if (tr.typeId == t.typeId) {
//						Error::check(!tr.init,
//								"Object is already init in lva: ", tr, ", ",
//								className, ".", name, desc, ", ", t);
						Error::check(tr.className != "", "empty clsname lva");
						Error::check(tr.className == t.className,
								"!= clsname lva");

						tr.init = true;
						tr.tag = TYPE_OBJECT;

					}
				}

				for (Type& tr : frame.stack) {
					if (tr.typeId == t.typeId) {
//						Error::check(!tr.init,
//								"Object is already init in stack: ", tr, ", ",
//								className, ".", name, desc, ", ", t);
						Error::check(tr.className != "", "empty clsname stack");
						Error::check(tr.className == t.className,
								"!= clsname stack");

						tr.init = true;
						tr.tag = TYPE_OBJECT;
					}
				}

				//frame.push(t);
			}
		}

		if (!returnType.isVoid()) {
			Error::assert(returnType.isOneOrTwoWord(), "Ret type: ",
					returnType);
			frame.pushType(returnType);
		}
	}

	Type fieldType(Inst& inst) {
		String className, name, desc;
		cp.getFieldRef(inst.field()->fieldRefIndex, &className, &name, &desc);

		const char* d = desc.c_str();
		const Type& t = _typeFactory.fromFieldDesc(d);

		return t;
	}

	void multianewarray(Inst& inst) {
		u1 dims = inst.multiarray()->dims;
		Error::check(dims >= 1, "invalid dims: ", dims);

		for (int i = 0; i < dims; i++) {
			frame.popIntegral();
		}

		String arrayClassName = cp.getClassName(inst.multiarray()->classIndex);
		const char* d = arrayClassName.c_str();
		Type arrayType = _typeFactory.fromFieldDesc(d);

		frame.pushType(arrayType);
	}

	void wide(Inst& inst) {
		u2 lvindex = inst.wide()->var.lvindex;
		switch (inst.wide()->subOpcode) {
			case OPCODE_iload:
				iload(lvindex);
				break;
			case OPCODE_lload:
				lload(lvindex);
				break;
			case OPCODE_fload:
				fload(lvindex);
				break;
			case OPCODE_dload:
				dload(lvindex);
				break;
			case OPCODE_aload:
				aload(lvindex);
				break;
			case OPCODE_istore:
				istore(lvindex);
				break;
			case OPCODE_fstore:
				fstore(lvindex);
				break;
			case OPCODE_lstore:
				lstore(lvindex);
				break;
			case OPCODE_dstore:
				dstore(lvindex);
				break;
			case OPCODE_astore:
				astore(lvindex);
				break;
			case OPCODE_iinc:
				iinc(inst.wide()->iinc.index);
				break;
			default:
				Error::raise("Unsupported wide opcode: ",
						inst.wide()->subOpcode);
		}

	}

	const Type& getArrayBaseType(int atype) {
		switch (atype) {
			case NEWARRAYTYPE_BOOLEAN:
				return _typeFactory.booleanType();
			case NEWARRAYTYPE_CHAR:
				return _typeFactory.charType();
			case NEWARRAYTYPE_BYTE:
				return _typeFactory.byteType();
			case NEWARRAYTYPE_SHORT:
				return _typeFactory.shortType();
			case NEWARRAYTYPE_INT:
				return _typeFactory.intType();
			case NEWARRAYTYPE_FLOAT:
				return _typeFactory.floatType();
			case NEWARRAYTYPE_LONG:
				return _typeFactory.longType();
			case NEWARRAYTYPE_DOUBLE:
				return _typeFactory.doubleType();
		}

		Error::raise("invalid atype: ", atype);
	}

	Frame& frame;
	const ConstPool& cp;
	Method* _m;
	TypeFactory& _typeFactory;
};

}

#endif
