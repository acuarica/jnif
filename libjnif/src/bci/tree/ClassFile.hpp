#ifndef __BCI__CLASS_FILE__HPP__
#define	__BCI__CLASS_FILE__HPP__

#include "../base.hpp"
#include "ConstPool.hpp"
#include "../utils/BufferWriter.hpp"

namespace JNIFNS {

class ClassFile {
public:

	class Members;
	class Member;
	class Attrs;

	class Attr {
		Attr(const Attr&) = delete;
	public:
		u2 nameIndex;
		u4 len;

		virtual void write(BufferWriter& bw) = 0;
	protected:
		Attr(u2 nameIndex, u4 len = 0) :
				nameIndex(nameIndex), len(len) {
		}

	};

	class Attrs {
		friend ClassFile;
		friend Member;
		Attrs(const Attrs&) = delete;

	public:
		Attrs(Attrs&&) = default;
		inline Attrs() {
		}

		inline Attr* add(Attr* attr) {
			attrs.push_back(attr);

			return attr;
		}

		inline u2 size() const {
			return attrs.size();
		}

		inline const Attr& operator[](u2 index) const {
			return *attrs[index];
		}

		void writeAttrs(BufferWriter& bw) {
			bw.writeu2(size());

			for (u4 i = 0; i < size(); i++) {
				ClassFile::Attr& attr = *attrs[i];

				bw.writeu2(attr.nameIndex);
				bw.writeu4(attr.len);

				u1* pos = bw.pos();
				bw.skip(attr.len);
				{
					BufferWriter bw(pos, attr.len);
					attr.write(bw);
				}
			}
		}

	private:

		vector<Attr*> attrs;
	};

	class UnknownAttr: public Attr {
	public:

		UnknownAttr(u2 nameIndex, u4 len, const u1* data) :
				Attr(nameIndex, len), data(data) {
		}

		virtual void write(BufferWriter& bw) {
			bw.writecount(data, len);
		}

		const u1 * const data;
	};

//	class Lnt : Attr {
//	public:
//		Lnt(u2 nameIndex) :
//				Attr(nameIndex), maxStack(0), maxLocals(0) {
//		}
//
//	};

	class CodeAttr: public Attr {
	public:

		struct ExceptionEntry {
			u2 startpc;
			u2 endpc;
			u2 handlerpc;
			u2 catchtype;
		};

		CodeAttr(u2 nameIndex) :
				Attr(nameIndex), maxStack(0), maxLocals(0) {
		}

		u2 maxStack;
		u2 maxLocals;

		basic_stringstream<u1> codeBuffer;

		vector<ExceptionEntry> exceptions;

		Attrs attrs;

		virtual void write(BufferWriter& bw) {
			bw.writeu2(maxStack);
			bw.writeu2(maxLocals);

			u4 codeLen = codeBuffer.tellp();
			bw.writeu4(codeLen);

			u1* codeStart = bw.pos();
			bw.skip(codeLen);
			{
				BufferWriter bw(codeStart, codeLen);
				const basic_string<u1>& str = codeBuffer.str();
				bw.writecount(str.c_str(), codeLen);
			}

			bw.writeu2(exceptions.size());
			for (u4 i = 0; i < exceptions.size(); i++) {
				auto& e = exceptions[i];
				bw.writeu2(e.startpc);
				bw.writeu2(e.endpc);
				bw.writeu2(e.handlerpc);
				bw.writeu2(e.catchtype);
			}

			attrs.writeAttrs(bw);
		}
	};

	class SourceFileAttr: public Attr {
	public:
		SourceFileAttr(u2 nameIndex, u4 len, u2 sourceFileIndex) :
				Attr(nameIndex, len), sourceFileIndex(sourceFileIndex) {
		}

		const u2 sourceFileIndex;

		virtual void write(BufferWriter& bw) {
			bw.writeu2(sourceFileIndex);
		}
	};

	class ExceptionsAttr: public Attr {
	public:
		ExceptionsAttr(u2 nameIndex, u4 len, const vector<u2>& es) :
				Attr(nameIndex, len), es(es) {
		}

		vector<u2> es;

		virtual void write(BufferWriter& bw) {
			bw.writeu2(es.size());
			for (u4 i = 0; i < es.size(); i++) {
				bw.writeu2(es[i]);
			}
		}

	};

	class Member {
		friend class Members;
		Member(const Member&) = delete;

	public:
		Member(Member&&) = default;

		u2 accessFlags;
		u2 nameIndex;
		u2 descIndex;
		Attrs attrs;
	private:

		inline Member(u2 accessFlags, u2 nameIndex, u2 descIndex) :
				accessFlags(accessFlags), nameIndex(nameIndex), descIndex(
						descIndex) {
		}
	};

	class Members {

		friend class ClassFile;
		Members(const Members&) = delete;

	public:

		inline Member& add(u2 accessFlags, u2 nameIndex, u2 descIndex) {
			Member member(accessFlags, nameIndex, descIndex);
			members.push_back(std::move(member));

			return members.back();
		}

		inline u2 size() const {
			return members.size();
		}

		inline Member& operator[](u2 index) {
			return members[index];
		}

	private:

		inline Members() {
		}

		vector<Member> members;
	};

	u4 magic;
	u2 minor;
	u2 major;
	ConstPool cp;
	u2 accessFlags;
	u2 thisClassIndex;
	u2 superClassIndex;
	vector<u2> interfaces;
	Members fields;
	Members methods;
	Attrs attrs;
};

}

#endif
