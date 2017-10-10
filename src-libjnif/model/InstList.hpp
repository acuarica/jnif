/*
 * InstList.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_INSTLIST_HPP
#define JNIF_INSTLIST_HPP

#include "Inst.hpp"
#include "Arena.hpp"

namespace jnif::model {

    class ClassFile;

/**
 * Represents the bytecode of a method.
 */
    class InstList {
        friend class CodeAttr;

    public:

        class Iterator {
            friend class InstList;

        public:

            Inst* operator*();

            Inst* operator->() const;

            bool friend operator==(const Iterator& lhs, const Iterator& rhs) {
                return lhs.position == rhs.position;
            }

            bool friend operator!=(const Iterator& lhs, const Iterator& rhs) {
                return lhs.position != rhs.position;
            }

            Iterator& operator++();

            Iterator& operator--();

        private:

            Iterator(Inst* position, Inst* last) :
                    position(position), last(last) {
            }

            Inst* position;
            Inst* last;
        };

        LabelInst* createLabel();

        void addLabel(LabelInst* inst, Inst* pos = nullptr);

        LabelInst* addLabel(Inst* pos = nullptr);

        ZeroInst* addZero(Opcode opcode, Inst* pos = nullptr);

        PushInst* addBiPush(u1 value, Inst* pos = nullptr);

        PushInst* addSiPush(u2 value, Inst* pos = nullptr);

        LdcInst* addLdc(Opcode opcode, ConstPool::Index valueIndex, Inst* pos = nullptr);

        VarInst* addVar(Opcode opcode, u1 lvindex, Inst* pos = nullptr);

        IincInst* addIinc(u1 index, u1 value, Inst* pos = nullptr);

        WideInst* addWideVar(Opcode varOpcode, u2 lvindex, Inst* pos = nullptr);

        WideInst* addWideIinc(u2 index, u2 value, Inst* pos = nullptr);

        JumpInst* addJump(Opcode opcode, LabelInst* targetLabel, Inst* pos = nullptr);

        FieldInst* addField(Opcode opcode, ConstPool::Index fieldRefIndex, Inst* pos = nullptr);

        InvokeInst* addInvoke(Opcode opcode, ConstPool::Index methodRefIndex, Inst* pos = nullptr);

        InvokeInterfaceInst* addInvokeInterface(ConstPool::Index interMethodRefIndex, u1 count, Inst* pos = nullptr);

        InvokeDynamicInst* addInvokeDynamic(ConstPool::Index callSite, Inst* pos = nullptr);

        TypeInst* addType(Opcode opcode, ConstPool::Index classIndex, Inst* pos = nullptr);

        NewArrayInst* addNewArray(u1 atype, Inst* pos = nullptr);

        MultiArrayInst* addMultiArray(ConstPool::Index classIndex, u1 dims, Inst* pos = nullptr);

        TableSwitchInst* addTableSwitch(LabelInst* def, int low, int high, Inst* pos = nullptr);

        LookupSwitchInst* addLookupSwitch(LabelInst* def, u4 npairs, Inst* pos = nullptr);

        bool hasBranches() const {
            return branchesCount > 0;
        }

        bool hasJsrOrRet() const {
            return jsrOrRet;
        }

        int size() const {
            return _size;
        }

        Iterator begin() const {
            return Iterator(first, last);
        }

        Iterator end() const {
            return Iterator(nullptr, last);
        }

        Inst* getInst(int offset);

        ClassFile* const constPool;

    private:

        InstList(ClassFile* arena) :
                constPool(arena), first(nullptr), last(nullptr), _size(0), nextLabelId(1), branchesCount(0),
                jsrOrRet(false) {
        }

        ~InstList();

        void addInst(Inst* inst, Inst* pos);

        Inst* first;
        Inst* last;

        int _size;

        int nextLabelId;

        int branchesCount;

        bool jsrOrRet;

        template<typename TInst, typename ... TArgs>
        TInst* _create(const TArgs& ... args);

    };

    std::ostream& operator<<(std::ostream& os, const InstList& instList);

}

#endif
