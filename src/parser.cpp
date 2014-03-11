#include "jnif.hpp"
#include "jniferr.hpp"

namespace jnif {

/**
 * Implements a memory buffer reader in big-endian encoding.
 */
class BufferReader {
public:

	BufferReader(const u1* buffer, int len) :
			buffer(buffer), len(len), off(0) {
	}

	~BufferReader() {
		end();
	}

	int size() const {
		return len;
	}

	u1 readu1() {
		ASSERT(off + 1 <= len, "Invalid read");

		u1 result = buffer[off];

		off += 1;

		return result;
	}

	u2 readu2() {
		ASSERT(off + 2 <= len, "Invalid read 2");

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];

		u2 result = r0 << 8 | r1;

		off += 2;

		return result;
	}

	u4 readu4() {
		ASSERT(off + 4 <= len, "Invalid read 4");

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];
		u1 r2 = buffer[off + 2];
		u1 r3 = buffer[off + 3];

		u4 result = r0 << 24 | r1 << 16 | r2 << 8 | r3;

		off += 4;

		return result;
	}

	void skip(int count) {
		ASSERT(off + count <= len, "Invalid read count: %d (offset: %d)", count,
				off);

		off += count;
	}

	int offset() const {
		return off;
	}

	const u1* pos() const {
		return buffer + off;
	}

	bool eor() const {
		return off == len;
	}

private:

	void end() {
		ASSERT(off == len,
				"End of buffer not reached while expecting end of buffer");
	}

	const u1 * const buffer;
	const int len;
	int off;
};

OpKind OPKIND[256] = { KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO, KIND_ZERO,
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
		KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, KIND_RESERVED, };

static void parseAttrs(BufferReader& br, ConstPool& cp, Attrs& as);

static void parseConstPool(BufferReader& br, ConstPool& cp) {
	u2 count = br.readu2();

	for (int i = 1; i < count; i++) {
		ConstPoolEntry e;
		ConstPoolEntry* entry = &e;

		entry->tag = (ConstTag) br.readu1();

		switch (entry->tag) {
			case CONSTANT_Class:
				entry->clazz.name_index = br.readu2();
				break;
			case CONSTANT_Fieldref:
			case CONSTANT_Methodref:
			case CONSTANT_InterfaceMethodref:
				entry->memberref.class_index = br.readu2();
				entry->memberref.name_and_type_index = br.readu2();
				break;
			case CONSTANT_String:
				entry->s.string_index = br.readu2();
				break;
			case CONSTANT_Integer:
				entry->i.value = br.readu4();
				break;
			case CONSTANT_Float:
				entry->f.value = br.readu4();
				break;
			case CONSTANT_Long:
				entry->l.high_bytes = br.readu4();
				entry->l.low_bytes = br.readu4();
				i++;
				break;
			case CONSTANT_Double:
				entry->d.high_bytes = br.readu4();
				entry->d.low_bytes = br.readu4();
				i++;
				break;
			case CONSTANT_NameAndType:
				entry->nameandtype.name_index = br.readu2();
				entry->nameandtype.descriptor_index = br.readu2();
				break;
			case CONSTANT_Utf8: {
				u2 len = br.readu2();
				std::string str((const char*) br.pos(), len);
				entry->utf8.str = str;
				br.skip(len);
				break;
			}
			case CONSTANT_MethodHandle:
				entry->methodhandle.reference_kind = br.readu1();
				entry->methodhandle.reference_index = br.readu2();
				break;
			case CONSTANT_MethodType:
				entry->methodtype.descriptor_index = br.readu2();
				break;
			case CONSTANT_InvokeDynamic:
				entry->invokedynamic.bootstrap_method_attr_index = br.readu2();
				entry->invokedynamic.name_and_type_index = br.readu2();
				break;
			default:
				EXCEPTION("Error while reading tag: %i", entry->tag);
		}

		if (entry->tag == CONSTANT_Long || entry->tag == CONSTANT_Double) {
			cp.addDouble(e);
		} else {
			cp.addSingle(e);
		}
	}
}

static Attr* parseSourceFile(BufferReader& br, Attrs& as, u2 nameIndex) {
	u2 sourceFileIndex = br.readu2();

	Attr* attr = new SourceFileAttr(nameIndex, 2, sourceFileIndex);
	as.add(attr);

	return attr;
}

