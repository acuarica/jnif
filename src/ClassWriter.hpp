/*
 * ClassWriter.hpp
 *
 *  Created on: Apr 4, 2014
 *      Author: luigi
 */

#ifndef JNIF_CLASSWRITER_HPP
#define JNIF_CLASSWRITER_HPP

namespace jnif {

template<typename TWriter>
class ClassWriter {
public:

	ClassWriter(TWriter& bw) :
			bw(bw) {
	}

	void writeClassFile(ClassFile& cf) {
		bw.writeu4(CLASSFILE_MAGIC);

		Version version = cf.getVersion();
		bw.writeu2(version.getMinor());
		bw.writeu2(version.getMajor());

		writeConstPool(cf);

		bw.writeu2(cf.accessFlags);
		bw.writeu2(cf.thisClassIndex);
		bw.writeu2(cf.superClassIndex);

		u2 interCount = cf.interfaces.size();
		bw.writeu2(interCount);

		for (u4 i = 0; i < cf.interfaces.size(); i++) {
			u2 interIndex = cf.interfaces[i];
			bw.writeu2(interIndex);
		}

		bw.writeu2(cf.fields.size());
		for (Field& f : cf.fields) {
			bw.writeu2(f.accessFlags);
			bw.writeu2(f.nameIndex);
			bw.writeu2(f.descIndex);

			writeAttrs(f);
		}

		bw.writeu2(cf.methods.size());
		for (Method& m : cf.methods) {
			bw.writeu2(m.accessFlags);
			bw.writeu2(m.nameIndex);
			bw.writeu2(m.descIndex);

			writeAttrs(m);
		}

		writeAttrs(cf);
	}

	void writeConstPool(const ConstPool& cp) {
		u2 count = cp.size();
		bw.writeu2(count);

		for (ConstPool::Iterator it = cp.iterator(); it.hasNext(); it++) {
			ConstPool::Index i = *it;
			const ConstPool::Entry* entry = &cp.entries[i];

			bw.writeu1(entry->tag);

			switch (entry->tag) {
				case CONSTANT_Class:
					bw.writeu2(entry->clazz.nameIndex);
					break;
				case CONSTANT_Fieldref:
					bw.writeu2(entry->fieldRef.classIndex);
					bw.writeu2(entry->fieldRef.nameAndTypeIndex);
					break;
				case CONSTANT_Methodref:
					bw.writeu2(entry->methodRef.classIndex);
					bw.writeu2(entry->methodRef.nameAndTypeIndex);
					break;
				case CONSTANT_InterfaceMethodref:
					bw.writeu2(entry->interMethodRef.classIndex);
					bw.writeu2(entry->interMethodRef.nameAndTypeIndex);
					break;
				case CONSTANT_String:
					bw.writeu2(entry->s.stringIndex);
					break;
				case CONSTANT_Integer:
					bw.writeu4(entry->i.value);
					break;
				case CONSTANT_Float:
					bw.writeu4(entry->f.value);
					break;
				case CONSTANT_Long: {
					long value = cp.getLong(i);
					bw.writeu4(value >> 32);
					bw.writeu4(value & 0xffffffff);
					//		i++;
					break;
				}
				case CONSTANT_Double: {
					double dvalue = cp.getDouble(i);
					long value = *(long*) &dvalue;
					bw.writeu4(value >> 32);
					bw.writeu4(value & 0xffffffff);
					//			i++;
					break;
				}
				case CONSTANT_NameAndType:
					bw.writeu2(entry->nameandtype.nameIndex);
					bw.writeu2(entry->nameandtype.descriptorIndex);
					break;
				case CONSTANT_Utf8: {
					u2 len = entry->utf8.str.length();
					const char* str = entry->utf8.str.c_str();
					bw.writeu2(len);
					bw.writecount(str, len);
					break;
				}
				case CONSTANT_MethodHandle:
					bw.writeu1(entry->methodhandle.referenceKind);
					bw.writeu2(entry->methodhandle.referenceIndex);
					break;
				case CONSTANT_MethodType:
					bw.writeu2(entry->methodtype.descriptorIndex);
					break;
				case CONSTANT_InvokeDynamic:
					bw.writeu2(entry->invokedynamic.bootstrapMethodAttrIndex);
					bw.writeu2(entry->invokedynamic.nameAndTypeIndex);
					break;
			}
		}
	}

