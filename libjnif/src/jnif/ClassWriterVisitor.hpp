#ifndef JNIF_CLASSWRITERVISITOR_HPP
#define JNIF_CLASSWRITERVISITOR_HPP

#include "base.hpp"

namespace jnif {

/**
 *
 */
template<typename TForward = ClassNullVisitor>
class ClassWriterVisitor {
public:

	class Field {
		friend ClassWriterVisitor;
	public:

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			f.attrs.add(new UnknownAttr(nameIndex, len, data));
		}

	private:

		Member& f;

		inline Field(Member& f) :
				f(f) {
		}
	};

	class Method {
	public:
		Member& m;
		ClassWriterVisitor& cpv;
		inline Method(Member& m, ClassWriterVisitor& cpv) :
				m(m), cpv(cpv) {
		}

		class Code;

		Code visitCode(u2 nameIndex) {
			auto attr = new CodeAttr(nameIndex);
			m.attrs.add(attr);
			return Code(attr);
		}

		class Code {
		public:
			CodeAttr* attr;

			inline Code(CodeAttr* attr) :
					attr(attr) {
			}

			void enter(u2 maxStack, u2 maxLocals) {
				attr->maxStack = maxStack;
				attr->maxLocals = maxLocals;
			}

			inline void visitExceptionEntry(u2 startpc, u2 endpc, u2 handlerpc,
					u2 catchtype) {
				CodeAttr::ExceptionEntry entry;
				entry.startpc = startpc;
				entry.endpc = endpc;
				entry.handlerpc = handlerpc;
				entry.catchtype = catchtype;

				attr->exceptions.push_back(entry);
			}

			int getSize() {
				u4 size = 0;

				size += sizeof(attr->maxStack);
				size += sizeof(attr->maxLocals);
				size += sizeof(pos()); // u4
				size += pos(); // Code size

				size += sizeof(u2);
				size += attr->exceptions.size() * 8;

				size += getAttrsSize(attr->attrs);

				return size;
			}

			void exit() {
				int size = getSize();
				attr->len = size;
			}

			void visitZero(int offset, u1 opcode) {
				line(offset, opcode);
			}

			inline void visitField(int offset, u1 opcode, u2 fieldRefIndex,
					const string& className, const string& name,
					const string& desc) {
				line(offset, opcode);
				writeu2(fieldRefIndex);
			}

			inline void visitBiPush(int offset, u1 opcode, u1 bytevalue) {
				line(offset, opcode);
				writeu1(bytevalue);
			}

			void visitSiPush(int offset, u1 opcode, u2 shortvalue) {
				line(offset, opcode);
				writeu2(shortvalue);
			}

			void visitNewArray(int offset, u1 opcode, u1 atype) {
				line(offset, opcode);
				writeu1(atype);
			}

			void visitType(int offset, u1 opcode, u2 classIndex,
					const string& className) {
				line(offset, opcode);
				writeu2(classIndex);
			}

			void visitJump(int offset, u1 opcode, u2 targetOffset) {
				line(offset, opcode);
				writeu2(targetOffset);
			}

			void visitMultiArray(int offset, u1 opcode, u2 classIndex,
					const string& className, u1 dims) {
				line(offset, opcode);
				writeu2(classIndex);
				writeu1(dims);
			}

			void visitIinc(int offset, u1 opcode, u1 index, u1 value) {
				line(offset, opcode);
				writeu1(index);
				writeu1(value);
			}

			void visitLdc(int offset, u1 opcode, u2 arg) {
				line(offset, opcode);

				if (opcode == OPCODE_ldc) {
					writeu1(arg);
				} else {
					writeu2(arg);
				}
			}

			void visitInvokeInterface(int offset, u1 opcode,
					u2 interMethodrefIndex, const string& className,
					const string& name, const string& desc, u1 count) {
				line(offset, opcode);
				writeu2(interMethodrefIndex);
				writeu1(count);
				writeu1(0);
			}

			void visitInvoke(int offset, u1 opcode, u2 methodrefIndex,
					const string& className, const string& name,
					const string& desc) {
				line(offset, opcode);
				writeu2(methodrefIndex);
			}

			void visitVar(int offset, u1 opcode, u2 lvindex) {
				line(offset, opcode);
				writeu1(lvindex);
			}

