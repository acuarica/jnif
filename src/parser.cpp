#include "jnif.hpp"

using namespace std;

namespace jnif {

/**
 * Implements a memory buffer reader in big-endian encoding.
 */
class BufferReader: private ErrorManager {
public:

	/**
	 * Constructs a BufferReader from a memory buffer and its size.
	 * The @ref buffer must be an accessible and readable memory location
	 * at least of size @ref len.
	 *
	 * @param buffer The memory buffer to read from.
	 * @param size The size of the buffer in bytes.
	 *
	 */
	BufferReader(const u1* buffer, u4 size) :
			buffer(buffer), _size(size), off(0) {
	}

	/**
	 * When this buffer reader finishes, it will check whether the end has
	 * been reached, i.e., all bytes from buffer were read or skipped.
	 * In other words, when the buffer reader br is destroyed, the condition
	 *
	 * @f[ br.offset() = br.size() @f]
	 *
	 * must hold.
	 */
	~BufferReader() {
		check(off == _size, "Expected end of buffer");
	}

	int size() const {
		return _size;
	}

	u1 readu1() {
		check(off + 1 <= _size, "Invalid read");

		u1 result = buffer[off];

		off += 1;

		return result;
	}

	u2 readu2() {
		check(off + 2 <= _size, "Invalid read 2");

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];

		u2 result = r0 << 8 | r1;

		off += 2;

		return result;
	}

	u4 readu4() {
		check(off + 4 <= _size, "Invalid read 4");

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];
		u1 r2 = buffer[off + 2];
		u1 r3 = buffer[off + 3];

		u4 result = r0 << 24 | r1 << 16 | r2 << 8 | r3;

		off += 4;

		return result;
	}

	void skip(int count) {
		const char* const m = "Invalid read: %d (offset: %d)";
		check(off + count <= _size, m, count, off);

		off += count;
	}

	int offset() const {
		return off;
	}

	const u1* pos() const {
		return buffer + off;
	}

	bool eor() const {
		return off == _size;
	}

private:

	const u1 * const buffer;
	const int _size;
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

class ClassParser: private ErrorManager {
public:

	static void parseClassFile(const u1* fileImage, const int fileImageLen,
			ClassFile& cf) {
		BufferReader br(fileImage, fileImageLen);

		u4 magic = br.readu4();

		check(magic == CLASSFILE_MAGIC,
				"Invalid magic number. Expected 0xcafebabe, found: ", magic);

		cf.minorVersion = br.readu2();
		cf.majorVersion = br.readu2();

		parseConstPool(br, cf);

		cf.accessFlags = br.readu2();
		cf.thisClassIndex = br.readu2();
		cf.superClassIndex = br.readu2();

		u2 interCount = br.readu2();
		for (int i = 0; i < interCount; i++) {
			u2 interIndex = br.readu2();
			cf.interfaces.push_back(interIndex);
		}

		u2 fieldCount = br.readu2();
		for (int i = 0; i < fieldCount; i++) {
			AccessFlags accessFlags = (AccessFlags) br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			Field* f = cf.addField(nameIndex, descIndex, accessFlags);

			parseAttrs(br, cf, *f);
		}

		u2 methodCount = br.readu2();
		for (int i = 0; i < methodCount; i++) {
			AccessFlags accessFlags = (AccessFlags) br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			Method* m = cf.addMethod(nameIndex, descIndex, accessFlags);

			parseAttrs(br, cf, *m);
		}

		parseAttrs(br, cf, cf);
	}

private:

	static void parseConstPool(BufferReader& br, ConstPool& cp) {
		u2 count = br.readu2();

		for (int i = 1; i < count; i++) {
			u1 tag = br.readu1();

			switch (tag) {
				case ConstPool::CLASS: {
					u2 classNameIndex = br.readu2();
					cp.addClass(classNameIndex);
					break;
				}
				case ConstPool::FIELDREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addFieldRef(classIndex, nameAndTypeIndex);
					break;
				}
				case ConstPool::METHODREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addMethodRef(classIndex, nameAndTypeIndex);
					break;
				}
				case ConstPool::INTERFACEMETHODREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addInterMethodRef(classIndex, nameAndTypeIndex);
					break;
				}
				case ConstPool::STRING: {
					u2 utf8Index = br.readu2();
					cp.addString(utf8Index);
					break;
				}
				case ConstPool::INTEGER: {
					u4 value = br.readu4();
					cp.addInteger(value);
					break;
				}
				case ConstPool::FLOAT: {
					u4 value = br.readu4();
					cp.addFloat(value);
					break;
				}
				case ConstPool::LONG: {
					u4 high = br.readu4();
					u4 low = br.readu4();
					long value = ((long) high << 32) + low;
					cp.addLong(value);
					i++;
					break;
				}
				case ConstPool::DOUBLE: {
					u4 high = br.readu4();
					u4 low = br.readu4();
					long lvalue = ((long) high << 32) + low;
					double dvalue = *(double*) &lvalue;
					cp.addDouble(dvalue);
					i++;
					break;
				}
				case ConstPool::NAMEANDTYPE: {
					u2 nameIndex = br.readu2();
					u2 descIndex = br.readu2();
					cp.addNameAndType(nameIndex, descIndex);
					break;
				}
				case ConstPool::UTF8: {
					u2 len = br.readu2();
					cp.addUtf8((const char*) br.pos(), len);
					br.skip(len);
					break;
				}
				case ConstPool::METHODHANDLE: {
					u1 refKind = br.readu1();
					u2 refIndex = br.readu2();
					cp.addMethodHandle(refKind, refIndex);
					break;
				}
				case ConstPool::METHODTYPE: {
					u2 descIndex = br.readu2();
					cp.addMethodType(descIndex);
					break;
				}
				case ConstPool::INVOKEDYNAMIC: {
					u2 bootstrapMethodAttrIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addInvokeDynamic(bootstrapMethodAttrIndex,
							nameAndTypeIndex);
					break;
				}
				default:
					raise("Error while reading tag: ", tag);
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

		vector<u2> es;
		for (int i = 0; i < len; i++) {
			u2 exceptionIndex = br.readu2();

			es.push_back(exceptionIndex);
		}

		Attr* attr = new ExceptionsAttr(nameIndex, len * 2 + 2, es);
		as.add(attr);