	void writeUnknown(UnknownAttr& attr) {
		bw.writecount(attr.data, attr.len);
	}

	void writeExceptions(ExceptionsAttr& attr) {
		u2 size = attr.es.size();

		bw.writeu2(size);
		for (u4 i = 0; i < size; i++) {
			u2 e = attr.es[i];
			bw.writeu2(e);
		}
	}

	void writeLnt(LntAttr& attr) {
		u2 count = attr.lnt.size();

		bw.writeu2(count);

		for (u4 i = 0; i < count; i++) {
			LntAttr::LnEntry& lne = attr.lnt[i];

			//bw.writeu2(lne.startpc);
			bw.writeu2(lne.startPcLabel->label.offset);
			bw.writeu2(lne.lineno);
		}
	}

	void writeLvt(LvtAttr& attr) {
		u2 count = attr.lvt.size();

		bw.writeu2(count);

		for (u4 i = 0; i < count; i++) {
			LvtAttr::LvEntry& lve = attr.lvt[i];

			//bw.writeu2(lve.startPc);
			bw.writeu2(lve.startPcLabel->label.offset);

			bw.writeu2(lve.len);
			bw.writeu2(lve.varNameIndex);
			bw.writeu2(lve.varDescIndex);
			bw.writeu2(lve.index);
		}
	}

	void writeSourceFile(SourceFileAttr& attr) {
		bw.writeu2(attr.sourceFileIndex);
	}

	void writeSmt(SmtAttr& attr) {
		auto parseTs =
				[&](std::vector<SmtAttr::VerType>& locs) {
					for (u1 i = 0; i < locs.size(); i++) {
						SmtAttr::VerType& vt = locs[i];

						u1 tag = vt.tag;
						bw.writeu1(tag);

						switch (tag) {
							case ITEM_Top:
							break;
							case ITEM_Integer:
							break;
							case ITEM_Float :
							break;
							case ITEM_Long :
							break;
							case ITEM_Double:
							break;
							case ITEM_Null :
							break;
							case ITEM_UninitializedThis :
							break;
							case ITEM_Object: {
								u2 cpIndex = vt.Object_variable_info.cpool_index;
								bw.writeu2( cpIndex);
								break;
							}
							case ITEM_Uninitialized: {
								u2 offset = vt.Uninitialized_variable_info.offset;

								offset = vt.Uninitialized_variable_info.label->label.offset;
								bw.writeu2(offset);
								break;
							}
						}
					}
				};

		bw.writeu2(attr.entries.size());

		int toff = -1;

		for (u2 i = 0; i < attr.entries.size(); i++) {
			SmtAttr::Entry& e = attr.entries[i];

			u2 offset = e.label->label.offset;

			toff += 1;

			u2 deltaOffset = offset - toff;

			toff = offset;

			u1 frameType = e.frameType;

			if (0 <= frameType && frameType <= 63) {
				if (deltaOffset <= 63) {
					frameType = deltaOffset;
				} else {
					frameType = 251;
					//EXCEPTION("not implemented 1");
				}
			} else if (64 <= frameType && frameType <= 127) {
				if (deltaOffset <= 63) {
					frameType = 64 + deltaOffset;
				} else {
					frameType = 247;
					//EXCEPTION("not implemented 2");
				}
			}

			bw.writeu1(frameType);

			if (0 <= frameType && frameType <= 63) {
			} else if (64 <= frameType && frameType <= 127) {
				parseTs(e.sameLocals_1_stack_item_frame.stack);
			} else if (frameType == 247) {
				u2 offsetDelta = deltaOffset;
				bw.writeu2(offsetDelta);
				parseTs(e.same_locals_1_stack_item_frame_extended.stack);
			} else if (248 <= frameType && frameType <= 250) {
				u2 offsetDelta = deltaOffset;
				bw.writeu2(offsetDelta);
			} else if (frameType == 251) {
				u2 offsetDelta = deltaOffset;
				bw.writeu2(offsetDelta);
			} else if (252 <= frameType && frameType <= 254) {
				u2 offsetDelta = deltaOffset;
				bw.writeu2(offsetDelta);
				parseTs(e.append_frame.locals);
			} else if (frameType == 255) {
				u2 offsetDelta = deltaOffset;
				bw.writeu2(offsetDelta);

				u2 numberOfLocals = e.full_frame.locals.size();
				bw.writeu2(numberOfLocals);
				parseTs(e.full_frame.locals);

				u2 numberOfStackItems = e.full_frame.stack.size();
				bw.writeu2(numberOfStackItems);
				parseTs(e.full_frame.stack);
			}
		}
	}

