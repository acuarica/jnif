#ifndef JNIF_CLASSWRITER_HPP
#define JNIF_CLASSWRITER_HPP

#include "base.hpp"
#include "ClassDefaultVisitor.hpp"

namespace jnif {

/**
 *
 */
template<typename TWriter>
class ClassWriter {
public:

	class Field {
		friend ClassWriter;
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
		ClassWriter& cpv;
		inline Method(Member& m, ClassWriter& cpv) :
				m(m), cpv(cpv) {
		}

		class Code;

		Code visitCode(u2 nameIndex) {
			auto attr = new CodeAttr(nameIndex);
			m.attrs.add(attr);
			return Code(attr);
		}

		void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			m.attrs.add(new UnknownAttr(nameIndex, len, data));
		}

	private:

	};

	inline ClassWriter(TWriter& w) :
			w(w) {
	}

	inline void visitVersion(Magic magic, u2 minor, u2 major) {
		w.writeu4(magic);
		w.writeu2(minor);
		w.writeu2(major);
	}

	inline void visitConstPool(ConstPool& cp) {
		writeConstPool(w,cp);
	}

	inline void visitThis(u2 accessFlags, u2 thisClassIndex,
			u2 superClassIndex) {
		w.writeu2(accessFlags);
		w.writeu2(thisClassIndex);
		w.writeu2(superClassIndex);
	}

	inline void visitInterfaceCount(u2 count) {
		w.writeu2(count);
	}

	inline void visitInterface(u2 interIndex) {
		w.writeu2(interIndex);
	}

	inline void visitFieldCount(u2 count) {
		w.writeu2(count);
	}

	inline Field visitField(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		Member& f = cf.fields.add(accessFlags, nameIndex, descIndex);

		return Field(f);
	}

	inline void visitMethodCount(u2 count) {
		w.writeu2(count);
	}

	inline Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		w.writeu2(accessFlags);
		w.writeu2(nameIndex);
		w.writeu2(descIndex);

		Member& m = cf.methods.add(accessFlags, nameIndex, descIndex);

		return Method(m, *this);
	}

	inline void visitSourceFile(u2 nameIndex, u2 sourceFileIndex) {
		cv.visitSourceFile(nameIndex, sourceFileIndex);
	}

	inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
		w.writecount(data, len);
		//cf.attrs.add(new UnknownAttr(nameIndex, len, data));

		cv.visitAttr(nameIndex, len, data);
	}

	static void writeClassFile(BufferWriter& bw, ClassFile& cf) {

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

	static void writeConstPool(TWriter& bw, const ConstPool& cp) {
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
					bw.writeu2(
							entry->invokedynamic.bootstrap_method_attr_index);
					bw.writeu2(entry->invokedynamic.name_and_type_index);
					break;
			}
		}
	}

//private:
	TWriter& w;
};

}

#endif
