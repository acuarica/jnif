#include "jnif.hpp"

namespace jnif {

/**
 *
 */
struct BaseWriter {

	virtual ~BaseWriter() {
	}

	virtual void writeu1(u1 value) = 0;
	virtual void writeu2(u2 value) = 0;
	virtual void writeu4(u4 value) = 0;
	virtual void writecount(const void* source, int count) = 0;
	virtual int getOffset() const = 0;

};

/**
 *
 */
struct SizeWriter: BaseWriter {

	SizeWriter() :
			offset(0) {
	}

	virtual ~SizeWriter() {
	}

	void writeu1(u1 value) {
		offset += 1;
	}

	void writeu2(u2 value) {
		offset += 2;
	}

	void writeu4(u4 value) {
		offset += 4;
	}

	void writecount(const void* source, int count) {
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
struct BufferWriter: BaseWriter {

	BufferWriter(u1* buffer, int len) :
			buffer(buffer), len(len), offset(0) {
	}

	~BufferWriter() {
		end();
	}

	inline void writeu1(u1 value) {
		ASSERT(offset + 1 <= len, "Invalid write");

		buffer[offset] = value;

		offset += 1;
	}

	inline void writeu2(u2 value) {
		ASSERT(offset + 2 <= len, "Invalid write");

		buffer[offset + 0] = ((u1*) &value)[1];
		buffer[offset + 1] = ((u1*) &value)[0];

		offset += 2;
	}

	inline void writeu4(u4 value) {
		ASSERT(offset + 4 <= len, "Invalid write");

		buffer[offset + 0] = ((u1*) &value)[3];
		buffer[offset + 1] = ((u1*) &value)[2];
		buffer[offset + 2] = ((u1*) &value)[1];
		buffer[offset + 3] = ((u1*) &value)[0];

		offset += 4;
	}

	inline void writecount(const void* source, int count) {
		ASSERT(offset + count <= len, "Invalid write count");

		copy((u1*) source, (u1*) source + count, buffer + offset);

		offset += count;
	}

	inline int getOffset() const {
		return offset;
	}

//	inline u1* pos() {
//		return buffer + offset;
//	}
//
//	inline void skip(int count) {
//		ASSERT(offset + count <= len, "Invalid skip count");
//
//		offset += count;
//	}

private:
	inline void end() {
		ASSERT(offset == len,
				"%d != %d. End of buffer writer not reached while expecting end of buffer",
				offset, len);
	}

	u1* const buffer;
	const int len;
	int offset;
};

u4 getClassFileSize32(ClassFile & cf) {

	auto getAttrsSize = [&](const Attrs& attrs) {
		u4 size = sizeof(u2);

		for (u4 i = 0; i < attrs.size(); i++) {
			const Attr& attr = attrs[i];

			size += sizeof(attr.nameIndex);
			size += sizeof(attr.len);
			size += attr.len;
		}

		return size;
	};

	auto getConstPoolSize = [&](const ConstPool& cp) {
		u4 size = sizeof(u2);

		for (u4 i = 1; i < cp.entries.size(); i++) {
			const ConstPoolEntry* entry = &cp.entries[i];

			size += sizeof(entry->tag);

			switch (entry->tag) {
				case CONSTANT_Class:
				size += sizeof(entry->clazz.name_index);
				break;
				case CONSTANT_Fieldref:
				case CONSTANT_Methodref:
				case CONSTANT_InterfaceMethodref:
				size += sizeof(entry->memberref.class_index);
				size += sizeof(entry->memberref.name_and_type_index);
				break;
				case CONSTANT_String:
				size += sizeof(entry->s.string_index);
				break;
				case CONSTANT_Integer:
				size += sizeof(entry->i.value);
				break;
				case CONSTANT_Float:
				size += sizeof(entry->f.value);
				break;
				case CONSTANT_Long:
				size += sizeof(entry->l.high_bytes);
				size += sizeof(entry->l.low_bytes);
				i++;
				break;
				case CONSTANT_Double:
				size += sizeof(entry->d.high_bytes);
				size += sizeof(entry->d.low_bytes);
				i++;
				break;
				case CONSTANT_NameAndType:
				size += sizeof(entry->nameandtype.name_index);
				size += sizeof(entry->nameandtype.descriptor_index);
				break;
				case CONSTANT_Utf8:
				size += sizeof(u2);
				size += entry->utf8.str.length();
				break;
				case CONSTANT_MethodHandle:
				size += sizeof(entry->methodhandle.reference_kind);
				size += sizeof(entry->methodhandle.reference_index);
				break;
				case CONSTANT_MethodType:
				size += sizeof(entry->methodtype.descriptor_index);
				break;
				case CONSTANT_InvokeDynamic:
				size +=
				sizeof(entry->invokedynamic.bootstrap_method_attr_index);
				size += sizeof(entry->invokedynamic.name_and_type_index);
				break;
				default:
				EXCEPTION("Error while writing tag: %i", entry->tag);
			}
		}

		return size;
	};

	auto getMembersSize = [&](Members& members) {
		u4 size = sizeof(u2);

		for (u4 i = 0; i < members.size(); i++) {

			Member& mi = members[i];

			size += sizeof(mi.accessFlags);
			size += sizeof(mi.nameIndex);
			size += sizeof(mi.descIndex);

			size += getAttrsSize(mi);
		}

		return size;
	};

	u4 size = 0;

	size += sizeof(cf.magic);
	size += sizeof(cf.minor);
	size += sizeof(cf.major);

	size += getConstPoolSize(cf.cp);

	size += sizeof(cf.accessFlags);
	size += sizeof(cf.thisClassIndex);
	size += sizeof(cf.superClassIndex);

	size += sizeof(u2);
	size += cf.interfaces.size() * sizeof(u2);

	size += getMembersSize(cf.fields);
	size += getMembersSize(cf.methods);
	size += getAttrsSize(cf);

	return size;
}

static void writeAttrs(BaseWriter& bw, Attrs& attrs);

static void writeConstPool(BaseWriter& bw, const ConstPool& cp) {
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
				bw.writeu2(entry->invokedynamic.bootstrap_method_attr_index);
				bw.writeu2(entry->invokedynamic.name_and_type_index);
				break;
		}
	}
}

static void writeUnknown(BaseWriter& bw, UnknownAttr& attr) {
	bw.writecount(attr.data, attr.len);
}

static void writeExceptions(BaseWriter& bw, ExceptionsAttr& attr) {
	u2 size = attr.es.size();

	bw.writeu2(size);
	for (u4 i = 0; i < size; i++) {
		u2 e = attr.es[i];
		bw.writeu2(e);
	}
}

static void writeLnt(BaseWriter& bw, LntAttr& attr) {
	u2 count = attr.lnt.size();

	bw.writeu2(count);

	for (u4 i = 0; i < count; i++) {
		LntAttr::LnEntry& lne = attr.lnt[i];

		bw.writeu2(lne.startpc);
		bw.writeu2(lne.lineno);
	}
}

static void writeLvt(BaseWriter& bw, LvtAttr& attr) {
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

static void writeSourceFile(BaseWriter& bw, SourceFileAttr& attr) {
	bw.writeu2(attr.sourceFileIndex);
}

static void writeSmt(BaseWriter& bw, SmtAttr& attr) {

}

static void writeInstList(BaseWriter& bw, InstList& instList) {
	for (Inst& inst : instList) {
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
			case KIND_JUMP:
				bw.writeu2(inst.jump.label);
				break;
			case KIND_TABLESWITCH: {
				int pad = (4 - (bw.getOffset() % 4)) % 4;
				for (int i = 0; i < pad; i++) {
					bw.writeu1(0);
				}

				bool check = bw.getOffset() % 4 == 0;
				ASSERT(check, "Padding offset must be mod 4: %d",
						bw.getOffset());

				bw.writeu4(inst.ts.def);
				bw.writeu4(inst.ts.low);
				bw.writeu4(inst.ts.high);

				for (int i = 0; i < inst.ts.high - inst.ts.low + 1; i++) {
					u4 t = inst.ts.targets[i];
					bw.writeu4(t);
				}
				break;
			}
			case KIND_LOOKUPSWITCH: {
				int pad = (4 - (bw.getOffset() % 4)) % 4;
				for (int i = 0; i < pad; i++) {
					bw.writeu1(0);
				}

				bool check = bw.getOffset() % 4 == 0;
				ASSERT(check, "Padding offset must be mod 4: %d",
						bw.getOffset());

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

static void writeCode(BaseWriter& bw, CodeAttr& attr) {
	bw.writeu2(attr.maxStack);
	bw.writeu2(attr.maxLocals);
	bw.writeu4(attr.codeLen);

	u4 offset = bw.getOffset();
	writeInstList(bw, attr.instList);
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

	writeAttrs(bw, attr.attrs);
}

static void writeAttrs(BaseWriter& bw, Attrs& attrs) {
	bw.writeu2(attrs.size());

	for (u4 i = 0; i < attrs.size(); i++) {
		Attr& attr = *attrs.attrs[i];

		bw.writeu2(attr.nameIndex);
		bw.writeu4(attr.len);

		//u1* pos = bw.pos();

		u4 offset = bw.getOffset();

		if (attr.kind == ATTR_UNKNOWN) {
			writeUnknown(bw, (UnknownAttr&) attr);
		} else {
			//BufferWriter bw(pos, attr.len);

			switch (attr.kind) {
				case ATTR_UNKNOWN:

				case ATTR_SOURCEFILE:
					writeSourceFile(bw, (SourceFileAttr&) attr);
					break;
				case ATTR_CODE:
					writeCode(bw, (CodeAttr&) attr);
					break;
				case ATTR_EXCEPTIONS:
					writeExceptions(bw, (ExceptionsAttr&) attr);
					break;
				case ATTR_LVT:
					writeLvt(bw, (LvtAttr&) attr);
					break;
				case ATTR_LNT:
					writeLnt(bw, (LntAttr&) attr);
					break;
				case ATTR_SMT:
					writeSmt(bw, (SmtAttr&) attr);
					break;
			}
		}

		attr.len = bw.getOffset() - offset;

//		ASSERT(attr.len == len, "Expected %d, actual %d in %d", attr.len, len,
//				attr.kind);

		//bw.skip(attr.len);
	}
}

void writeClassFile(ClassFile& cf, BaseWriter& bw) {

	auto writeMembers = [](BaseWriter& bw, Members& members) {
		bw.writeu2(members.size());

		for (u4 i = 0; i < members.size(); i++) {

			Member& mi = members[i];

			bw.writeu2(mi.accessFlags);
			bw.writeu2(mi.nameIndex);
			bw.writeu2(mi.descIndex);

			writeAttrs(bw, mi);
		}
	};

	bw.writeu4(cf.magic);
	bw.writeu2(cf.minor);
	bw.writeu2(cf.major);

	writeConstPool(bw, cf.cp);

	bw.writeu2(cf.accessFlags);
	bw.writeu2(cf.thisClassIndex);
	bw.writeu2(cf.superClassIndex);

	u2 interCount = cf.interfaces.size();
	bw.writeu2(interCount);

	for (u4 i = 0; i < cf.interfaces.size(); i++) {
		u2 interIndex = cf.interfaces[i];
		bw.writeu2(interIndex);
	}

	writeMembers(bw, cf.fields);
	writeMembers(bw, cf.methods);
	writeAttrs(bw, cf);
}

u4 getClassFileSize(ClassFile & cf) {
	SizeWriter bw;
	writeClassFile(cf, bw);

	return bw.getOffset();
}

void writeClassFile(ClassFile& cf, u1* fileImage, const int fileImageLen) {
	BufferWriter bw(fileImage, fileImageLen);
	writeClassFile(cf, bw);
}

}