	void writeInstList(InstList& instList) {
		int offset = bw.getOffset();

		auto pos = [&]() {return bw.getOffset() - offset;};

		for (Inst* instp : instList) {
			Inst& inst = *instp;

			if (inst.kind == KIND_LABEL) {
				inst.label.offset = pos();

				//fprintf(stderr, "label pos @ write: %d\n", inst.label.offset);
				continue;
			}

			bw.writeu1(inst.opcode);

			switch (inst.kind) {
				case KIND_ZERO:
					if (inst.opcode == OPCODE_wide) {
						bw.writeu1(inst.wide.opcode);
						if (inst.wide.opcode == OPCODE_iinc) {
							bw.writeu2(inst.wide.iinc.index);
							bw.writeu2(inst.wide.iinc.value);
						} else {
							bw.writeu2(inst.wide.var.lvindex);
						}
					}
					break;
				case KIND_BIPUSH:
					bw.writeu1(inst.push.value);
					break;
				case KIND_SIPUSH:
					bw.writeu2(inst.push.value);
					break;
				case KIND_LDC:
					if (inst.opcode == OPCODE_ldc) {
						ASSERT(inst.ldc.valueIndex == (u1) inst.ldc.valueIndex,
								"invalid value for ldc: %d",
								inst.ldc.valueIndex);

						bw.writeu1(inst.ldc.valueIndex);
					} else {
						bw.writeu2(inst.ldc.valueIndex);
					}
					break;
				case KIND_VAR:
					bw.writeu1(inst.var.lvindex);
					break;
				case KIND_IINC:
					bw.writeu1(inst.iinc.index);
					bw.writeu1(inst.iinc.value);
					break;
				case KIND_JUMP: {
					//bw.writeu2(inst.jump.label);
					//fprintf(stderr, "target offset @ write: %d\n",	inst.jump.label2->label.offset);

					int jumppos = pos() - 1;

					bw.writeu2(inst.jump.label2->label.offset - jumppos);
					break;
				}
				case KIND_TABLESWITCH: {
					int tspos = pos() - 1;

					//	fprintf(stderr, "writer ts: offset: %d\n", pos());

					int pad = (4 - (pos() % 4)) % 4;
					for (int i = 0; i < pad; i++) {
						bw.writeu1(0);
					}

					bool check = pos() % 4 == 0;
					ASSERT(check, "Padding offset must be mod 4: %d", pos());

					bw.writeu4(inst.ts.def->label.offset - tspos);
					bw.writeu4(inst.ts.low);
					bw.writeu4(inst.ts.high);

					for (int i = 0; i < inst.ts.high - inst.ts.low + 1; i++) {
						Inst* t = inst.ts.targets[i];
						bw.writeu4(t->label.offset - tspos);
					}

					//	fprintf(stderr, "writer ts: offset: %d\n", pos());

					break;
				}
				case KIND_LOOKUPSWITCH: {
					int lspos = pos() - 1;

					int pad = (4 - (pos() % 4)) % 4;
					for (int i = 0; i < pad; i++) {
						bw.writeu1(0);
					}

					bool check = pos() % 4 == 0;
					ASSERT(check, "Padding offset must be mod 4: %d", pos());

					bw.writeu4(inst.ls.defbyte->label.offset - lspos);
					bw.writeu4(inst.ls.npairs);

					for (u4 i = 0; i < inst.ls.npairs; i++) {
						u4 k = inst.ls.keys[i];
						bw.writeu4(k);

						Inst* t = inst.ls.targets[i];
						bw.writeu4(t->label.offset - lspos);
					}
					break;
				}
				case KIND_FIELD:
					bw.writeu2(inst.field.fieldRefIndex);
					break;
				case KIND_INVOKE:
					bw.writeu2(inst.invoke.methodRefIndex);
					break;
				case KIND_INVOKEINTERFACE:
					bw.writeu2(inst.invokeinterface.interMethodRefIndex);
					bw.writeu1(inst.invokeinterface.count);
					bw.writeu1(0);
					break;
				case KIND_INVOKEDYNAMIC:
					break;
				case KIND_TYPE:
					bw.writeu2(inst.type.classIndex);
					break;
				case KIND_NEWARRAY:
					bw.writeu1(inst.newarray.atype);
					break;
				case KIND_MULTIARRAY:
					bw.writeu2(inst.multiarray.classIndex);
					bw.writeu1(inst.multiarray.dims);
					break;
				case KIND_PARSE4TODO:
					EXCEPTION("not implemetd");
					break;
				case KIND_RESERVED:
					EXCEPTION("not implemetd");
					break;
				default:
					EXCEPTION("default kind in instlist!!");
			}
		}
	}

