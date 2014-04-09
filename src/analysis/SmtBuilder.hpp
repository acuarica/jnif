/*
 * SmtBuilder.hpp
 *
 *  Created on: Apr 7, 2014
 *      Author: luigi
 */

#ifndef JNIF_SMTBUILDER_HPP
#define JNIF_SMTBUILDER_HPP

#include "State.hpp"

namespace jnif {

class SmtBuilder: private DescParser {
public:

	static void computeState(ControlFlowGraph::NodeKey to, H& how,
			ControlFlowGraph& cfg, std::vector<State>& states,
			InstList& instList, std::ostream& os, ClassFile& cf) {

		InstList::iterator b;
		InstList::iterator e;
		std::string name;
		std::tie(b, e, name) = cfg.getNode(to);
		os << "computing " << name << std::endl;

		if (b == instList.end()) {
			assert(name == "Exit", "");
			assert(e == instList.end(), "");
			return;
		}

		assert(how.valid, "");

		State& s = states[to._index];

		assert(s.in.valid == s.out.valid, "");

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

				computeState(nid, h, cfg, states, instList, os, cf);
			}
		}
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

		auto invoke = [&](u2 methodRefIndex, bool popThis) {
			std::string className, name, desc;
			cp.getMethodRef(methodRefIndex, &className, &name, &desc);

			const char* d = desc.c_str();
			std::vector<Type> argsType;
			Type returnType = parseMethodDesc(d, &argsType);

			for (u4 i = 0; i < argsType.size(); i++) {
				h.pop();
			}

			if (popThis) {
				h.pop();
			}

			if (!returnType.isVoid()) {
				h.push(returnType);
			}
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

				ConstPool::Tag tag = cp.getTag(inst.ldc.valueIndex);
				switch (tag) {
					case ConstPool::INTEGER:
						h.pushInt();
						break;
					case ConstPool::FLOAT:
						h.pushFloat();
						break;
					case ConstPool::LONG:
						h.pushLong();
						break;
					case ConstPool::DOUBLE:
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
				std::string className, name, desc;
				cp.getFieldRef(inst.field.fieldRefIndex, &className, &name,
						&desc);

				const char* d = desc.c_str();
				auto t = parseFieldDesc(d);
				h.push(t);
				break;
			}
			case OPCODE_putstatic:
				h.pop(); // wrong: could be double or long.
				break;
			case OPCODE_getfield:
				h.pop(); // wrong: could be double or long.
				h.pushRef();
				break;
			case OPCODE_putfield:
				h.pop();
				h.pop();
				break;
			case OPCODE_invokevirtual:
			case OPCODE_invokespecial:
				invoke(inst.invoke.methodRefIndex, true);
				break;
			case OPCODE_invokestatic:
				invoke(inst.invoke.methodRefIndex, false);
				break;
			case OPCODE_invokeinterface:
				invoke(inst.invokeinterface.interMethodRefIndex, true);
				break;
			case OPCODE_invokedynamic:
				raise("invoke dynamic instances not implemented");
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
				assert(false,
						"athrow checkcast instanceof me, me not implemented");
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
				assert(false, "extended not implemented");
				break;

			default:
				break;
		}
	}

};

}

#endif