static Attr* parseExceptions(BufferReader& br, Attrs& as, u2 nameIndex) {
	u2 len = br.readu2();

	std::vector<u2> es;
	for (int i = 0; i < len; i++) {
		u2 exceptionIndex = br.readu2();

		es.push_back(exceptionIndex);
	}

	Attr* attr = new ExceptionsAttr(nameIndex, len * 2 + 2, es);
	as.add(attr);

	return attr;
}

static Inst** parseLabels(BufferReader& br) {

	Inst** labels = new Inst*[br.size()];
	for (int i = 0; i < br.size(); i++) {
		labels[i] = nullptr;
	}

	while (!br.eor()) {
		int offset = br.offset();

		Opcode opcode = (Opcode) br.readu1();
		OpKind kind = OPKIND[opcode];

		switch (kind) {
			case KIND_ZERO:
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
			case KIND_LDC:
				if (opcode == OPCODE_ldc) {
					br.readu1();
				} else {
					br.readu2();
				}
				break;
			case KIND_JUMP: {
				u2 targetOffset = br.readu2();

				u2 labelpos = offset + targetOffset;
				ASSERT(labelpos < br.size(), "invalid target for jump");

				Inst*& lab = labels[labelpos];
				if (lab == nullptr) {
					lab = new Inst(KIND_LABEL);
				}

				break;
			}
			case KIND_TABLESWITCH: {
				for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
					u1 pad = br.readu1();
					ASSERT(pad == 0, "Padding must be zero");
				}

				{
					bool check = br.offset() % 4 == 0;
					ASSERT(check, "%d", br.offset());
				}

				br.readu4(); // def
				int low = br.readu4();
				int high = br.readu4();

				ASSERT(low <= high,
						"low (%d) must be less or equal than high (%d)", low,
						high);

				for (int i = 0; i < high - low + 1; i++) {
					br.readu4();
				}
				break;
			}
			case KIND_LOOKUPSWITCH: {
				for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
					u1 pad = br.readu1();
					ASSERT(pad == 0, "Padding must be zero");
				}

				u4 defbyte = br.readu4();
				u4 npairs = br.readu4();

				for (u4 i = 0; i < npairs; i++) {
					br.readu4();
					br.readu4();
				}
				break;
			}
		}
	}

	return labels;
}

