/*
 * CodeAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_CODEATTRPARSER_HPP
#define JNIF_PARSER_CODEATTRPARSER_HPP

#include "LabelManager.hpp"
#include "AttrsParser.hpp"

namespace jnif {

    namespace parser {

        OpKind OPKIND[256] = {KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_BIPUSH,
                              KIND_SIPUSH, KIND_LDC, KIND_LDC, KIND_LDC, KIND_VAR, KIND_VAR, KIND_VAR,
                              KIND_VAR, KIND_VAR, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_VAR, KIND_VAR, KIND_VAR, KIND_VAR, KIND_VAR, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_IINC,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_JUMP, KIND_JUMP, KIND_JUMP, KIND_JUMP,
                              KIND_JUMP, KIND_JUMP, KIND_JUMP, KIND_JUMP, KIND_JUMP, KIND_JUMP,
                              KIND_JUMP, KIND_JUMP, KIND_JUMP, KIND_JUMP, KIND_JUMP, KIND_JUMP,
                              KIND_VAR, KIND_TABLESWITCH, KIND_LOOKUPSWITCH, KIND_ZERO, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_FIELD, KIND_FIELD,
                              KIND_FIELD, KIND_FIELD, KIND_INVOKE, KIND_INVOKE, KIND_INVOKE,
                              KIND_INVOKEINTERFACE, KIND_INVOKEDYNAMIC, KIND_TYPE, KIND_NEWARRAY,
                              KIND_TYPE, KIND_ZERO, KIND_ZERO, KIND_TYPE, KIND_TYPE, KIND_ZERO,
                              KIND_ZERO, KIND_ZERO, KIND_MULTIARRAY, KIND_JUMP, KIND_JUMP,
                              KIND_PARSE4TODO, KIND_PARSE4TODO, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,
                              KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED,};

        template<typename ... TAttrParserList>
        class CodeAttrParser {
        public:

            static constexpr const char* AttrName = "Code";

            void parseInstTargets(BufferReader& br, LabelManager& labelManager) {
                while (!br.eor()) {
                    int offset = br.offset();

                    Opcode opcode = (Opcode) br.readu1();
                    OpKind kind = OPKIND[(int) opcode];

                    switch (kind) {
                        case KIND_ZERO:
                            if (opcode == Opcode::wide) {
                                Opcode opcodew = (Opcode) br.readu1();
                                if (opcodew == Opcode::iinc) {
                                    br.skip(4);
                                } else {
                                    br.skip(2);
                                }
                            }
                            break;
                        case KIND_BIPUSH:
                        case KIND_VAR:
                        case KIND_NEWARRAY:
                            br.skip(1);
                            break;
                        case KIND_SIPUSH:
                        case KIND_IINC:
                        case KIND_FIELD:
                        case KIND_INVOKE:
                        case KIND_TYPE:
                            br.skip(2);
                            break;
                        case KIND_MULTIARRAY:
                            br.skip(3);
                            break;
                        case KIND_INVOKEINTERFACE:
                            br.skip(4);
                            break;
                        case KIND_INVOKEDYNAMIC:
                            br.skip(4);
                            break;
                        case KIND_LDC:
                            if (opcode == Opcode::ldc) {
                                br.readu1();
                            } else {
                                br.readu2();
                            }
                            break;
                        case KIND_JUMP: {
                            short targetOffset = br.readu2();
                            short labelpos = offset + targetOffset;

                            JnifError::assert(labelpos >= 0, "invalid target for jump: must be >= 0");
                            JnifError::assert(labelpos < br.size(), "invalid target for jump");

                            labelManager.createLabel(labelpos);
                            break;
                        }
                        case KIND_TABLESWITCH: {
                            for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
                                u1 pad = br.readu1();
                                JnifError::assert(pad == 0, "Padding must be zero");
                            }

                            {
                                bool check2 = br.offset() % 4 == 0;
                                JnifError::assert(check2, "%d", br.offset());
                            }

                            int defOffset = br.readu4();
                            labelManager.createLabel(offset + defOffset);

                            int low = br.readu4();
                            int high = br.readu4();

                            JnifError::assert(low <= high, "low (%d) must be less or equal than high (%d)", low, high);

                            for (int i = 0; i < high - low + 1; i++) {
                                int targetOffset = br.readu4();
                                labelManager.createLabel(offset + targetOffset);
                            }
                            break;
                        }
                        case KIND_LOOKUPSWITCH: {
                            for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
                                u1 pad = br.readu1();
                                JnifError::assert(pad == 0, "Padding must be zero");
                            }

                            int defOffset = br.readu4();
                            labelManager.createLabel(offset + defOffset);

                            u4 npairs = br.readu4();

                            for (u4 i = 0; i < npairs; i++) {
                                br.readu4(); // Key

                                int targetOffset = br.readu4();
                                labelManager.createLabel(offset + targetOffset);
                            }
                            break;
                        }
//			case KIND_RESERVED:
                            //			break;
                        default:
                            throw JnifException("default kind in parseInstTargets: "
                                                     "opcode: ", opcode, ", kind: ", kind);
                    }
                }
            }

            Inst* parseInst(BufferReader& br, InstList& instList,
                            const LabelManager& labelManager) {
                int offset = br.offset();

//		if (labelManager.hasLabel(offset)) {
//			LabelInst* label = labelManager[offset];
//			label->_offset = offset;
//			instList.addLabel(label);
//		}

                labelManager.putLabelIfExists(offset);

                Opcode opcode = (Opcode) br.readu1();
                u1 kind = OPKIND[(int) opcode];

                //Inst* instp = new Inst(opcode, kind);
                //instp->_offset = offset;
                //Inst& inst = *instp;

                //inst.opcode = (Opcode) br.readu1();
                //inst.kind = OPKIND[inst.opcode];

                if (kind == KIND_ZERO) {
                    if (opcode == Opcode::wide) {
                        Opcode subOpcode = (Opcode) br.readu1();
                        if (subOpcode == Opcode::iinc) {
                            u2 index = br.readu2();
                            u2 value = br.readu2();

                            return instList.addWideIinc(index, value);
                        } else {
                            u2 lvindex = br.readu2();
                            return instList.addWideVar(subOpcode, lvindex);
                        }
                    } else {
                        return instList.addZero(opcode);
                    }
                } else if (kind == KIND_BIPUSH) {
                    u1 value = br.readu1();
                    return instList.addBiPush(value);
                } else if (kind == KIND_SIPUSH) {
                    u2 value = br.readu2();
                    return instList.addSiPush(value);
                } else if (kind == KIND_LDC) {
                    u2 valueIndex;
                    if (opcode == Opcode::ldc) {
                        valueIndex = br.readu1();
                    } else {
                        valueIndex = br.readu2();
                    }
                    return instList.addLdc(opcode, valueIndex);
                } else if (kind == KIND_VAR) {
                    u1 lvindex = br.readu1();
                    return instList.addVar(opcode, lvindex);
                } else if (kind == KIND_IINC) {
                    u1 index = br.readu1();
                    u1 value = br.readu1();
                    return instList.addIinc(index, value);
                } else if (kind == KIND_JUMP) {
                    //
                    short targetOffset = br.readu2();
                    //inst.jump.label = targetOffset;

                    short labelpos = offset + targetOffset;
                    JnifError::check(labelpos >= 0,
                                     "invalid target for jump: must be >= 0");
                    JnifError::check(labelpos < br.size(), "invalid target for jump");

                    //	fprintf(stderr, "target offset @ parse: %d\n", targetOffset);

                    LabelInst* targetLabel = labelManager[offset + targetOffset];
                    JnifError::check(targetLabel != NULL, "invalid label");

                    return instList.addJump(opcode, targetLabel);
                } else if (kind == KIND_TABLESWITCH) {
                    for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
                        u1 pad = br.readu1();
                        JnifError::assert(pad == 0, "Padding must be zero");
                    }

                    {
                        bool check2 = br.offset() % 4 == 0;
                        JnifError::assert(check2, "%d", br.offset());
                    }

                    int defOffset = br.readu4();
                    LabelInst* def = labelManager[offset + defOffset];
                    int low = br.readu4();
                    int high = br.readu4();

                    JnifError::assert(low <= high,
                                      "low (%d) must be less or equal than high (%d)", low, high);

                    TableSwitchInst* ts = instList.addTableSwitch(def, low, high);
                    for (int i = 0; i < high - low + 1; i++) {
                        u4 targetOffset = br.readu4();
                        //ts->targets.push_back(labelManager[offset + targetOffset]);
                        ts->addTarget(labelManager[offset + targetOffset]);
                    }

                    //		fprintf(stderr, "parser ts: offset: %d\n", br.offset());

                    return ts;
                } else if (kind == KIND_LOOKUPSWITCH) {
                    for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
                        u1 pad = br.readu1();
                        JnifError::assert(pad == 0, "Padding must be zero");
                    }

                    int defOffset = br.readu4();
                    LabelInst* defbyte = labelManager[offset + defOffset];
                    u4 npairs = br.readu4();

                    LookupSwitchInst* ls = instList.addLookupSwitch(defbyte, npairs);
                    for (u4 i = 0; i < npairs; i++) {
                        u4 key = br.readu4();
                        u4 offsetTarget = br.readu4();

                        ls->keys.push_back(key);
                        //ls->targets.push_back(labelManager[offset + offsetTarget]);
                        ls->addTarget(labelManager[offset + offsetTarget]);
                    }

                    return ls;
                } else if (kind == KIND_FIELD) {
                    u2 fieldRefIndex = br.readu2();

                    return instList.addField(opcode, fieldRefIndex);
                } else if (kind == KIND_INVOKE) {
                    u2 methodRefIndex = br.readu2();

                    return instList.addInvoke(opcode, methodRefIndex);
                } else if (kind == KIND_INVOKEINTERFACE) {
                    JnifError::assert(opcode == Opcode::invokeinterface, "invalid opcode");

                    u2 interMethodRefIndex = br.readu2();
                    u1 count = br.readu1();

                    JnifError::assert(count != 0, "Count is zero!");

                    u1 zero = br.readu1();
                    JnifError::assert(zero == 0, "Fourth operand must be zero");

                    return instList.addInvokeInterface(interMethodRefIndex, count);
                } else if (kind == KIND_INVOKEDYNAMIC) {
                    u2 callSite = br.readu2();
                    u2 zero = br.readu2();
                    JnifError::check(zero == 0, "Zero is not zero: ", zero);

                    return instList.addInvokeDynamic(callSite);
                } else if (kind == KIND_TYPE) {
                    ConstPool::Index classIndex = br.readu2();

                    return instList.addType(opcode, classIndex);
                } else if (kind == KIND_NEWARRAY) {
                    u1 atype = br.readu1();

                    return instList.addNewArray(atype);
                } else if (kind == KIND_MULTIARRAY) {
                    ConstPool::Index classIndex = br.readu2();
                    u1 dims = br.readu1();

                    return instList.addMultiArray(classIndex, dims);
                } else if (kind == KIND_PARSE4TODO) {
                    throw JnifException("FrParse4__TODO__Instr not implemented");
                } else if (kind == KIND_RESERVED) {
                    throw JnifException("FrParseReservedInstr not implemented");
                } else {
                    throw JnifException("default kind in parseInstList");
                }
            }

            void parseInstList(BufferReader& br, InstList& instList,
                               const LabelManager& labelManager) {
                while (!br.eor()) {
                    int offset = br.offset();
                    Inst* inst = parseInst(br, instList, labelManager);
                    inst->_offset = offset;
                }
            }

            Attr* parse(BufferReader* br, ClassFile* cp, u2 nameIndex) {

                CodeAttr* ca = cp->_arena.create<CodeAttr>(nameIndex, cp);

                ca->maxStack = br->readu2();
                ca->maxLocals = br->readu2();

                u4 codeLen = br->readu4();

                JnifError::check(codeLen > 0, "");
                JnifError::check(codeLen < (2 << 16), "");

                ca->codeLen = codeLen;

                const u1* codeBuf = br->pos();
                br->skip(ca->codeLen);

                LabelManager labelManager(codeLen, ca->instList);

                {
                    BufferReader br(codeBuf, codeLen);
                    parseInstTargets(br, labelManager);
                }

                u2 exceptionTableCount = br->readu2();
                for (int i = 0; i < exceptionTableCount; i++) {
                    u2 startPc = br->readu2();
                    u2 endPc = br->readu2();
                    u2 handlerPc = br->readu2();
                    ConstPool::Index catchType = br->readu2();

                    JnifError::check(startPc < endPc, "");
                    JnifError::check(endPc <= ca->codeLen, "");
                    JnifError::check(handlerPc < ca->codeLen, "");
                    JnifError::check(catchType == ConstPool::NULLENTRY || cp->isClass(catchType), "");

                    ca->exceptions.push_back({
                                                     labelManager.createExceptionLabel(startPc, true, false, false),
                                                     labelManager.createExceptionLabel(endPc, false, true, false),
                                                     labelManager.createExceptionLabel(handlerPc, false, false, true),
                                                     catchType
                                             });
                }

                AttrsParser<TAttrParserList ...>().parse(br, cp, &ca->attrs, &labelManager);

                {
                    BufferReader br(codeBuf, codeLen);
                    parseInstList(br, ca->instList, labelManager);
                }

                labelManager.putLabelIfExists(codeLen);

                return ca;
            }

        };

    }
}

#endif
