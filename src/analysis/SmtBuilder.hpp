/*
 * SmtBuilder.hpp
 *
 *  Created on: Jun 25, 2014
 *      Author: luigi
 */

#ifndef JNIF_ANALYSIS_SMTBUILDER_HPP
#define JNIF_ANALYSIS_SMTBUILDER_HPP

#include "../ConstPool.hpp"
#include "../Inst.hpp"
#include "../TypeFactory.hpp"
#include "../Error.hpp"

namespace jnif {

    class JsrRetNotSupported {

    };

    template<class TFrame>
    class SmtBuilder {
    public:

        SmtBuilder(TFrame& frame, const ConstPool& cp) :
            frame(frame), cp(cp) {
        }

        void exec(Inst& inst) {
            switch (inst.opcode) {
            case OPCODE_nop:
                break;
            case OPCODE_aconst_null:
                frame.pushNull(&inst);
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
                frame.pushInt(&inst);
                break;
            case OPCODE_lconst_0:
            case OPCODE_lconst_1:
                frame.pushLong(&inst);
                break;
            case OPCODE_fconst_0:
            case OPCODE_fconst_1:
            case OPCODE_fconst_2:
                frame.pushFloat(&inst);
                break;
            case OPCODE_dconst_0:
            case OPCODE_dconst_1:
                frame.pushDouble(&inst);
                break;
            case OPCODE_ldc:
            case OPCODE_ldc_w:
                ldc(inst);
                break;
            case OPCODE_ldc2_w:
                ldc2(inst);
                break;
            case OPCODE_iload:
                iload(inst.var()->lvindex, &inst);
                break;
            case OPCODE_iload_0:
                iload(0, &inst);
                break;
            case OPCODE_iload_1:
                iload(1, &inst);
                break;
            case OPCODE_iload_2:
                iload(2, &inst);
                break;
            case OPCODE_iload_3:
                iload(3, &inst);
                break;
            case OPCODE_lload:
                lload(inst.var()->lvindex, &inst);
                break;
            case OPCODE_lload_0:
                lload(0, &inst);
                break;
            case OPCODE_lload_1:
                lload(1, &inst);
                break;
            case OPCODE_lload_2:
                lload(2, &inst);
                break;
            case OPCODE_lload_3:
                lload(3, &inst);
                break;
            case OPCODE_fload:
                fload(inst.var()->lvindex, &inst);
                break;
            case OPCODE_fload_0:
                fload(0, &inst);
                break;
            case OPCODE_fload_1:
                fload(1, &inst);
                break;
            case OPCODE_fload_2:
                fload(2, &inst);
                break;
            case OPCODE_fload_3:
                fload(3, &inst);
                break;
            case OPCODE_dload:
                dload(inst.var()->lvindex, &inst);
                break;
            case OPCODE_dload_0:
                dload(0, &inst);
                break;
            case OPCODE_dload_1:
                dload(1, &inst);
                break;
            case OPCODE_dload_2:
                dload(2, &inst);
                break;
            case OPCODE_dload_3:
                dload(3, &inst);
                break;
            case OPCODE_aload:
                aload(inst.var()->lvindex, &inst);
                break;
            case OPCODE_aload_0:
                aload(0, &inst);
                break;
            case OPCODE_aload_1:
                aload(1, &inst);
                break;
            case OPCODE_aload_2:
                aload(2, &inst);
                break;
            case OPCODE_aload_3:
                aload(3, &inst);
                break;
            case OPCODE_iaload:
            case OPCODE_baload:
            case OPCODE_caload:
            case OPCODE_saload:
                frame.popIntegral(&inst);
                frame.popArray(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_laload:
                frame.popIntegral(&inst);
                frame.popArray(&inst);
                frame.pushLong(&inst);
                break;
            case OPCODE_faload:
                frame.popIntegral(&inst);
                frame.popArray(&inst);
                frame.pushFloat(&inst);
                break;
            case OPCODE_daload: {
                frame.popIntegral(&inst);
                Type arrayType = frame.popArray(&inst);
                frame.pushDouble(&inst);
                break;
            }
            case OPCODE_aaload:
                aaload(inst);
                break;
            case OPCODE_istore:
                istore(inst.var()->lvindex, &inst);
                break;
            case OPCODE_lstore:
                lstore(inst.var()->lvindex, &inst);
                break;
            case OPCODE_fstore:
                fstore(inst.var()->lvindex, &inst);
                break;
            case OPCODE_dstore:
                dstore(inst.var()->lvindex, &inst);
                break;
            case OPCODE_astore:
                astore(inst.var()->lvindex, &inst);
                break;
            case OPCODE_istore_0:
                istore(0, &inst);
                break;
            case OPCODE_istore_1:
                istore(1, &inst);
                break;
            case OPCODE_istore_2:
                istore(2, &inst);
                break;
            case OPCODE_istore_3:
                istore(3, &inst);
                break;
            case OPCODE_lstore_0:
                lstore(0, &inst);
                break;
            case OPCODE_lstore_1:
                lstore(1, &inst);
                break;
            case OPCODE_lstore_2:
                lstore(2, &inst);
                break;
            case OPCODE_lstore_3:
                lstore(3, &inst);
                break;
            case OPCODE_fstore_0:
                fstore(0, &inst);
                break;
            case OPCODE_fstore_1:
                fstore(1, &inst);
                break;
            case OPCODE_fstore_2:
                fstore(2, &inst);
                break;
            case OPCODE_fstore_3:
                fstore(3, &inst);
                break;
            case OPCODE_dstore_0:
                dstore(0, &inst);
                break;
            case OPCODE_dstore_1:
                dstore(1, &inst);
                break;
            case OPCODE_dstore_2:
                dstore(2, &inst);
                break;
            case OPCODE_dstore_3:
                dstore(3, &inst);
                break;
            case OPCODE_astore_0:
                astore(0, &inst);
                break;
            case OPCODE_astore_1:
                astore(1, &inst);
                break;
            case OPCODE_astore_2:
                astore(2, &inst);
                break;
            case OPCODE_astore_3:
                astore(3, &inst);
                break;
            case OPCODE_iastore:
            case OPCODE_bastore:
            case OPCODE_castore:
            case OPCODE_sastore:
                frame.popIntegral(&inst);
                xastore(&inst);
                break;
            case OPCODE_lastore:
                frame.popLong(&inst);
                xastore(&inst);
                break;
            case OPCODE_fastore:
                frame.popFloat(&inst);
                xastore(&inst);
                break;
            case OPCODE_dastore:
                frame.popDouble(&inst);
                xastore(&inst);
                break;
            case OPCODE_aastore:
                frame.popRef(&inst);
                xastore(&inst);
                break;
            case OPCODE_pop:
                frame.popOneWord(&inst);
                break;
            case OPCODE_pop2:
                frame.popTwoWord(&inst);
                break;
            case OPCODE_dup: {
                const Type& t1 = frame.popOneWord(&inst);
                frame.push(t1, &inst);
                frame.push(t1, &inst);
                break;
            }
            case OPCODE_dup_x1: {
                const Type& t1 = frame.popOneWord(&inst);
                const Type& t2 = frame.popOneWord(&inst);
                frame.push(t1, &inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                break;
            }
            case OPCODE_dup_x2: {
                const Type& t1 = frame.pop(&inst);
                const Type& t2 = frame.pop(&inst);
                const Type& t3 = frame.pop(&inst);
                frame.push(t1, &inst);
                frame.push(t3, &inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                break;
            }
            case OPCODE_dup2: {
                const Type& t1 = frame.pop(&inst);
                const Type& t2 = frame.pop(&inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                break;
            }
            case OPCODE_dup2_x1: {
                const Type& t1 = frame.pop(&inst);
                const Type& t2 = frame.pop(&inst);
                const Type& t3 = frame.pop(&inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                frame.push(t3, &inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                break;
            }
            case OPCODE_dup2_x2: {
                const Type& t1 = frame.pop(&inst);
                const Type& t2 = frame.pop(&inst);
                const Type& t3 = frame.pop(&inst);
                const Type& t4 = frame.pop(&inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                frame.push(t4, &inst);
                frame.push(t3, &inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                break;
            }
            case OPCODE_swap: {
                const Type& t1 = frame.pop(&inst);
                const Type& t2 = frame.pop(&inst);
                frame.push(t1, &inst);
                frame.push(t2, &inst);
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
                const Type& t1 = frame.pop(&inst);
                frame.pop(&inst);
                frame.push(t1, &inst);
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
                frame.popLong(&inst);
                frame.popLong(&inst);
                frame.pushLong(&inst);
                break;
            case OPCODE_lshl:
            case OPCODE_lshr:
            case OPCODE_lushr:
                frame.popIntegral(&inst);
                frame.popLong(&inst);
                frame.pushLong(&inst);
                break;
            case OPCODE_dadd:
            case OPCODE_dsub:
            case OPCODE_dmul:
            case OPCODE_ddiv:
            case OPCODE_drem: {
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pushDouble(&inst);
                break;
            }
            case OPCODE_ineg:
            case OPCODE_fneg: {
                const Type& t1 = frame.pop(&inst);
                frame.push(t1, &inst);
                break;
            }
            case OPCODE_lneg: {
                frame.popLong(&inst);
                frame.pushLong(&inst);
                break;
            }
            case OPCODE_dneg: {
                frame.popDouble(&inst);
                frame.pushDouble(&inst);
                break;
            }
            case OPCODE_iinc:
                iinc(inst.iinc()->index, &inst);
                break;
            case OPCODE_i2l:
                frame.popIntegral(&inst);
                frame.pushLong(&inst);
                break;
            case OPCODE_i2f:
                frame.popIntegral(&inst);
                frame.pushFloat(&inst);
                break;
            case OPCODE_i2d:
                frame.popIntegral(&inst);
                frame.pushDouble(&inst);
                break;
            case OPCODE_l2i:
                frame.popLong(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_l2f:
                frame.popLong(&inst);
                frame.pushFloat(&inst);
                break;
            case OPCODE_l2d:
                frame.popLong(&inst);
                frame.pushDouble(&inst);
                break;
            case OPCODE_f2i:
                frame.popFloat(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_f2l:
                frame.popFloat(&inst);
                frame.pushLong(&inst);
                break;
            case OPCODE_f2d:
                frame.popFloat(&inst);
                frame.pushDouble(&inst);
                break;
            case OPCODE_d2i:
                frame.popDouble(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_d2l:
                frame.popDouble(&inst);
                frame.pushLong(&inst);
                break;
            case OPCODE_d2f:
                frame.popDouble(&inst);
                frame.pushFloat(&inst);
                break;
            case OPCODE_i2b:
            case OPCODE_i2c:
            case OPCODE_i2s:
                frame.popIntegral(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_lcmp:
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_fcmpl:
            case OPCODE_fcmpg:
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_dcmpl:
            case OPCODE_dcmpg:
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_ifeq:
            case OPCODE_ifne:
            case OPCODE_iflt:
            case OPCODE_ifge:
            case OPCODE_ifgt:
            case OPCODE_ifle:
                frame.pop(&inst);
                break;
            case OPCODE_if_icmpeq:
            case OPCODE_if_icmpne:
            case OPCODE_if_icmplt:
            case OPCODE_if_icmpge:
            case OPCODE_if_icmpgt:
            case OPCODE_if_icmple:
                frame.popIntegral(&inst);
                frame.popIntegral(&inst);
                break;
            case OPCODE_if_acmpeq:
            case OPCODE_if_acmpne:
                frame.pop(&inst);
                frame.pop(&inst);
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
                frame.pop(&inst);
                break;
            case OPCODE_ireturn:
                frame.pop(&inst);
                break;
            case OPCODE_lreturn:
                frame.pop(&inst);
                frame.pop(&inst);
                break;
            case OPCODE_freturn:
                frame.pop(&inst);
                break;
            case OPCODE_dreturn:
                frame.pop(&inst);
                frame.pop(&inst);
                break;
            case OPCODE_areturn:
                frame.pop(&inst);
                break;
            case OPCODE_return:
                break;
            case OPCODE_getstatic: {
                const Type& t = fieldType(inst);
                frame.pushType(t, &inst);
                break;
            }
            case OPCODE_putstatic: {
                const Type& t = fieldType(inst);
                frame.popType(t, &inst);
                break;
            }
            case OPCODE_getfield: {
                const Type& t = fieldType(inst);
                frame.popRef(&inst);
                frame.pushType(t, &inst);
                break;
            }
            case OPCODE_putfield: {
                const Type& t = fieldType(inst);
                frame.popType(t, &inst);
                frame.popRef(&inst);
                break;
            }
            case OPCODE_invokevirtual:
                invokeMethod(inst.invoke()->methodRefIndex, true, false, &inst);
                break;
            case OPCODE_invokespecial:
                invokeSpecial(inst.invoke()->methodRefIndex, &inst);
                break;
            case OPCODE_invokestatic:
                invokeStatic(inst.invoke()->methodRefIndex, &inst);
                break;
            case OPCODE_invokeinterface:
                invokeInterface(inst.invokeinterface()->interMethodRefIndex, true, &inst);
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
                frame.pop(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_athrow:
                athrow(inst);
                break;
            case OPCODE_checkcast:
                checkcast(inst);
                break;
            case OPCODE_instanceof:
                frame.popRef(&inst);
                frame.pushInt(&inst);
                break;
            case OPCODE_monitorenter:
            case OPCODE_monitorexit:
                frame.popRef(&inst);
                break;
            case OPCODE_wide:
                wide(inst);
                break;
            case OPCODE_multianewarray:
                multianewarray(inst);
                break;
            case OPCODE_ifnull:
            case OPCODE_ifnonnull:
                frame.pop(&inst);
                break;
            case OPCODE_goto_w:
            case OPCODE_jsr_w:
            case OPCODE_breakpoint:
            case OPCODE_impdep1:
            case OPCODE_impdep2:
                JnifError::raise("goto_w, jsr_w breakpoint not implemented");
                break;
            case OPCODE_invokedynamic: {
                ConstPool::Index callSite = inst.indy()->callSite();
                const ConstPool::InvokeDynamic& dyn = cp.getInvokeDynamic(callSite);

                String name;
                String desc;
                cp.getNameAndType(dyn.nameAndTypeIndex, &name, &desc);

                //cerr << name << endl;
                //cerr << desc << endl;

                invoke("invokedynamic call site", name, desc, false, false, &inst);

                //Error::raise("invoke dynamic instances not implemented");
                break;
            }
            default:
                JnifError::raise("unknown opcode not implemented: ", inst.opcode);
            }
        }

    private:

        void newinst(TypeInst& inst) {
            const String& className = cp.getClassName(inst.type()->classIndex);
            const Type& t = TypeFactory::fromConstClass(className);
            t.init = false;
            t.typeId = Type::nextTypeId;
            Type::nextTypeId++;

            t.uninit.newinst = &inst;
            JnifError::check(!t.isArray(), "New with array: ", t);
            frame.push(t, &inst);

        }

        void newarray(Inst& inst) {
            frame.popIntegral(&inst);
            frame.pushArray(getArrayBaseType(inst.newarray()->atype), 1, &inst);
        }

        void anewarray(Inst& inst) {
            frame.popIntegral(&inst);
            const String& className = cp.getClassName(inst.type()->classIndex);
            const Type& t = TypeFactory::fromConstClass(className);
            frame.pushArray(t, t.getDims() + 1, &inst);
        }

        void aaload(Inst& inst) {
            frame.popIntegral(&inst);
            Type arrayType = frame.popArray(&inst);
            if (arrayType.isNull()) {
                frame.pushNull(&inst);
            } else {
                Type elementType = arrayType.elementType();
                JnifError::check(elementType.isObject(), "Not an object:", elementType);
                frame.push(elementType, &inst);
            }
        }

        void ldc(Inst& inst) {
            ConstPool::Tag tag = cp.getTag(inst.ldc()->valueIndex);
            switch (tag) {
            case ConstPool::INTEGER:
                frame.pushInt(&inst);
                break;
            case ConstPool::FLOAT:
                frame.pushFloat(&inst);
                break;
            case ConstPool::CLASS:
                frame.pushRef(cp.getClassName(inst.ldc()->valueIndex), &inst);
                break;
            case ConstPool::STRING:
                frame.pushRef("java/lang/String", &inst);
                break;
            default:
                JnifError::raise("Invalid tag entry: ", tag);
            }
        }

        void ldc2(Inst& inst) {
            ConstPool::Tag tag = cp.getTag(inst.ldc()->valueIndex);
            switch (tag) {
            case ConstPool::LONG:
                frame.pushLong(&inst);
                break;
            case ConstPool::DOUBLE:
                frame.pushDouble(&inst);
                break;
            default:
                JnifError::raise("Invalid constant for ldc2_w");
            }
        }

        void athrow(Inst& inst) {
            Type t = frame.popRef(&inst);
            frame.clearStack();
            frame.push(t, &inst);
        }

        void checkcast(Inst& inst) {
            frame.popRef(&inst);
            const String& className = cp.getClassName(inst.type()->classIndex);
            frame.push(TypeFactory::fromConstClass(className), &inst);
        }

        void istore(int lvindex, Inst* inst) {
            frame.popIntegral(inst);
            frame.setIntVar(lvindex, inst);
        }

        void lstore(int lvindex, Inst* inst) {
            frame.popLong(inst);
            frame.setLongVar(lvindex, inst);
        }

        void fstore(int lvindex, Inst* inst) {
            frame.popFloat(inst);
            frame.setFloatVar(lvindex, inst);
        }

        void dstore(int lvindex, Inst* inst) {
            frame.popDouble(inst);
            frame.setDoubleVar(lvindex, inst);
        }

        void astore(int lvindex, Inst* inst) {
            Type refType = frame.popRef(inst);
            JnifError::check(!refType.isTop(), "astore: Setting variable index ",
                             lvindex, " to Top", refType, " in frame ", frame);

            frame.setRefVar(lvindex, refType, inst);
        }

        void iload(u4 lvindex, Inst* inst, int offset = 0) {
            const Type& type = frame.getVar(lvindex, inst);
            JnifError::check(type.isIntegral(), "iload: ", type, " at index ", lvindex,
                             ":offset:", offset, " for ");
            frame.pushInt(inst);
        }

        void fload(u4 lvindex, Inst* inst) {
            const Type& type = frame.getVar(lvindex, inst);
            JnifError::check(type.isFloat(), "fload: ", type, " @ ", lvindex);
            frame.pushFloat(inst);
        }

        void lload(u4 lvindex, Inst* inst) {
            const Type& type = frame.getVar(lvindex, inst);
            JnifError::check(type.isLong(), "lload: ", type, " @ ", lvindex);
            frame.pushLong(inst);
        }

        void dload(u4 lvindex, Inst* inst) {
            const Type& type = frame.getVar(lvindex, inst);
            JnifError::check(type.isDouble(), "dload: ", type, " @ ", lvindex);
            frame.pushDouble(inst);
        }

        void aload(u4 lvindex, Inst* inst) {
            const Type& type = frame.getVar(lvindex, inst);
            JnifError::check(type.isObject() || type.isNull() || type.isUninitThis(),
                             "Bad ref var at index[", lvindex, "]: ", type, " @ frame: ",
                             frame);
            frame.pushType(type, inst);
        }

        void xastore(Inst* inst) {
            frame.popIntegral(inst);
            frame.popArray(inst);
        }

        void iinc(u2 index, Inst* inst) {
            Type type = frame.getVar(index, inst);
            JnifError::check(type.isInt());
            frame.setIntVar(index, inst);
        }

        void invokeMethod(u2 methodRefIndex, bool popThis, bool isSpecial, Inst* inst) {
            String className, name, desc;
            cp.getMethodRef(methodRefIndex, &className, &name, &desc);
            invoke(className, name, desc, popThis, isSpecial, inst);
        }

        void invokeSpecial(u2 methodRefIndex, Inst* inst) {
            ConstPool::Tag tag = cp.getTag(methodRefIndex);
            JnifError::check(tag == ConstPool::METHODREF || tag == ConstPool::INTERMETHODREF,
                             "INVOKESPECIAL index must be either a method or an interface method symbolic reference");
            if (tag == ConstPool::METHODREF) {
                invokeMethod(methodRefIndex, true, true, inst);
            } else {
                invokeInterface(methodRefIndex, true, inst);
            }
        }

        void invokeInterface(u2 interMethodRefIndex, bool popThis, Inst* inst) {
            String className, name, desc;
            cp.getInterMethodRef(interMethodRefIndex, &className, &name, &desc);
            invoke(className, name, desc, popThis, false, inst);
        }

        void invokeStatic(u2 methodRefIndex, Inst* inst) {
            ConstPool::Tag tag = cp.getTag(methodRefIndex);
            JnifError::check(tag == ConstPool::METHODREF || tag == ConstPool::INTERMETHODREF,
                             "INVOKESPECIAL index must be either a method or an interface method symbolic reference");
            if (tag == ConstPool::METHODREF) {
                invokeMethod(methodRefIndex, false, false, inst);
            } else {
                invokeInterface(methodRefIndex, false, inst);
            }
        }

        void invoke(const String& className, const String& name, const String& desc,
                    bool popThis, bool isSpecial, Inst* inst) {
            const char* d = desc.c_str();
            std::vector<Type> argsType;
            const Type& returnType = TypeFactory::fromMethodDesc(d, &argsType);

            for (int i = argsType.size() - 1; i >= 0; i--) {
                const Type& argType = argsType[i];
                JnifError::check(argType.isOneOrTwoWord(), "Invalid arg type in method");
                frame.popType(argType, inst);
            }

            if (popThis) {
                Type t = frame.popRef(inst);
                //JnifError::check(t.iso)
                if (isSpecial && name == "<init>") {
                    JnifError::check(t.typeId > 0, "inv typeId: ", t.typeId, t);
                    JnifError::check(!t.init, "Object is already init: ", t, ", ",
                                     className, ".", name, desc, ", frame: ", frame);

                    t.init = true;
                    frame.init(t);
                }
            }

            if (!returnType.isVoid()) {
                JnifError::assert(returnType.isOneOrTwoWord(), "Ret type: ", returnType);
                frame.pushType(returnType, inst);
            }
        }

        Type fieldType(Inst& inst) {
            String className, name, desc;
            cp.getFieldRef(inst.field()->fieldRefIndex, &className, &name, &desc);

            const char* d = desc.c_str();
            const Type& t = TypeFactory::fromFieldDesc(d);

            return t;
        }

        void multianewarray(Inst& inst) {
            u1 dims = inst.multiarray()->dims;
            JnifError::check(dims >= 1, "invalid dims: ", dims);

            for (int i = 0; i < dims; i++) {
                frame.popIntegral(&inst);
            }

            String arrayClassName = cp.getClassName(inst.multiarray()->classIndex);
            const char* d = arrayClassName.c_str();
            Type arrayType = TypeFactory::fromFieldDesc(d);

            frame.pushType(arrayType, &inst);
        }

        void wide(Inst& inst) {
            u2 lvindex = inst.wide()->var.lvindex;
            switch (inst.wide()->subOpcode) {
            case OPCODE_iload:
                iload(lvindex, &inst);
                break;
            case OPCODE_lload:
                lload(lvindex, &inst);
                break;
            case OPCODE_fload:
                fload(lvindex, &inst);
                break;
            case OPCODE_dload:
                dload(lvindex, &inst);
                break;
            case OPCODE_aload:
                aload(lvindex, &inst);
                break;
            case OPCODE_istore:
                istore(lvindex, &inst);
                break;
            case OPCODE_fstore:
                fstore(lvindex, &inst);
                break;
            case OPCODE_lstore:
                lstore(lvindex, &inst);
                break;
            case OPCODE_dstore:
                dstore(lvindex, &inst);
                break;
            case OPCODE_astore:
                astore(lvindex, &inst);
                break;
            case OPCODE_iinc:
                iinc(inst.wide()->iinc.index, &inst);
                break;
            default:
                JnifError::raise("Unsupported wide opcode: ",
                                 inst.wide()->subOpcode);
            }

        }

        const Type& getArrayBaseType(int atype) {
            switch (atype) {
            case NewArrayInst::NEWARRAYTYPE_BOOLEAN:
                return TypeFactory::booleanType();
            case NewArrayInst::NEWARRAYTYPE_CHAR:
                return TypeFactory::charType();
            case NewArrayInst::NEWARRAYTYPE_BYTE:
                return TypeFactory::byteType();
            case NewArrayInst::NEWARRAYTYPE_SHORT:
                return TypeFactory::shortType();
            case NewArrayInst::NEWARRAYTYPE_INT:
                return TypeFactory::intType();
            case NewArrayInst::NEWARRAYTYPE_FLOAT:
                return TypeFactory::floatType();
            case NewArrayInst::NEWARRAYTYPE_LONG:
                return TypeFactory::longType();
            case NewArrayInst::NEWARRAYTYPE_DOUBLE:
                return TypeFactory::doubleType();
            }

            JnifError::raise("invalid atype: ", atype);
        }

        TFrame& frame;
        const ConstPool& cp;

    };

}

#endif