			void visitTableSwitch(int offset, u1 opcode, int def, int low,
					int high, const vector<u4>& targets) {
				line(offset, opcode);

				int pad = (4 - (pos() % 4)) % 4;
				for (int i = 0; i < pad; i++) {
					writeu1(0);
				}

				bool check = pos() % 4 == 0;
				ASSERT(check, "Padding offset must be mod 4: %d", pos());

				writeu4(def);
				writeu4(low);
				writeu4(high);

				for (int i = 0; i < high - low + 1; i++) {
					u4 t = targets[i];
					writeu4(t);
				}
			}

			void visitLookupSwitch(int offset, u1 opcode, u4 defbyte, u4 npairs,
					const vector<u4>& keys, const vector<u4>& targets) {
				line(offset, opcode);

				int pad = (4 - (pos() % 4)) % 4;
				for (int i = 0; i < pad; i++) {
					writeu1(0);
				}

				bool check = pos() % 4 == 0;
				ASSERT(check, "Padding offset must be mod 4: %d", pos());

				writeu4(defbyte);
				writeu4(npairs);

				for (u4 i = 0; i < npairs; i++) {
					u4 k = keys[i];
					writeu4(k);

					u4 t = targets[i];
					writeu4(t);
				}
			}

			inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
				attr->attrs.add(new UnknownAttr(nameIndex, len, data));
			}

			inline void visitLnt(u2 nameIndex, u2 startpc, u2 lineno) {
				//attr->attrs.add(new LntAttr(nameIndex, startpc, lineno));
			}

//		inline void visitLvt(u2 startPc, u2 len, u2 varNameIndex,
//				u2 varDescIndex, u2 index) {
//			line() << startPc;
//			line() << len;
//			line() << varNameIndex;
//			line() << varDescIndex;
//			line() << index;
//		}
			inline void line(int offset, u1 opcode) {
				writeu1(opcode);
			}

			inline void writeu1(u1 value) {
				attr->codeBuffer.write(&value, 1);
			}

			inline void writeu2(u2 value) {
				u1 r[2];
				r[0] = ((u1*) &value)[1];
				r[1] = ((u1*) &value)[0];
				attr->codeBuffer.write(r, 2);
			}

			inline void writeu4(u4 value) {
				u1 r[4];

				r[0] = ((u1*) &value)[3];
				r[1] = ((u1*) &value)[2];
				r[2] = ((u1*) &value)[1];
				r[3] = ((u1*) &value)[0];

				attr->codeBuffer.write(r, 4);
			}

			inline int pos() const {
				return attr->codeBuffer.tellp();
			}
		};

		void visitException(u2 nameIndex, vector<u2>& es) {
			u4 len = 0;

			len += sizeof(u2);
			len += es.size() * sizeof(u2);

			auto attr = new ExceptionsAttr(nameIndex, len, es);

			m.attrs.add(attr);
		}