	void writeCode(CodeAttr& attr) {
		bw.writeu2(attr.maxStack);
		bw.writeu2(attr.maxLocals);
		bw.writeu4(attr.codeLen);

		u4 offset = bw.getOffset();
		writeInstList(attr.instList);

		attr.codeLen = bw.getOffset() - offset;

		u2 esize = attr.exceptions.size();
		bw.writeu2(esize);
		for (u4 i = 0; i < esize; i++) {
			CodeExceptionEntry& e = attr.exceptions[i];
			bw.writeu2(e.startpc->label.offset);
			bw.writeu2(e.endpc->label.offset);
			bw.writeu2(e.handlerpc->label.offset);
			bw.writeu2(e.catchtype);
		}

		writeAttrs(attr.attrs);
	}

	void writeAttrs(Attrs& attrs) {
		bw.writeu2(attrs.size());

		for (u4 i = 0; i < attrs.size(); i++) {
			Attr& attr = *attrs.attrs[i];

			bw.writeu2(attr.nameIndex);
			bw.writeu4(attr.len);

			u4 offset = bw.getOffset();

			if (attr.kind == ATTR_UNKNOWN) {
				writeUnknown((UnknownAttr&) attr);
			} else {
				//BufferWriter bw(pos, attr.len);

				switch (attr.kind) {
					case ATTR_UNKNOWN:

					case ATTR_SOURCEFILE:
						writeSourceFile((SourceFileAttr&) attr);
						break;
					case ATTR_CODE:
						writeCode((CodeAttr&) attr);
						break;
					case ATTR_EXCEPTIONS:
						writeExceptions((ExceptionsAttr&) attr);
						break;
					case ATTR_LVT:
						writeLvt((LvtAttr&) attr);
						break;
					case ATTR_LVTT:
						writeLvt((LvtAttr&) attr);
						break;
					case ATTR_LNT:
						writeLnt((LntAttr&) attr);
						break;
					case ATTR_SMT:
						writeSmt((SmtAttr&) attr);
						break;
				}
			}

//		u4 len = bw.getOffset() - offset;
//		ASSERT(attr.len == len, "Expected %d, actual %d in %d", attr.len, len,
//				attr.kind);

			attr.len = bw.getOffset() - offset;
		}
	}

private:

	TWriter& bw;
};

}

#endif
