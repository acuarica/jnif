/*
 * SmtBuilder.hpp
 *
 *  Created on: Jun 25, 2014
 *      Author: luigi
 */

#ifndef JNIF_ANALYSIS_SMTBUILDER_HPP
#define JNIF_ANALYSIS_SMTBUILDER_HPP

#include <jnif.hpp>

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
            case Opcode::nop:
                break;
            case Opcode::aconst_null:
                frame.pushNull(&inst);
                break;
            case Opcode::iconst_m1:
            case Opcode::iconst_0:
            case Opcode::iconst_1:
            case Opcode::iconst_2:
            case Opcode::iconst_3:
            case Opcode::iconst_4:
            case Opcode::iconst_5:
            case Opcode::bipush:
            case Opcode::sipush:
                frame.pushInt(&inst);
                break;
            case Opcode::lconst_0:
            case Opcode::lconst_1:
                frame.pushLong(&inst);
                break;
            case Opcode::fconst_0:
            case Opcode::fconst_1:
            case Opcode::fconst_2:
                frame.pushFloat(&inst);
                break;
            case Opcode::dconst_0:
            case Opcode::dconst_1:
                frame.pushDouble(&inst);
                break;
            case Opcode::ldc:
            case Opcode::ldc_w:
                ldc(inst);
                break;
            case Opcode::ldc2_w:
                ldc2(inst);
                break;
            case Opcode::iload:
                iload(inst.var()->lvindex, &inst);
                break;
            case Opcode::iload_0:
                iload(0, &inst);
                break;
            case Opcode::iload_1:
                iload(1, &inst);
                break;
            case Opcode::iload_2:
                iload(2, &inst);
                break;
            case Opcode::iload_3:
                iload(3, &inst);
                break;
            case Opcode::lload:
                lload(inst.var()->lvindex, &inst);
                break;
            case Opcode::lload_0:
                lload(0, &inst);
                break;
            case Opcode::lload_1:
                lload(1, &inst);
                break;
            case Opcode::lload_2:
                lload(2, &inst);
                break;
            case Opcode::lload_3:
                lload(3, &inst);
                break;
            case Opcode::fload:
                fload(inst.var()->lvindex, &inst);
                break;
            case Opcode::fload_0:
                fload(0, &inst);
                break;
            case Opcode::fload_1:
                fload(1, &inst);
                break;
            case Opcode::fload_2:
                fload(2, &inst);
                break;
            case Opcode::fload_3:
                fload(3, &inst);
                break;
            case Opcode::dload:
                dload(inst.var()->lvindex, &inst);
                break;
            case Opcode::dload_0:
                dload(0, &inst);
                break;
            case Opcode::dload_1:
                dload(1, &inst);
                break;
            case Opcode::dload_2:
                dload(2, &inst);
                break;
            case Opcode::dload_3:
                dload(3, &inst);
                break;
            case Opcode::aload:
                aload(inst.var()->lvindex, &inst);
                break;
            case Opcode::aload_0:
                aload(0, &inst);
                break;
            case Opcode::aload_1:
                aload(1, &inst);
                break;
            case Opcode::aload_2:
                aload(2, &inst);
                break;
            case Opcode::aload_3:
                aload(3, &inst);
                break;
            case Opcode::iaload:
            case Opcode::baload:
            case Opcode::caload:
            case Opcode::saload:
                frame.popIntegral(&inst);
                frame.popArray(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::laload:
                frame.popIntegral(&inst);
                frame.popArray(&inst);
                frame.pushLong(&inst);
                break;
            case Opcode::faload:
                frame.popIntegral(&inst);
                frame.popArray(&inst);
                frame.pushFloat(&inst);
                break;
            case Opcode::daload: {
                frame.popIntegral(&inst);
                Type arrayType = frame.popArray(&inst);
                frame.pushDouble(&inst);
                break;
            }
            case Opcode::aaload:
                aaload(inst);
                break;
            case Opcode::istore:
                istore(inst.var()->lvindex, &inst);
                break;
            case Opcode::lstore:
                lstore(inst.var()->lvindex, &inst);
                break;
            case Opcode::fstore:
                fstore(inst.var()->lvindex, &inst);
                break;
            case Opcode::dstore:
                dstore(inst.var()->lvindex, &inst);
                break;
            case Opcode::astore:
                astore(inst.var()->lvindex, &inst);
                break;
            case Opcode::istore_0:
                istore(0, &inst);
                break;
            case Opcode::istore_1:
                istore(1, &inst);
                break;
            case Opcode::istore_2:
                istore(2, &inst);
                break;
            case Opcode::istore_3:
                istore(3, &inst);
                break;
            case Opcode::lstore_0:
                lstore(0, &inst);
                break;
            case Opcode::lstore_1:
                lstore(1, &inst);
                break;
            case Opcode::lstore_2:
                lstore(2, &inst);
                break;
            case Opcode::lstore_3:
                lstore(3, &inst);
                break;
            case Opcode::fstore_0:
                fstore(0, &inst);
                break;
            case Opcode::fstore_1:
                fstore(1, &inst);
                break;
            case Opcode::fstore_2:
                fstore(2, &inst);
                break;
            case Opcode::fstore_3:
                fstore(3, &inst);
                break;
            case Opcode::dstore_0:
                dstore(0, &inst);
                break;
            case Opcode::dstore_1:
                dstore(1, &inst);
                break;
            case Opcode::dstore_2:
                dstore(2, &inst);
                break;
            case Opcode::dstore_3:
                dstore(3, &inst);
                break;
            case Opcode::astore_0:
                astore(0, &inst);
                break;
            case Opcode::astore_1:
                astore(1, &inst);
                break;
            case Opcode::astore_2:
                astore(2, &inst);
                break;
            case Opcode::astore_3:
                astore(3, &inst);
                break;
            case Opcode::iastore:
            case Opcode::bastore:
            case Opcode::castore:
            case Opcode::sastore:
                frame.popIntegral(&inst);
                xastore(&inst);
                break;
            case Opcode::lastore:
                frame.popLong(&inst);
                xastore(&inst);
                break;
            case Opcode::fastore:
                frame.popFloat(&inst);
                xastore(&inst);
                break;
            case Opcode::dastore:
                frame.popDouble(&inst);
                xastore(&inst);
                break;
            case Opcode::aastore:
                frame.popRef(&inst);
                xastore(&inst);
                break;
            case Opcode::pop:
                frame.popOneWord(&inst);
                break;
            case Opcode::pop2:
                frame.popTwoWord(&inst);
                break;
            case Opcode::dup: {
                const Type& t1 = frame.popOneWord(&inst);
                frame.push(t1, &inst);
                frame.push(t1, &inst);
                break;
            }
            case Opcode::dup_x1: {
                const Type& t1 = frame.popOneWord(&inst);
                const Type& t2 = frame.popOneWord(&inst);
                frame.push(t1, &inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                break;
            }
            case Opcode::dup_x2: {
                const Type& t1 = frame.pop(&inst);
                const Type& t2 = frame.pop(&inst);
                const Type& t3 = frame.pop(&inst);
                frame.push(t1, &inst);
                frame.push(t3, &inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                break;
            }
            case Opcode::dup2: {
                const Type& t1 = frame.pop(&inst);
                const Type& t2 = frame.pop(&inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                frame.push(t2, &inst);
                frame.push(t1, &inst);
                break;
            }
            case Opcode::dup2_x1: {
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
            case Opcode::dup2_x2: {
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
            case Opcode::swap: {
                const Type& t1 = frame.pop(&inst);
                const Type& t2 = frame.pop(&inst);
                frame.push(t1, &inst);
                frame.push(t2, &inst);
                break;
            }
            case Opcode::iadd:
            case Opcode::fadd:
            case Opcode::isub:
            case Opcode::fsub:
            case Opcode::imul:
            case Opcode::fmul:
            case Opcode::idiv:
            case Opcode::fdiv:
            case Opcode::irem:
            case Opcode::frem:
            case Opcode::ishl:
            case Opcode::ishr:
            case Opcode::iushr:
            case Opcode::iand:
            case Opcode::ior:
            case Opcode::ixor: {
                const Type& t1 = frame.pop(&inst);
                frame.pop(&inst);
                frame.push(t1, &inst);
                break;
            }
            case Opcode::ladd:
            case Opcode::lsub:
            case Opcode::lmul:
            case Opcode::ldiv:
            case Opcode::lrem:
            case Opcode::land:
            case Opcode::lor:
            case Opcode::lxor:
                frame.popLong(&inst);
                frame.popLong(&inst);
                frame.pushLong(&inst);
                break;
            case Opcode::lshl:
            case Opcode::lshr:
            case Opcode::lushr:
                frame.popIntegral(&inst);
                frame.popLong(&inst);
                frame.pushLong(&inst);
                break;
            case Opcode::dadd:
            case Opcode::dsub:
            case Opcode::dmul:
            case Opcode::ddiv:
            case Opcode::drem: {
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pushDouble(&inst);
                break;
            }
            case Opcode::ineg:
            case Opcode::fneg: {
                const Type& t1 = frame.pop(&inst);
                frame.push(t1, &inst);
                break;
            }
            case Opcode::lneg: {
                frame.popLong(&inst);
                frame.pushLong(&inst);
                break;
            }
            case Opcode::dneg: {
                frame.popDouble(&inst);
                frame.pushDouble(&inst);
                break;
            }
            case Opcode::iinc:
                iinc(inst.iinc()->index, &inst);
                break;
            case Opcode::i2l:
                frame.popIntegral(&inst);
                frame.pushLong(&inst);
                break;
            case Opcode::i2f:
                frame.popIntegral(&inst);
                frame.pushFloat(&inst);
                break;
            case Opcode::i2d:
                frame.popIntegral(&inst);
                frame.pushDouble(&inst);
                break;
            case Opcode::l2i:
                frame.popLong(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::l2f:
                frame.popLong(&inst);
                frame.pushFloat(&inst);
                break;
            case Opcode::l2d:
                frame.popLong(&inst);
                frame.pushDouble(&inst);
                break;
            case Opcode::f2i:
                frame.popFloat(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::f2l:
                frame.popFloat(&inst);
                frame.pushLong(&inst);
                break;
            case Opcode::f2d:
                frame.popFloat(&inst);
                frame.pushDouble(&inst);
                break;
            case Opcode::d2i:
                frame.popDouble(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::d2l:
                frame.popDouble(&inst);
                frame.pushLong(&inst);
                break;
            case Opcode::d2f:
                frame.popDouble(&inst);
                frame.pushFloat(&inst);
                break;
            case Opcode::i2b:
            case Opcode::i2c:
            case Opcode::i2s:
                frame.popIntegral(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::lcmp:
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::fcmpl:
            case Opcode::fcmpg:
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::dcmpl:
            case Opcode::dcmpg:
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pop(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::ifeq:
            case Opcode::ifne:
            case Opcode::iflt:
            case Opcode::ifge:
            case Opcode::ifgt:
            case Opcode::ifle:
                frame.pop(&inst);
                break;
            case Opcode::if_icmpeq:
            case Opcode::if_icmpne:
            case Opcode::if_icmplt:
            case Opcode::if_icmpge:
            case Opcode::if_icmpgt:
            case Opcode::if_icmple:
                frame.popIntegral(&inst);
                frame.popIntegral(&inst);
                break;
            case Opcode::if_acmpeq:
            case Opcode::if_acmpne:
                frame.pop(&inst);
                frame.pop(&inst);
                break;
            case Opcode::GOTO:
                break;
            case Opcode::jsr:
                throw JsrRetNotSupported();
                break;
            case Opcode::ret:
                throw JsrRetNotSupported();
                break;
            case Opcode::tableswitch:
            case Opcode::lookupswitch:
                frame.pop(&inst);
                break;
            case Opcode::ireturn:
                frame.pop(&inst);
                break;
            case Opcode::lreturn:
                frame.pop(&inst);
                frame.pop(&inst);
                break;
            case Opcode::freturn:
                frame.pop(&inst);
                break;
            case Opcode::dreturn:
                frame.pop(&inst);
                frame.pop(&inst);
                break;
            case Opcode::areturn:
                frame.pop(&inst);
                break;
            case Opcode::RETURN:
                break;
            case Opcode::getstatic: {
                const Type& t = fieldType(inst);
                frame.pushType(t, &inst);
                break;
            }
            case Opcode::putstatic: {
                const Type& t = fieldType(inst);
                frame.popType(t, &inst);
                break;
            }
            case Opcode::getfield: {
                const Type& t = fieldType(inst);
                frame.popRef(&inst);
                frame.pushType(t, &inst);
                break;
            }
            case Opcode::putfield: {
                const Type& t = fieldType(inst);
                frame.popType(t, &inst);
                frame.popRef(&inst);
                break;
            }
            case Opcode::invokevirtual:
                invokeMethod(inst.invoke()->methodRefIndex, true, false, &inst);
                break;
            case Opcode::invokespecial:
                invokeSpecial(inst.invoke()->methodRefIndex, &inst);
                break;
            case Opcode::invokestatic:
                invokeStatic(inst.invoke()->methodRefIndex, &inst);
                break;
            case Opcode::invokeinterface:
                invokeInterface(inst.invokeinterface()->interMethodRefIndex, true, &inst);
                break;
            case Opcode::NEW:
                newinst(*inst.type());
                break;
            case Opcode::newarray:
                newarray(inst);
                break;
            case Opcode::anewarray:
                anewarray(inst);
                break;
            case Opcode::arraylength:
                frame.pop(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::athrow:
                athrow(inst);
                break;
            case Opcode::checkcast:
                checkcast(inst);
                break;
            case Opcode::instanceof:
                frame.popRef(&inst);
                frame.pushInt(&inst);
                break;
            case Opcode::monitorenter:
            case Opcode::monitorexit:
                frame.popRef(&inst);
                break;
            case Opcode::wide:
                wide(inst);
                break;
            case Opcode::multianewarray:
                multianewarray(inst);
                break;
            case Opcode::ifnull:
            case Opcode::ifnonnull:
                frame.pop(&inst);
                break;
            case Opcode::goto_w:
            case Opcode::jsr_w:
            case Opcode::breakpoint:
            case Opcode::impdep1:
            case Opcode::impdep2:
                throw Exception("goto_w, jsr_w breakpoint not implemented");
                break;
            case Opcode::invokedynamic: {
                ConstPool::Index callSite = inst.indy()->callSite();
                const ConstPool::InvokeDynamic& dyn = cp.getInvokeDynamic(callSite);

                string name;
                string desc;
                cp.getNameAndType(dyn.nameAndTypeIndex, &name, &desc);

                //cerr << name << endl;
                //cerr << desc << endl;

                invoke("invokedynamic call site", name, desc, false, false, &inst);

                //Error::raise("invoke dynamic instances not implemented");
                break;
            }
            default:
                throw Exception("unknown opcode not implemented: ", inst.opcode);
            }
        }

    private:

        void newinst(TypeInst& inst) {
            const string& className = cp.getClassName(inst.type()->classIndex);
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
            const string& className = cp.getClassName(inst.type()->classIndex);
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
                throw Exception("Invalid tag entry: ", tag);
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
                throw Exception("Invalid constant for ldc2_w");
            }
        }

        void athrow(Inst& inst) {
            Type t = frame.popRef(&inst);
            frame.clearStack();
            frame.push(t, &inst);
        }

        void checkcast(Inst& inst) {
            frame.popRef(&inst);
            const string& className = cp.getClassName(inst.type()->classIndex);
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
            string className, name, desc;
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
            string className, name, desc;
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

        void invoke(const string& className, const string& name, const string& desc,
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
            string className, name, desc;
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

            string arrayClassName = cp.getClassName(inst.multiarray()->classIndex);
            const char* d = arrayClassName.c_str();
            Type arrayType = TypeFactory::fromFieldDesc(d);

            frame.pushType(arrayType, &inst);
        }

        void wide(Inst& inst) {
            u2 lvindex = inst.wide()->var.lvindex;
            switch (inst.wide()->subOpcode) {
            case Opcode::iload:
                iload(lvindex, &inst);
                break;
            case Opcode::lload:
                lload(lvindex, &inst);
                break;
            case Opcode::fload:
                fload(lvindex, &inst);
                break;
            case Opcode::dload:
                dload(lvindex, &inst);
                break;
            case Opcode::aload:
                aload(lvindex, &inst);
                break;
            case Opcode::istore:
                istore(lvindex, &inst);
                break;
            case Opcode::fstore:
                fstore(lvindex, &inst);
                break;
            case Opcode::lstore:
                lstore(lvindex, &inst);
                break;
            case Opcode::dstore:
                dstore(lvindex, &inst);
                break;
            case Opcode::astore:
                astore(lvindex, &inst);
                break;
            case Opcode::iinc:
                iinc(inst.wide()->iinc.index, &inst);
                break;
            default:
                throw Exception("Unsupported wide opcode: ", inst.wide()->subOpcode);
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

            throw Exception("invalid atype: ", atype);
        }

        TFrame& frame;
        const ConstPool& cp;

    };

}

#endif
