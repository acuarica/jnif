#include "jnif.hpp"
#include "jnifex.hpp"

using namespace std;

namespace jnif {

/**
 * Implements a memory buffer reader in big-endian encoding.
 */
class BufferReader {
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
		//Error::check(off == _size, "Expected end of buffer");
	}

	int size() const {
		return _size;
	}

	u1 readu1() {
		Error::check(off + 1 <= _size, "Invalid read");

		u1 result = buffer[off];

		off += 1;

		return result;
	}

	u2 readu2() {
		Error::check(off + 2 <= _size, "Invalid read 2");

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];

		u2 result = r0 << 8 | r1;

		off += 2;

		return result;
	}

	u4 readu4() {
		Error::check(off + 4 <= _size, "Invalid read 4");
		//if (off >= 256 ) Error::raise()

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
		Error::check(off + count <= _size, m, count, off);

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

class LabelManager {
public:

	LabelManager(u4 codeLen, InstList& instList) :
			codeLen(codeLen), instList(instList), labels(
					new LabelInst*[codeLen]) {
		for (u4 i = 0; i < codeLen; i++) {
			labels[i] = nullptr;
		}
	}

	~LabelManager() {
		delete[] labels;
	}

	LabelInst* createLabel(int labelPos) {
		Error::assert(0 <= labelPos, "Invalid position for label: ", labelPos);
		Error::assert((u4) labelPos < codeLen, "Invalid position for label: ",
				labelPos);

		LabelInst*& lab = labels[labelPos];
		if (lab == nullptr) {
			lab = instList.createLabel();
		}

		return lab;
	}

	LabelInst* createExceptionLabel(u2 labelPos, bool isTryStart,
			bool isCatchHandler) {
		LabelInst* label = createLabel(labelPos);
		label->isTryStart = label->isTryStart || isTryStart;
		label->isCatchHandler = label->isCatchHandler || isCatchHandler;

		return label;
	}

	bool hasLabel(u2 labelPos) const {
		Error::assert(labelPos < codeLen, "Invalid position for label: ",
				labelPos);

		return labels[labelPos] != nullptr;
	}

	LabelInst* operator[](u2 labelPos) const {
		Error::assert(hasLabel(labelPos), "No label in position: ", labelPos);

		return labels[labelPos];
	}

private:

	u4 codeLen;

	InstList& instList;

	LabelInst** labels;
};

class ClassParser: private Error {
public:

	static void parseClassFile(const u1* fileImage, const int fileImageLen,
			ClassFile& cf) {
		BufferReader br(fileImage, fileImageLen);

		u4 magic = br.readu4();

		check(magic == CLASSFILE_MAGIC,
				"Invalid magic number. Expected 0xcafebabe, found: ", magic);

		cf.version.minor = br.readu2();
		cf.version.major = br.readu2();

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
			u2 accessFlags = br.readu2();
			u2 nameIndex = br.readu2();
			u2 descIndex = br.readu2();

			Field* f = cf.addField(nameIndex, descIndex, accessFlags);

			parseAttrs(br, cf, *f);
		}

		u2 methodCount = br.readu2();
		for (int i = 0; i < methodCount; i++) {
			u2 accessFlags = br.readu2();
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
				case CONST_CLASS: {
					u2 classNameIndex = br.readu2();
					cp.addClass(classNameIndex);
					break;
				}
				case CONST_FIELDREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addFieldRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONST_METHODREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addMethodRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONST_INTERMETHODREF: {
					u2 classIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addInterMethodRef(classIndex, nameAndTypeIndex);
					break;
				}
				case CONST_STRING: {
					u2 utf8Index = br.readu2();
					cp.addString(utf8Index);
					break;
				}
				case CONST_INTEGER: {
					u4 value = br.readu4();
					cp.addInteger(value);
					break;
				}
				case CONST_FLOAT: {
					u4 value = br.readu4();
					float fvalue = *(float*) &value;
					//auto idx =
							cp.addFloat(fvalue);

					//float v = cp.entries[idx].f.value;
					//Error::assert(value== *(u4*) &v, "invalid: ", br.offset() );
//					float fvalue = entry->f.value;
//					u4 value = *(u4*) &fvalue;

					break;
				}
				case CONST_LONG: {
					u4 high = br.readu4();
					u4 low = br.readu4();
					long value = ((long) high << 32) + low;
					cp.addLong(value);
					i++;
					break;
				}
				case CONST_DOUBLE: {
					u4 high = br.readu4();
					u4 low = br.readu4();
					long lvalue = ((long) high << 32) + low;
					double dvalue = *(double*) &lvalue;
					cp.addDouble(dvalue);
					i++;
					break;
				}
				case CONST_NAMEANDTYPE: {
					u2 nameIndex = br.readu2();
					u2 descIndex = br.readu2();
					cp.addNameAndType(nameIndex, descIndex);
					break;
				}
				case CONST_UTF8: {
					u2 len = br.readu2();
					cp.addUtf8((const char*) br.pos(), len);
					br.skip(len);
					break;
				}
				case CONST_METHODHANDLE: {
					u1 refKind = br.readu1();
					u2 refIndex = br.readu2();
					cp.addMethodHandle(refKind, refIndex);
					break;
				}
				case CONST_METHODTYPE: {
					u2 descIndex = br.readu2();
					cp.addMethodType(descIndex);
					break;
				}
				case CONST_INVOKEDYNAMIC: {
					u2 bootstrapMethodAttrIndex = br.readu2();
					u2 nameAndTypeIndex = br.readu2();
					cp.addInvokeDynamic(bootstrapMethodAttrIndex,
							nameAndTypeIndex);
					break;
				}
				default:
					Error::raise("Error while reading tag: ", tag);
			}
		}
	}

	static Attr* parseSourceFile(BufferReader& br, ConstIndex nameIndex,
			ConstPool* constPool) {
		u2 sourceFileIndex = br.readu2();

		Attr* attr = new SourceFileAttr(nameIndex, sourceFileIndex, constPool);

		return attr;
	}

	static Attr* parseExceptions(BufferReader& br, ConstIndex nameIndex,
			ConstPool* constPool) {
		u2 len = br.readu2();

		std::vector<ConstIndex> es;
		for (int i = 0; i < len; i++) {
			ConstIndex exceptionIndex = br.readu2();

			es.push_back(exceptionIndex);
		}

		Attr* attr = new ExceptionsAttr(nameIndex, constPool, es);

		return attr;
	}

	static void parseInstTargets(BufferReader& br, LabelManager& labelManager) {
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

					labelManager.createLabel(labelpos);
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
					labelManager.createLabel(offset + defOffset);

					int low = br.readu4();
					int high = br.readu4();

					Error::assert(low <= high,
							"low (%d) must be less or equal than high (%d)",
							low, high);

					for (int i = 0; i < high - low + 1; i++) {
						int targetOffset = br.readu4();
						labelManager.createLabel(offset + targetOffset);
					}
					break;
				}
				case KIND_LOOKUPSWITCH: {
					for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
						u1 pad = br.readu1();
						assert(pad == 0, "Padding must be zero");
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
					Error::raise("default kind in parseInstTargets: "
							"opcode: %d, kind: %d", opcode, kind);
			}
		}
	}

	static void parseInstList(BufferReader& br, InstList& instList,
			LabelManager& labelManager) {
		while (!br.eor()) {
			int offset = br.offset();

			if (labelManager.hasLabel(offset)) {
				LabelInst* label = labelManager[offset];
				label->_offset = offset;
				instList.addLabel(label);
			}

			Opcode opcode = (Opcode) br.readu1();
			u1 kind = OPKIND[opcode];

			//Inst* instp = new Inst(opcode, kind);
			//instp->_offset = offset;
			//Inst& inst = *instp;

			//inst.opcode = (Opcode) br.readu1();
			//inst.kind = OPKIND[inst.opcode];

			if (kind == KIND_ZERO) {
				if (opcode == OPCODE_wide) {
					Opcode subOpcode = (Opcode) br.readu1();
					if (subOpcode == OPCODE_iinc) {
						u2 index = br.readu2();
						u2 value = br.readu2();

						instList.addWideIinc(index, value);
					} else {
						u2 lvindex = br.readu2();
						instList.addWideVar(subOpcode, lvindex);
					}
				} else {
					instList.addZero(opcode);
				}
			} else if (kind == KIND_BIPUSH) {
				u1 value = br.readu1();
				instList.addBiPush(value);
			} else if (kind == KIND_SIPUSH) {
				u2 value = br.readu2();
				instList.addSiPush(value);
			} else if (kind == KIND_LDC) {
				u2 valueIndex;
				if (opcode == OPCODE_ldc) {
					valueIndex = br.readu1();
				} else {
					valueIndex = br.readu2();
				}
				instList.addLdc(opcode, valueIndex);
			} else if (kind == KIND_VAR) {
				u1 lvindex = br.readu1();
				instList.addVar(opcode, lvindex);
			} else if (kind == KIND_IINC) {
				u1 index = br.readu1();
				u1 value = br.readu1();
				instList.addIinc(index, value);
			} else if (kind == KIND_JUMP) {
				//
				short targetOffset = br.readu2();
				//inst.jump.label = targetOffset;

				short labelpos = offset + targetOffset;
				Error::check(labelpos >= 0,
						"invalid target for jump: must be >= 0");
				Error::check(labelpos < br.size(), "invalid target for jump");

				//	fprintf(stderr, "target offset @ parse: %d\n", targetOffset);

				LabelInst* targetLabel = labelManager[offset + targetOffset];
				Error::check(targetLabel != nullptr, "invalid label");

				instList.addJump(opcode, targetLabel);
			} else if (kind == KIND_TABLESWITCH) {
				for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
					u1 pad = br.readu1();
					Error::assert(pad == 0, "Padding must be zero");
				}

				{
					bool check2 = br.offset() % 4 == 0;
					assert(check2, "%d", br.offset());
				}

				int defOffset = br.readu4();
				LabelInst* def = labelManager[offset + defOffset];
				int low = br.readu4();
				int high = br.readu4();

				Error::assert(low <= high,
						"low (%d) must be less or equal than high (%d)", low,
						high);

				TableSwitchInst* ts = instList.addTableSwitch(def, low, high);
				for (int i = 0; i < high - low + 1; i++) {
					u4 targetOffset = br.readu4();
					ts->targets.push_back(labelManager[offset + targetOffset]);
				}

				//		fprintf(stderr, "parser ts: offset: %d\n", br.offset());

			} else if (kind == KIND_LOOKUPSWITCH) {
				for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
					u1 pad = br.readu1();
					assert(pad == 0, "Padding must be zero");
				}

				int defOffset = br.readu4();
				LabelInst* defbyte = labelManager[offset + defOffset];
				u4 npairs = br.readu4();

				LookupSwitchInst* ls = instList.addLookupSwitch(defbyte,
						npairs);
				for (u4 i = 0; i < npairs; i++) {
					u4 key = br.readu4();
					u4 offsetTarget = br.readu4();

					ls->keys.push_back(key);
					ls->targets.push_back(labelManager[offset + offsetTarget]);
				}
			} else if (kind == KIND_FIELD) {
				u2 fieldRefIndex = br.readu2();

				instList.addField(opcode, fieldRefIndex);
			} else if (kind == KIND_INVOKE) {
				u2 methodRefIndex = br.readu2();

				instList.addInvoke(opcode, methodRefIndex);
			} else if (kind == KIND_INVOKEINTERFACE) {
				Error::assert(opcode == OPCODE_invokeinterface,
						"invalid opcode");

				u2 interMethodRefIndex = br.readu2();
				u1 count = br.readu1();

				Error::assert(count != 0, "Count is zero!");

				u1 zero = br.readu1();
				Error::assert(zero == 0, "Fourth operand must be zero");

				instList.addInvokeInterface(interMethodRefIndex, count);
			} else if (kind == KIND_INVOKEDYNAMIC) {
				Error::raise("FrParseInvokeDynamicInstr not implemented");
			} else if (kind == KIND_TYPE) {
				ConstIndex classIndex = br.readu2();

				instList.addType(opcode, classIndex);
			} else if (kind == KIND_NEWARRAY) {
				u1 atype = br.readu1();

				instList.addNewArray(atype);
			} else if (kind == KIND_MULTIARRAY) {
				ConstIndex classIndex = br.readu2();
				u1 dims = br.readu1();

				instList.addMultiArray(classIndex, dims);
			} else if (kind == KIND_PARSE4TODO) {
				Error::raise("FrParse4__TODO__Instr not implemented");
			} else if (kind == KIND_RESERVED) {
				Error::raise("FrParseReservedInstr not implemented");
			} else {
				Error::raise("default kind in parseInstList");
			}
		}
	}

	static Attr* parseCode(BufferReader& br, ConstPool& cp, u2 nameIndex) {

		CodeAttr* ca = new CodeAttr(nameIndex, &cp);

		ca->maxStack = br.readu2();
		ca->maxLocals = br.readu2();

		u4 codeLen = br.readu4();

		check(codeLen > 0, "");
		check(codeLen < (2 << 16), "");

		ca->codeLen = codeLen;

		const u1* codeBuf = br.pos();
		br.skip(ca->codeLen);

		LabelManager labelManager(codeLen, ca->instList);

		{
			BufferReader br(codeBuf, codeLen);
			parseInstTargets(br, labelManager);
		}

		u2 exceptionTableCount = br.readu2();
		for (int i = 0; i < exceptionTableCount; i++) {
			u2 startPc = br.readu2();
			u2 endPc = br.readu2();
			u2 handlerPc = br.readu2();
			ConstIndex catchType = br.readu2();

			check(startPc < endPc, "");
			check(endPc <= ca->codeLen, "");
			check(handlerPc < ca->codeLen, "");
			check(catchType == ConstPool::NULLENTRY || cp.isClass(catchType),
					"");

			CodeExceptionEntry e;
			e.startpc = labelManager.createExceptionLabel(startPc, true, false);
			e.endpc = labelManager.createExceptionLabel(endPc, false, false);
			e.handlerpc = labelManager.createExceptionLabel(handlerPc, false,
					true);
			e.catchtype = catchType;

			ca->exceptions.push_back(e);
		}

		parseAttrs(br, cp, ca->attrs, &labelManager);

		{
			BufferReader br(codeBuf, codeLen);
			parseInstList(br, ca->instList, labelManager);
		}

		return ca;
	}

	static Attr* parseLnt(BufferReader& br, u2 nameIndex, void* args,
			ConstPool* constPool) {
		LabelManager& labelManager = *(LabelManager*) args;

		u2 lntlen = br.readu2();

		LntAttr* lnt = new LntAttr(nameIndex, constPool);

		for (int i = 0; i < lntlen; i++) {
			LntAttr::LnEntry e;
			u2 startpc = br.readu2();
			u2 lineno = br.readu2();

			e.startPcLabel = labelManager.createLabel(startpc);

			e.startpc = startpc;
			e.lineno = lineno;

			lnt->lnt.push_back(e);
		}

		return lnt;
	}

	static Attr* parseLvt(BufferReader& br, u2 nameIndex, ConstPool* constPool,
			void* args) {
		LabelManager& labelManager = *(LabelManager*) args;

		u2 count = br.readu2();

		LvtAttr* lvt = new LvtAttr(ATTR_LVT, nameIndex, constPool);

		for (u2 i = 0; i < count; i++) {
			LvtAttr::LvEntry e;

			u2 startPc = br.readu2();

			e.startPcLabel = labelManager.createLabel(startPc);

			e.startPc = startPc;
			e.len = br.readu2();
			e.varNameIndex = br.readu2();
			e.varDescIndex = br.readu2();
			e.index = br.readu2();

			lvt->lvt.push_back(e);
		}

		return lvt;
	}

	static Attr* parseLvtt(BufferReader& br, u2 nameIndex, ConstPool* constPool,
			void* args) {
		LabelManager& labelManager = *(LabelManager*) args;

		u2 count = br.readu2();

		LvtAttr* lvt = new LvtAttr(ATTR_LVTT, nameIndex, constPool);

		for (u2 i = 0; i < count; i++) {
			LvtAttr::LvEntry e;

			u2 startPc = br.readu2();

			e.startPcLabel = labelManager.createLabel(startPc);

			e.startPc = startPc;
			e.len = br.readu2();
			e.varNameIndex = br.readu2();

			// Signature instead of descriptor.
			e.varDescIndex = br.readu2();
			e.index = br.readu2();

			lvt->lvt.push_back(e);
		}

		return lvt;
	}

	static Attr* parseSmt(BufferReader& br, ConstPool& cp, u2 nameIndex,
			void* args) {

		LabelManager& labelManager = *(LabelManager*) args;

		SmtAttr* smt = new SmtAttr(nameIndex, &cp);

		auto parseType = [&](BufferReader& br) {
			u1 tag = br.readu1();

			switch (tag) {
				case TYPE_TOP:
				return Type::topType();
				case TYPE_INTEGER:
				return Type::intType();
				case TYPE_FLOAT:
				return Type::floatType();
				case TYPE_LONG:
				return Type::longType();
				case TYPE_DOUBLE:
				return Type::doubleType();
				case TYPE_NULL:
				return Type::nullType();
				case TYPE_UNINITTHIS:
				return Type::uninitThisType();
				case TYPE_OBJECT: {
					u2 cpIndex = br.readu2();
					check(cp.isClass(cpIndex), "Bad cpindex: ", cpIndex);
					string className = cp.getClassName(cpIndex);
					return Type::objectType(className, cpIndex);
				}
				case TYPE_UNINIT: {
					u2 offset = br.readu2();

					LabelInst* label = labelManager.createLabel(offset);
//					if (label == nullptr) {
//						label = new Inst(KIND_LABEL);
//					}

				return Type::uninitType(offset, label);
			}
		}

		raise("Error on parse smt");
	}	;

		auto parseTs = [&](int count, vector<Type>& locs) {
			for (u1 i = 0; i < count; i++) {
				Type t = parseType(br);
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

//			Inst*& label = labels[toff];
//			if (label == nullptr) {
//				//fprintf(stderr, "WARNING: Label is null in smt at offset %d", toff);
//				label = new Inst(KIND_LABEL);
//			}
			LabelInst* label = labelManager.createLabel(toff);

			e.label = label;

			smt->entries.push_back(e);
		}

		return smt;
	}

	static void parseAttrs(BufferReader& br, ConstPool& cp, Attrs& as,
			void* args = nullptr) {

		auto parseAttr = [&]() -> Attr* {
			u2 nameIndex = br.readu2();
			u4 len = br.readu4();
			const u1* data = br.pos();

			br.skip(len);

			string attrName = cp.getUtf8(nameIndex);

			if (attrName == "SourceFile") {
				BufferReader br(data, len);
				return parseSourceFile(br, nameIndex, &cp);
			} else if (attrName == "Exceptions") {
				BufferReader br(data, len);
				return parseExceptions(br, nameIndex, &cp);
			} else if (attrName == "Code") {
				BufferReader br(data, len);
				return parseCode(br, cp, nameIndex);
			} else if (attrName == "LineNumberTable") {
				BufferReader br(data, len);
				return parseLnt(br, nameIndex, args, &cp);
			} else if (attrName == "LocalVariableTable") {
				BufferReader br(data, len);
				return parseLvt(br, nameIndex, &cp, args);
			} else if (attrName == "StackMapTable") {
				BufferReader br(data, len);
				return parseSmt(br, cp, nameIndex, args);
			} else if (attrName == "LocalVariableTypeTable") {
				BufferReader br(data, len);
				return parseLvtt(br, nameIndex, &cp, args);
			} else {
				return new UnknownAttr(nameIndex, len, data, &cp);
			}
		};

		u2 attrCount = br.readu2();
		for (int i = 0; i < attrCount; i++) {
			Attr* a = parseAttr();

			as.add(a);
			//a->len = l;
		}
	}

};

ClassFile::ClassFile(const u1* classFileData, const int classFileLen) :
		version(0, 0), accessFlags(0), thisClassIndex(0), superClassIndex(0) {
	ClassParser::parseClassFile(classFileData, classFileLen, *this);
}

}