static void parseInstList(BufferReader& br, InstList& instList) {

	const u1* code = br.pos();
	int codeLen = br.size();
	Inst** labels;
	{
		BufferReader br(code, codeLen);
		labels = parseLabels(br);
	}

	while (!br.eor()) {
		int offset = br.offset();

		if (labels[offset] != nullptr) {
			instList.push_back(labels[offset]);
		}

		Inst* instp = new Inst();
		Inst& inst = *instp;

		inst.opcode = (Opcode) br.readu1();
		inst.kind = OPKIND[inst.opcode];

		switch (inst.kind) {
			case KIND_ZERO:
				if (inst.opcode == OPCODE_wide) {
					ASSERT(false, "wide not supported yet");
				}
				break;
			case KIND_BIPUSH:
				inst.push.value = br.readu1();
				break;
			case KIND_SIPUSH:
				inst.push.value = br.readu2();
				break;
			case KIND_LDC:
				if (inst.opcode == OPCODE_ldc) {
					inst.ldc.valueIndex = br.readu1();
				} else {
					inst.ldc.valueIndex = br.readu2();
				}
				break;
			case KIND_VAR:
				inst.var.lvindex = br.readu1();
				break;
			case KIND_IINC:
				inst.iinc.index = br.readu1();
				inst.iinc.value = br.readu1();
				break;
			case KIND_JUMP: {
				//
				u2 targetOffset = br.readu2();
				inst.jump.label = targetOffset;
				inst.jump.label2 = labels[offset + targetOffset];
				break;
			}
			case KIND_TABLESWITCH:
				for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
					u1 pad = br.readu1();
					ASSERT(pad == 0, "Padding must be zero");
				}

				{
					bool check = br.offset() % 4 == 0;
					ASSERT(check, "%d", br.offset());
				}

				inst.ts.def = br.readu4();
				inst.ts.low = br.readu4();
				inst.ts.high = br.readu4();

				ASSERT(inst.ts.low <= inst.ts.high,
						"low (%d) must be less or equal than high (%d)",
						inst.ts.low, inst.ts.high);

				for (int i = 0; i < inst.ts.high - inst.ts.low + 1; i++) {
					u4 targetOffset = br.readu4();
					inst.ts.targets.push_back(targetOffset);
				}

				//		fprintf(stderr, "parser ts: offset: %d\n", br.offset());

				break;
			case KIND_LOOKUPSWITCH:
				for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
					u1 pad = br.readu1();
					ASSERT(pad == 0, "Padding must be zero");
				}

				inst.ls.defbyte = br.readu4();
				inst.ls.npairs = br.readu4();

				for (u4 i = 0; i < inst.ls.npairs; i++) {
					u4 key = br.readu4();
					u4 offsetTarget = br.readu4();

					inst.ls.keys.push_back(key);
					inst.ls.targets.push_back(offsetTarget);
				}
				break;
			case KIND_FIELD:
				inst.field.fieldRefIndex = br.readu2();
				break;
			case KIND_INVOKE:
				inst.invoke.methodRefIndex = br.readu2();
				break;
			case KIND_INVOKEINTERFACE:
				inst.invokeinterface.interMethodRefIndex = br.readu2();
				inst.invokeinterface.count = br.readu1();

				ASSERT(inst.invokeinterface.count != 0, "Count is zero!");
				{
					u1 zero = br.readu1();
					ASSERT(zero == 0, "Fourth operand must be zero");
				}
				break;
			case KIND_INVOKEDYNAMIC:
				EXCEPTION("FrParseInvokeDynamicInstr not implemented");
				break;
			case KIND_TYPE:
				inst.type.classIndex = br.readu2();
				break;
			case KIND_NEWARRAY:
				inst.newarray.atype = br.readu1();
				break;
			case KIND_MULTIARRAY:
				inst.multiarray.classIndex = br.readu2();
				inst.multiarray.dims = br.readu1();
				break;
			case KIND_PARSE4TODO:
				EXCEPTION("FrParse4__TODO__Instr not implemented");
				break;
			case KIND_RESERVED:
				EXCEPTION("FrParseReservedInstr not implemented");
				break;

		}

		instList.push_back(instp);
	}
}

static Attr* parseCode(BufferReader& br, Attrs& as, ConstPool& cp,
		u2 nameIndex) {

	CodeAttr* ca = new CodeAttr(nameIndex);

	ca->maxStack = br.readu2();
	ca->maxLocals = br.readu2();
	ca->codeLen = br.readu4();

	const u1* codeBuf = br.pos();
	br.skip(ca->codeLen);

	{
		BufferReader br(codeBuf, ca->codeLen);
		parseInstList(br, ca->instList);
	}

	u2 exceptionTableCount = br.readu2();
	for (int i = 0; i < exceptionTableCount; i++) {
		CodeExceptionEntry e;
		e.startpc = br.readu2();
		e.endpc = br.readu2();
		e.handlerpc = br.readu2();
		e.catchtype = br.readu2();

		ca->exceptions.push_back(e);
	}

	parseAttrs(br, cp, ca->attrs);

	as.add(ca);

	return ca;
}

static Attr* parseLnt(BufferReader& br, Attrs& as, u2 nameIndex) {
	u2 lntlen = br.readu2();

	LntAttr* lnt = new LntAttr(nameIndex);

	for (int i = 0; i < lntlen; i++) {
		LntAttr::LnEntry e;
		e.startpc = br.readu2();
		e.lineno = br.readu2();

		lnt->lnt.push_back(e);
	}

	as.add(lnt);

	return lnt;
}

