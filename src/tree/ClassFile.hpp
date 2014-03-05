#ifndef JNIF_CLASSFILE_HPP
#define JNIF_CLASSFILE_HPP

#include "../base.hpp"
#include <vector>

namespace jnif {

struct Visitor {

	virtual ~Visitor() {
	}
	virtual void visitSourceFile(SourceFileAttr&) = 0;
	virtual void visitUnknown(UnknownAttr&) = 0;
};

/**
 * Represents a Java Class File following version 7.
 *
 * Definitions taken from Chapter 4. The class File Format from the
 * Java Virtual Machine Specification
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 */
struct ClassFile: Attrs {
	u4 magic;
	u2 minor;
	u2 major;
	ConstPool cp;
	u2 accessFlags;
	u2 thisClassIndex;
	u2 superClassIndex;
	std::vector<u2> interfaces;
	Members fields;
	Members methods;

	void write(u1* new_class_data, int newlen) {
		BufferWriter bw(new_class_data, newlen);
		writeClassFile(bw, *this);
	}

	u4 size() {
		return getClassFileSize(*this);
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
		cf.writeAttrs(bw);
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
					bw.writeu2(
							entry->invokedynamic.bootstrap_method_attr_index);
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

			mi.writeAttrs(bw);
		}
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
		size += getAttrsSize(cf);

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

			size += getAttrsSize(mi);
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