		void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			m.attrs.add(new UnknownAttr(nameIndex, len, data));
		}

	private:

	};

	inline ClassWriterVisitor(ostream& os,
			TForward& cv = ClassNullVisitor::inst) :
			cv(cv), os(os) {
	}

	inline void visitVersion(u4 magic, u2 minor, u2 major) {
		cv.visitVersion(magic, minor, major);

		cf.magic = magic;
		cf.minor = minor;
		cf.major = major;
	}

	inline void visitConstPool(ConstPool& cp) {
		cv.visitConstPool(cp);

		cf.cp = cp;
	}

	inline void visitConstPool32(ConstPool& cp) {
		for (u4 i = 1; i < cp.entries.size(); i++) {
			const ConstPool::Entry* entry = &cp.entries[i];

			line() << entry->tag;

			switch (entry->tag) {
			case CONSTANT_Class:
				os << entry->clazz.name_index;
				break;
			case CONSTANT_Fieldref:
			case CONSTANT_Methodref:
			case CONSTANT_InterfaceMethodref:
				os << entry->memberref.class_index;
				os << entry->memberref.name_and_type_index;
				break;
			case CONSTANT_String:
				os << entry->s.string_index;
				break;
			case CONSTANT_Integer:
				os << entry->i.value;
				break;
			case CONSTANT_Float:
				os << entry->f.value;
				break;
			case CONSTANT_Long:
				os << entry->l.high_bytes;
				os << entry->l.low_bytes;
				i++;
				break;
			case CONSTANT_Double:
				os << entry->d.high_bytes;
				os << entry->d.low_bytes;
				i++;
				break;
			case CONSTANT_NameAndType:
				os << entry->nameandtype.name_index;
				os << entry->nameandtype.descriptor_index;
				break;
			case CONSTANT_Utf8:
				os << entry->utf8.str.length();
				os << entry->utf8.str.c_str();
				break;
			case CONSTANT_MethodHandle:
				os << entry->methodhandle.reference_kind;
				os << entry->methodhandle.reference_index;
				break;
			case CONSTANT_MethodType:
				os << entry->methodtype.descriptor_index;
				break;
			case CONSTANT_InvokeDynamic:
				os << entry->invokedynamic.bootstrap_method_attr_index;
				os << entry->invokedynamic.name_and_type_index;
				break;
			}
		}
	}

	inline void visitThis(u2 accessFlags, u2 thisClassIndex,
			u2 superClassIndex) {
		cv.visitThis(accessFlags, thisClassIndex, superClassIndex);

		cf.accessFlags = accessFlags;
		cf.thisClassIndex = thisClassIndex;
		cf.superClassIndex = superClassIndex;
	}

	inline void visitInterface(u2 interIndex) {
		cv.visitInterface(interIndex);

		cf.interfaces.push_back(interIndex);
	}

	inline Field visitField(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		cv.visitField(accessFlags, nameIndex, descIndex);

		Member& f = cf.fields.add(accessFlags, nameIndex, descIndex);

		return Field(f);
	}

	inline Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		cv.visitMethod(accessFlags, nameIndex, descIndex);

		Member& m = cf.methods.add(accessFlags, nameIndex, descIndex);

		return Method(m, *this);
	}

	inline void visitSourceFile(u2 nameIndex, u2 sourceFileIndex) {
		cf.attrs.add(new SourceFileAttr(nameIndex, 2, sourceFileIndex));

		cv.visitSourceFile(nameIndex, sourceFileIndex);
	}

	inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
		cf.attrs.add(new UnknownAttr(nameIndex, len, data));

		cv.visitAttr(nameIndex, len, data);
	}

	static void writeClassFile(BufferWriter& bw, ClassFile& cf) {
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
		cf.attrs.writeAttrs(bw);
	}

	static void writeConstPool(BufferWriter& bw, const ConstPool& cp) {
		u2 count = cp.entries.size();
		bw.writeu2(count);

		for (u4 i = 1; i < cp.entries.size(); i++) {
			const ConstPool::Entry* entry = &cp.entries[i];

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

	static void writeMembers(BufferWriter& bw, Members& members) {
		bw.writeu2(members.size());

		for (u4 i = 0; i < members.size(); i++) {

			Member& mi = members[i];

			bw.writeu2(mi.accessFlags);
			bw.writeu2(mi.nameIndex);
			bw.writeu2(mi.descIndex);

			mi.attrs.writeAttrs(bw);
		}
	}

//private:

	ClassFile cf;

	TForward& cv;
	ostream& os;

	inline ostream& line() {
		return os;
	}

	static u4 getClassFileSize(ClassFile& cf) {
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
		size += getAttrsSize(cf.attrs);

		return size;
	}

	static u4 getConstPoolSize(const ConstPool& cp) {
		u4 size = sizeof(u2);

		for (u4 i = 1; i < cp.entries.size(); i++) {
			const ConstPool::Entry* entry = &cp.entries[i];

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
	}

	static u4 getMembersSize(Members& members) {
		u4 size = sizeof(u2);

		for (u4 i = 0; i < members.size(); i++) {

			Member& mi = members[i];

			size += sizeof(mi.accessFlags);
			size += sizeof(mi.nameIndex);
			size += sizeof(mi.descIndex);

			size += getAttrsSize(mi.attrs);
		}

		return size;
	}

	static u4 getAttrsSize(const Attrs& attrs) {
		u4 size = sizeof(u2);

		for (u4 i = 0; i < attrs.size(); i++) {
			const Attr& attr = attrs[i];

			size += sizeof(attr.nameIndex);
			size += sizeof(attr.len);
			size += attr.len;
		}

		return size;
	}
};

}

#endif