		return attr;
	}

	static Inst* createLabel(Inst** labels, int labelpos) {
		Inst*& lab = labels[labelpos];
		if (lab == nullptr) {
			lab = new Inst(KIND_LABEL);
		}

		return lab;
	}

	static void parseInstTargets(BufferReader& br, Inst** labels) {
		while (!br.eor()) {
			int offset = br.offset();

			Opcode opcode = (Opcode) br.readu1();
			OpKind kind = OPKIND[opcode];

			switch (kind) {
				case KIND_ZERO:
					if (opcode == OPCODE_wide) {
						u1 opcodew = (Opcode) br.readu1();
						if (opcodew == OPCODE_iinc) {
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
				case KIND_LDC:
					if (opcode == OPCODE_ldc) {
						br.readu1();
					} else {
						br.readu2();
					}
					break;
				case KIND_JUMP: {
					short targetOffset = br.readu2();

					short labelpos = offset + targetOffset;
					assert(labelpos >= 0,
							"invalid target for jump: must be >= 0");
					assert(labelpos < br.size(), "invalid target for jump");

					createLabel(labels, labelpos);
//				Inst*& lab = labels[labelpos];
//				if (lab == nullptr) {
//					lab = new Inst(KIND_LABEL);
//				}

					break;
				}
				case KIND_TABLESWITCH: {
					for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
						u1 pad = br.readu1();
						assert(pad == 0, "Padding must be zero");
					}

					{
						bool check2 = br.offset() % 4 == 0;
						assert(check2, "%d", br.offset());
					}

					int defOffset = br.readu4();
					createLabel(labels, offset + defOffset);

					int low = br.readu4();
					int high = br.readu4();

					assert(low <= high,
							"low (%d) must be less or equal than high (%d)",
							low, high);

					for (int i = 0; i < high - low + 1; i++) {
						int targetOffset = br.readu4();
						createLabel(labels, offset + targetOffset);
					}
					break;
				}
				case KIND_LOOKUPSWITCH: {
					for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
						u1 pad = br.readu1();
						assert(pad == 0, "Padding must be zero");
					}

					int defOffset = br.readu4();
					createLabel(labels, offset + defOffset);

					u4 npairs = br.readu4();

					for (u4 i = 0; i < npairs; i++) {
						br.readu4(); // Key

						int targetOffset = br.readu4();
						createLabel(labels, offset + targetOffset);
					}
					break;
				}
//			case KIND_RESERVED:
					//			break;
				default:
					raise(
							"default kind in parseInstTargets: opcode: %d, kind: %d",
							opcode, kind);
			}
		}
	}

	static void parseInstList(BufferReader& br, InstList& instList,
			Inst** labels) {
		while (!br.eor()) {
			int offset = br.offset();

			if (labels[offset] != nullptr) {
				labels[offset]->_offset = offset;
				instList.push_back(labels[offset]);
			}

			Opcode opcode = (Opcode) br.readu1();

			Inst* instp = new Inst(opcode, OPKIND[opcode]);
			instp->_offset = offset;
			Inst& inst = *instp;

			//inst.opcode = (Opcode) br.readu1();
			//inst.kind = OPKIND[inst.opcode];

			switch (inst.kind) {
				case KIND_ZERO:
					if (inst.opcode == OPCODE_wide) {
						inst.wide.opcode = (Opcode) br.readu1();
						if (inst.wide.opcode == OPCODE_iinc) {
							inst.wide.iinc.index = br.readu2();
							inst.wide.iinc.value = br.readu2();
						} else {
							inst.wide.var.lvindex = br.readu2();
						}
						//assert(false, "wide not supported yet");
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
					short targetOffset = br.readu2();
					//inst.jump.label = targetOffset;

					short labelpos = offset + targetOffset;
					check(labelpos >= 0,
							"invalid target for jump: must be >= 0");
					check(labelpos < br.size(), "invalid target for jump");

					//	fprintf(stderr, "target offset @ parse: %d\n", targetOffset);

					inst.jump.label2 = labels[offset + targetOffset];

					check(inst.jump.label2 != nullptr, "invalid label");
					break;
				}
				case KIND_TABLESWITCH: {
					for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
						u1 pad = br.readu1();
						assert(pad == 0, "Padding must be zero");
					}

					{
						bool check2 = br.offset() % 4 == 0;
						assert(check2, "%d", br.offset());
					}

					int defOffset = br.readu4();
					inst.ts.def = labels[offset + defOffset];
					inst.ts.low = br.readu4();
					inst.ts.high = br.readu4();

					assert(inst.ts.low <= inst.ts.high,
							"low (%d) must be less or equal than high (%d)",
							inst.ts.low, inst.ts.high);

					for (int i = 0; i < inst.ts.high - inst.ts.low + 1; i++) {
						u4 targetOffset = br.readu4();
						inst.ts.targets.push_back(
								labels[offset + targetOffset]);
					}

					//		fprintf(stderr, "parser ts: offset: %d\n", br.offset());

					break;
				}
				case KIND_LOOKUPSWITCH: {
					for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
						u1 pad = br.readu1();
						assert(pad == 0, "Padding must be zero");
					}

					int defOffset = br.readu4();
					inst.ls.defbyte = labels[offset + defOffset];
					inst.ls.npairs = br.readu4();

					for (u4 i = 0; i < inst.ls.npairs; i++) {
						u4 key = br.readu4();
						u4 offsetTarget = br.readu4();

						inst.ls.keys.push_back(key);
						inst.ls.targets.push_back(
								labels[offset + offsetTarget]);
					}
					break;
				}
				case KIND_FIELD:
					inst.field.fieldRefIndex = br.readu2();
					break;
				case KIND_INVOKE:
					inst.invoke.methodRefIndex = br.readu2();
					break;
				case KIND_INVOKEINTERFACE:
					inst.invokeinterface.interMethodRefIndex = br.readu2();
					inst.invokeinterface.count = br.readu1();

					assert(inst.invokeinterface.count != 0, "Count is zero!");
					{
						u1 zero = br.readu1();
						assert(zero == 0, "Fourth operand must be zero");
					}
					break;
				case KIND_INVOKEDYNAMIC:
					raise("FrParseInvokeDynamicInstr not implemented");
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
					raise("FrParse4__TODO__Instr not implemented");
					break;
				case KIND_RESERVED:
					raise("FrParseReservedInstr not implemented");
					break;
				default:
					raise("default kind in parseInstList");

			}

			instList.push_back(instp);
		}
	}

	static Attr* parseCode(BufferReader& br, Attrs& as, ConstPool& cp,
			u2 nameIndex) {

		CodeAttr* ca = new CodeAttr(nameIndex);

		ca->maxStack = br.readu2();
		ca->maxLocals = br.readu2();

		u4 codeLen = br.readu4();

		check(codeLen > 0, "");
		check(codeLen < (2 << 16), "");

		ca->codeLen = codeLen;

		const u1* codeBuf = br.pos();
		br.skip(ca->codeLen);

		Inst** labels = new Inst*[codeLen];
		for (u4 i = 0; i < codeLen; i++) {
			labels[i] = nullptr;
		}

		{
			BufferReader br(codeBuf, codeLen);
			parseInstTargets(br, labels);
		}

		auto getLabel = [&](u2 labelpos) {
			Inst*& lab = labels[labelpos];
			if (lab == nullptr) {
				lab = new Inst(KIND_LABEL);
			}

			return lab;
		};

		u2 exceptionTableCount = br.readu2();
		for (int i = 0; i < exceptionTableCount; i++) {
			u2 startPc = br.readu2();
			u2 endPc = br.readu2();
			u2 handlerPc = br.readu2();
			ConstPool::Index catchType = br.readu2();

			check(startPc < endPc, "");
			check(endPc <= ca->codeLen, "");
			check(handlerPc < ca->codeLen, "");
			check(catchType == ConstPool::NULLENTRY || cp.isClass(catchType),
					"");

			CodeExceptionEntry e;
			e.startpc = getLabel(startPc);
			e.endpc = getLabel(endPc);
			e.handlerpc = getLabel(handlerPc);
			e.catchtype = catchType;

			ca->exceptions.push_back(e);
		}

		parseAttrs(br, cp, ca->attrs, labels);

		{
			BufferReader br(codeBuf, codeLen);
			parseInstList(br, ca->instList, labels);
		}

		as.add(ca);

		delete[] labels;

		return ca;
	}

	static Attr* parseLnt(BufferReader& br, Attrs& /*as*/, u2 nameIndex,
			void* /*args*/) {
		//Inst** labels = (Inst**) args;

		u2 lntlen = br.readu2();

		LntAttr* lnt = new LntAttr(nameIndex);

		for (int i = 0; i < lntlen; i++) {
			LntAttr::LnEntry e;
			u2 startpc = br.readu2();
			u2 lineno = br.readu2();

//		e.startPcLabel = createLabel(labels, startpc);

			e.startpc = startpc;
			e.lineno = lineno;

			lnt->lnt.push_back(e);
		}

		//as.add(lnt);

		return lnt;
	}

	static Attr* parseLvt(BufferReader& br, Attrs& as, u2 nameIndex,
			void* args) {
		Inst** labels = (Inst**) args;

		u2 count = br.readu2();

		LvtAttr* lvt = new LvtAttr(ATTR_LVT, nameIndex);

		for (u2 i = 0; i < count; i++) {
			LvtAttr::LvEntry e;

			u2 startPc = br.readu2();

			e.startPcLabel = createLabel(labels, startPc);

			e.startPc = startPc;
			e.len = br.readu2();
			e.varNameIndex = br.readu2();
			e.varDescIndex = br.readu2();
			e.index = br.readu2();

			lvt->lvt.push_back(e);
		}

		as.add(lvt);

		return lvt;
	}

	static Attr* parseLvtt(BufferReader& br, Attrs& as, u2 nameIndex,
			void* args) {
		Inst** labels = (Inst**) args;

		u2 count = br.readu2();

		LvtAttr* lvt = new LvtAttr(ATTR_LVTT, nameIndex);

		for (u2 i = 0; i < count; i++) {
			LvtAttr::LvEntry e;

			u2 startPc = br.readu2();

			e.startPcLabel = createLabel(labels, startPc);

			e.startPc = startPc;
			e.len = br.readu2();
			e.varNameIndex = br.readu2();

			// Signature instead of descriptor.
			e.varDescIndex = br.readu2();
			e.index = br.readu2();

			lvt->lvt.push_back(e);
		}

		as.add(lvt);

		return lvt;
	}

	static Attr* parseSmt(BufferReader& br, Attrs& as, ConstPool&, u2 nameIndex,
			void* args) {

		Inst** labels = (Inst**) args;

		SmtAttr* smt = new SmtAttr(nameIndex);

		auto parseType = [&](BufferReader& br, Inst** labels) {
			u1 tag = br.readu1();

			switch (tag) {
				case Type::TYPE_TOP:
				return Type::top();
				case Type::TYPE_INTEGER:
				return Type::intt();
				case Type::TYPE_FLOAT:
				return Type::floatt();
				case Type::TYPE_LONG:
				return Type::longt();
				case Type::TYPE_DOUBLE:
				return Type::doublet();
				case Type::TYPE_NULL:
				return Type::nullt();
				case Type::TYPE_UNINITTHIS:
				return Type::uninitthist();
				case Type::TYPE_OBJECT: {
					u2 cpIndex = br.readu2();
					return Type::objectt(cpIndex);
				}
				case Type::TYPE_UNINIT: {
					u2 offset = br.readu2();

					Inst*& label = labels[offset];
					if (label == nullptr) {
						label = new Inst(KIND_LABEL);
					}

					return Type::uninitt(offset, label);
				}
			}

			raise("Error on parse smt");
		};

		auto parseTs = [&](int count, vector<Type>& locs) {
			for (u1 i = 0; i < count; i++) {
				Type t = parseType(br, labels);
				locs.push_back(t);
			}
		};

		u2 numberOfEntries = br.readu2();

		int toff = -1;

		for (u2 i = 0; i < numberOfEntries; i++) {
			u1 frameType = br.readu1();

			SmtAttr::Entry e;
			e.frameType = frameType;

			if (0 <= frameType && frameType <= 63) {
				//	v.visitFrameSame(frameType);
				toff += frameType;
			} else if (64 <= frameType && frameType <= 127) {
				parseTs(1, e.sameLocals_1_stack_item_frame.stack);
				//v.visitFrameSameLocals1StackItem(frameType);

				toff += frameType - 64;
			} else if (frameType == 247) {
				u2 offsetDelta = br.readu2();
				e.same_locals_1_stack_item_frame_extended.offset_delta =
						offsetDelta;

				toff += e.same_locals_1_stack_item_frame_extended.offset_delta;
				parseTs(1, e.same_locals_1_stack_item_frame_extended.stack);
			} else if (248 <= frameType && frameType <= 250) {
				u2 offsetDelta = br.readu2();
				e.chop_frame.offset_delta = offsetDelta;

				toff += e.chop_frame.offset_delta;
			} else if (frameType == 251) {
				u2 offsetDelta = br.readu2();
				e.same_frame_extended.offset_delta = offsetDelta;

				toff += e.same_frame_extended.offset_delta;
			} else if (252 <= frameType && frameType <= 254) {
				u2 offsetDelta = br.readu2();
				e.append_frame.offset_delta = offsetDelta;
				parseTs(frameType - 251, e.append_frame.locals);

				toff += e.append_frame.offset_delta;
			} else if (frameType == 255) {
				u2 offsetDelta = br.readu2();
				e.full_frame.offset_delta = offsetDelta;

				u2 numberOfLocals = br.readu2();
				parseTs(numberOfLocals, e.full_frame.locals);

				u2 numberOfStackItems = br.readu2();
				parseTs(numberOfStackItems, e.full_frame.stack);

				toff += e.full_frame.offset_delta;
			}

			toff += 1;

			Inst*& label = labels[toff];

			if (label == nullptr) {
				//fprintf(stderr, "WARNING: Label is null in smt at offset %d", toff);

				label = new Inst(KIND_LABEL);
			}

			e.label = label;

			smt->entries.push_back(e);
		}

		as.add(smt);

		return smt;
	}

	static void parseAttrs(BufferReader& br, ConstPool& cp, Attrs& as,
			void* args = nullptr) {
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
				a = parseLnt(br, as, nameIndex, args);
			} else if (attrName == "LocalVariableTable") {
				BufferReader br(data, len);
				a = parseLvt(br, as, nameIndex, args);
			} else if (attrName == "StackMapTable") {
				BufferReader br(data, len);
				a = parseSmt(br, as, cp, nameIndex, args);
			} else if (attrName == "LocalVariableTypeTable") {
				BufferReader br(data, len);
				a = parseLvtt(br, as, nameIndex, args);
			} else {
				a = new UnknownAttr(nameIndex, len, data);
				as.add(a);
			}

			a->len = len;

			br.skip(len);
		}
	}

};

ClassFile::ClassFile(const u1* classFileData, const int classFileLen) :
		majorVersion(0), minorVersion(0), accessFlags(0), thisClassIndex(0), superClassIndex(
				0) {
	ClassParser::parseClassFile(classFileData, classFileLen, *this);
}

}