static Attr* parseLvt(BufferReader& br, Attrs& as, u2 nameIndex) {
	u2 count = br.readu2();

	LvtAttr* lvt = new LvtAttr(nameIndex);

	for (u2 i = 0; i < count; i++) {
		LvtAttr::LvEntry e;

		e.startPc = br.readu2();
		e.len = br.readu2();
		e.varNameIndex = br.readu2();
		e.varDescIndex = br.readu2();
		e.index = br.readu2();

		lvt->lvt.push_back(e);
	}

	as.add(lvt);

	return lvt;
}
//
//static void parseSmt(BufferReader& br, Attrs& as, ConstPool& cp, u2 nameIndex) {
//
//	auto parseTs = [&](int count) {
//		for (u1 i = 0; i < count; i++) {
//			u1 tag = br.readu1();
//			switch (tag) {
//				case ITEM_Top:
//				break;
//				case ITEM_Integer:
//				break;
//				case ITEM_Float :
//
//				break;
//				case ITEM_Long :
//
//				break;
//				case ITEM_Double:
//				break;
//				case ITEM_Null :
//				break;
//				case ITEM_UninitializedThis :break;
//				case ITEM_Object: {
//					u2 cpIndex = br.readu2();
//					break;
//				}
//				case ITEM_Uninitialized: {
//					u2 offset= br.readu2();
//					break;
//				}
//			}
//		}
//	};
//
//	u2 numberOfEntries = br.readu2();
//
//	for (u2 i = 0; i < numberOfEntries; i++) {
//		u1 frameType = br.readu1();
//
//		if (0 <= frameType && frameType <= 63) {
//			//	v.visitFrameSame(frameType);
//		} else if (64 <= frameType && frameType <= 127) {
//			parseTs(1);
//			//v.visitFrameSameLocals1StackItem(frameType);
//		} else if (frameType == 247) {
//			u2 offsetDelta = br.readu2();
//			parseTs(1);
//		} else if (248 <= frameType && frameType <= 250) {
//			u2 offsetDelta = br.readu2();
//
//		} else if (frameType == 251) {
//			u2 offsetDelta = br.readu2();
//
//		} else if (252 <= frameType && frameType <= 254) {
//			u2 offsetDelta = br.readu2();
//			parseTs(frameType - 251);
//
//		} else if (frameType == 255) {
//			u2 offsetDelta = br.readu2();
//			u2 numberOfLocals = br.readu2();
//			parseTs(numberOfLocals);
//			u2 numberOfStackItems = br.readu2();
//			parseTs(numberOfStackItems);
//		}
//	}
//
//	//as.add(new SourceFileAttr(nameIndex, 2, sourceFileIndex));
//}

static void parseAttrs(BufferReader& br, ConstPool& cp, Attrs& as) {
	u2 attrCount = br.readu2();

	for (int i = 0; i < attrCount; i++) {
		u2 nameIndex = br.readu2();
		u4 len = br.readu4();
		const u1* data = br.pos();

		string attrName = cp.getUtf8(nameIndex);

		Attr* a;
		if (attrName == "SourceFile") {
			BufferReader br(data, len);
			a = parseSourceFile(br, as, nameIndex);
		} else if (attrName == "Exceptions") {
			BufferReader br(data, len);
			a = parseExceptions(br, as, nameIndex);
		} else if (attrName == "Code") {
			BufferReader br(data, len);
			a = parseCode(br, as, cp, nameIndex);
		} else if (attrName == "LineNumberTable") {
			BufferReader br(data, len);
			a = parseLnt(br, as, nameIndex);
		} else if (attrName == "LocalVariableTable") {
			BufferReader br(data, len);
			a = parseLvt(br, as, nameIndex);
//		} else if (attrName == "StackMapTable") {
//			BufferReader br(data, len);
//			parseSmt(br, as, cp, nameIndex);
		} else {
			a = new UnknownAttr(nameIndex, len, data);
			as.add(a);
		}

		a->len = len;

		br.skip(len);
	}
}

void parseClassFile(const u1* fileImage, const int fileImageLen,
		ClassFile& cf) {
	BufferReader br(fileImage, fileImageLen);

	u4 magic = br.readu4();

	CHECK(magic == CLASSFILE_MAGIC,
			"Invalid magic number. Expected 0xcafebabe, found: %x", magic);

	u2 minor = br.readu2();
	u2 major = br.readu2();

	cf.setVersion(Version(major, minor));

	parseConstPool(br, cf);

	cf.accessFlags = br.readu2();
	cf.thisClassIndex = br.readu2();
	cf.superClassIndex = br.readu2();

	u2 interCount = br.readu2();
	for (int i = 0; i < interCount; i++) {
		u2 interIndex = br.readu2();
		cf.interfaces.push_back(interIndex);
	}

	auto mm = [&](Members& ms) {
		u2 memberCount = br.readu2();
		for (int i = 0; i < memberCount; i++) {
			AccessFlags accessFlags = (AccessFlags)br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			Member& m = ms.add(accessFlags, nameIndex, descIndex);

			parseAttrs(br, cf, m);
		}};

	mm(cf.fields);
	mm(cf.methods);

	parseAttrs(br, cf, cf);
}

}

