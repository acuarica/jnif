#include "jnif.hpp"
#include "jniferr.hpp"

namespace jnif {

/**
 *
 */
class SizeWriter {
public:

	SizeWriter() :
			offset(0) {
	}

	void writeu1(u1) {
		offset += 1;
	}

	void writeu2(u2) {
		offset += 2;
	}

	void writeu4(u4) {
		offset += 4;
	}

	void writecount(const void*, int count) {
		offset += count;
	}

	int getOffset() const {
		return offset;
	}

private:
	int offset;
};

/**
 * Implements a memory buffer writer in big-endian encoding.
 */
class BufferWriter {
public:

	BufferWriter(u1* buffer, int len) :
			buffer(buffer), len(len), offset(0) {
	}

	~BufferWriter() {
		end();
	}

	void writeu1(u1 value) {
		ASSERT(offset + 1 <= len, "Invalid write");

		buffer[offset] = value;

		offset += 1;
	}

	void writeu2(u2 value) {
		ASSERT(offset + 2 <= len, "Invalid write");

		buffer[offset + 0] = ((u1*) &value)[1];
		buffer[offset + 1] = ((u1*) &value)[0];

		offset += 2;
	}

	void writeu4(u4 value) {
		ASSERT(offset + 4 <= len, "Invalid write");

		buffer[offset + 0] = ((u1*) &value)[3];
		buffer[offset + 1] = ((u1*) &value)[2];
		buffer[offset + 2] = ((u1*) &value)[1];
		buffer[offset + 3] = ((u1*) &value)[0];

		offset += 4;
	}

	void writecount(const void* source, int count) {
		ASSERT(offset + count <= len, "Invalid write count");

		copy((u1*) source, (u1*) source + count, buffer + offset);

		offset += count;
	}

	int getOffset() const {
		return offset;
	}

private:

	void end() {
		ASSERT(offset == len,
				"%d != %d. End of buffer writer not reached while expecting "

				"end of buffer", offset, len);
	}

private:

	u1* const buffer;
	const int len;
	int offset;
};

template<typename TWriter>
class ClassWriter {
public:

	ClassWriter(TWriter& bw) :
			bw(bw) {
	}

	void writeClassFile(ClassFile& cf) {
		auto writeMembers = [this](Members& members) {
			bw.writeu2(members.size());

			for (u4 i = 0; i < members.size(); i++) {

				Member& mi = members[i];

				bw.writeu2(mi.accessFlags);
				bw.writeu2(mi.nameIndex);
				bw.writeu2(mi.descIndex);

				writeAttrs(mi);
			}
		};

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

		writeMembers(cf.fields);
		writeMembers(cf.methods);
		writeAttrs(cf);
	}

private:

	void writeConstPool(const ConstPool& cp) {
		u2 count = cp.entries.size();
		bw.writeu2(count);

		for (u4 i = 1; i < cp.entries.size(); i++) {
			const ConstPoolEntry* entry = &cp.entries[i];

			bw.writeu1(entry->tag);

			switch (entry->tag) {
				case CONSTANT_Class:
					bw.writeu2(entry->clazz.name_index);
					break;
				case CONSTANT_Fieldref:
				case CONSTANT_Methodref:
				case CONSTANT_InterfaceMethodref:
					bw.writeu2(entry->memberref.class_index);
					bw.writeu2(entry->memberref.name_and_type_index);
					break;
				case CONSTANT_String:
					bw.writeu2(entry->s.string_index);
					break;
				case CONSTANT_Integer:
					bw.writeu4(entry->i.value);
					break;
				case CONSTANT_Float:
					bw.writeu4(entry->f.value);
					break;
				case CONSTANT_Long:
					bw.writeu4(entry->l.high_bytes);
					bw.writeu4(entry->l.low_bytes);
					i++;
					break;
				case CONSTANT_Double:
					bw.writeu4(entry->d.high_bytes);
					bw.writeu4(entry->d.low_bytes);
					i++;
					break;
				case CONSTANT_NameAndType:
					bw.writeu2(entry->nameandtype.name_index);
					bw.writeu2(entry->nameandtype.descriptor_index);
					break;
				case CONSTANT_Utf8: {
					u2 len = entry->utf8.str.length();
					const char* str = entry->utf8.str.c_str();
					bw.writeu2(len);
					bw.writecount(str, len);
					break;
				}
				case CONSTANT_MethodHandle:
					bw.writeu1(entry->methodhandle.reference_kind);
					bw.writeu2(entry->methodhandle.reference_index);
					break;
				case CONSTANT_MethodType:
					bw.writeu2(entry->methodtype.descriptor_index);
					break;
				case CONSTANT_InvokeDynamic:
					bw.writeu2(
							entry->invokedynamic.bootstrap_method_attr_index);
					bw.writeu2(entry->invokedynamic.name_and_type_index);
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

			bw.writeu2(lne.startpc);
			bw.writeu2(lne.lineno);
		}
	}

	void writeLvt(LvtAttr& attr) {
		u2 count = attr.lvt.size();

		bw.writeu2(count);

		for (u4 i = 0; i < count; i++) {
			LvtAttr::LvEntry& lve = attr.lvt[i];

			bw.writeu2(lve.startPc);
			bw.writeu2(lve.len);
			bw.writeu2(lve.varNameIndex);
			bw.writeu2(lve.varDescIndex);
			bw.writeu2(lve.index);
		}
	}

	void writeSourceFile(SourceFileAttr& attr) {
		bw.writeu2(attr.sourceFileIndex);
	}

	void writeSmt(SmtAttr& /*attr*/) {
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
					break;
				case KIND_BIPUSH:
					bw.writeu1(inst.push.value);
					break;
				case KIND_SIPUSH:
					bw.writeu2(inst.push.value);
					break;
				case KIND_LDC:
					if (inst.opcode == OPCODE_ldc) {
						ASSERT(inst.ldc.valueIndex == (u1 )inst.ldc.valueIndex,
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
					//	fprintf(stderr, "writer ts: offset: %d\n", pos());

					int pad = (4 - (pos() % 4)) % 4;
					for (int i = 0; i < pad; i++) {
						bw.writeu1(0);
					}

					bool check = pos() % 4 == 0;
					ASSERT(check, "Padding offset must be mod 4: %d", pos());

					bw.writeu4(inst.ts.def);
					bw.writeu4(inst.ts.low);
					bw.writeu4(inst.ts.high);

					for (int i = 0; i < inst.ts.high - inst.ts.low + 1; i++) {
						u4 t = inst.ts.targets[i];
						bw.writeu4(t);
					}

					//	fprintf(stderr, "writer ts: offset: %d\n", pos());

					break;
				}
				case KIND_LOOKUPSWITCH: {
					int pad = (4 - (pos() % 4)) % 4;
					for (int i = 0; i < pad; i++) {
						bw.writeu1(0);
					}

					bool check = pos() % 4 == 0;
					ASSERT(check, "Padding offset must be mod 4: %d", pos());

					bw.writeu4(inst.ls.defbyte);
					bw.writeu4(inst.ls.npairs);

					for (u4 i = 0; i < inst.ls.npairs; i++) {
						u4 k = inst.ls.keys[i];
						bw.writeu4(k);

						u4 t = inst.ls.targets[i];
						bw.writeu4(t);
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
			}
		}
	}

	void writeCode(CodeAttr& attr) {
		bw.writeu2(attr.maxStack);
		bw.writeu2(attr.maxLocals);
		bw.writeu4(attr.codeLen);

		u4 offset = bw.getOffset();
		writeInstList(attr.instList);

//	u4 len = bw.getOffset() - offset;
//	ASSERT(attr.codeLen == len, "writeCode:: Expected %d, actual %d in %d",
//			attr.codeLen, len, attr.kind);

		attr.codeLen = bw.getOffset() - offset;

		u2 esize = attr.exceptions.size();
		bw.writeu2(esize);
		for (u4 i = 0; i < esize; i++) {
			CodeExceptionEntry& e = attr.exceptions[i];
			bw.writeu2(e.startpc);
			bw.writeu2(e.endpc);
			bw.writeu2(e.handlerpc);
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

			//u1* pos = bw.pos();

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

u4 getClassFileSize(ClassFile & cf) {
	SizeWriter bw;
	ClassWriter<SizeWriter>(bw).writeClassFile(cf);

	return bw.getOffset();
}

void writeClassFile(ClassFile& cf, u1* fileImage, const int fileImageLen) {
	BufferWriter bw(fileImage, fileImageLen);
	ClassWriter<BufferWriter>(bw).writeClassFile(cf);
}

}
