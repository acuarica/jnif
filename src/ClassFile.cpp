/*
 * ClassFile.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: luigi
 */
#include "ClassFile.hpp"
#include "Error.hpp"
#include "parser/BufferReader.hpp"
#include "parser/ClassParser.hpp"
#include "parser/CodeAttrParser.hpp"
#include "parser/ExceptionsAttrParser.hpp"
#include "parser/LineNumberTableAttrParser.hpp"
#include "parser/LocalVariableTableAttrParser.hpp"
#include "parser/LocalVariableTypeTableAttrParser.hpp"
#include "parser/StackMapTableAttrParser.hpp"
#include "parser/SourceFileAttrParser.hpp"
#include "parser/SignatureAttrParser.hpp"

namespace jnif {

Method::~Method() {
  Error::trace("Method::~Method");
}

bool Method::isInit() const {
	String name = constPool->getUtf8(nameIndex);
	return hasCode() && name == "<init>";
}

bool Method::isMain() const {
	String name = constPool->getUtf8(nameIndex);
	String desc = constPool->getUtf8(descIndex);

	return hasCode() && name == "main" && isStatic() && isPublic()
			&& desc == "([Ljava/lang/String;)V";
}

const char* Member::getName() const {
	const char* name = constPool->getUtf8(nameIndex);
	return name;
}

const char* Member::getDesc() const {
	const char* desc = constPool->getUtf8(descIndex);
	return desc;
}

InstList& Method::instList() {
	for (Attr* attr : attrs) {
		if (attr->kind == ATTR_CODE) {
			return ((CodeAttr*) attr)->instList;
		}
	}

	Error::raise("ERROR! get inst list");
}

ClassFile::ClassFile(const u1* classFileData, const int classFileLen) :
  version(0, 0), accessFlags(0), thisClassIndex(0), superClassIndex(0), sig(&attrs) {

	BufferReader br(classFileData, classFileLen);
	ClassParser<
    AttrsParser<
      SourceFileAttrParser,
      SignatureAttrParser>,
    AttrsParser<
      CodeAttrParser<
        LineNumberTableAttrParser,
        LocalVariableTableAttrParser,
        LocalVariableTypeTableAttrParser,
        StackMapTableAttrParser>,
      ExceptionsAttrParser,
      SignatureAttrParser>,
    AttrsParser<
      SignatureAttrParser>
    > parser;
	parser.parse(br, *this);
}

ClassFile::~ClassFile() {
  Error::trace("~ClassFile");

	for (Field* field : fields) {
		field->~Field();
	}

	for (Method* method : methods) {
		method->~Method();
	}
}

Field* ClassFile::addField(ConstIndex nameIndex, ConstIndex descIndex,
		u2 accessFlags) {
	Field* field = _arena.create<Field>(accessFlags, nameIndex, descIndex, this);
	fields.push_back(field);
	return field;
}

Method* ClassFile::addMethod(ConstIndex nameIndex, ConstIndex descIndex,
		u2 accessFlags) {
	Method* method = _arena.create<Method>(accessFlags, nameIndex, descIndex, this);
	methods.push_back(method);
	return method;
}

static std::ostream& dotFrame(std::ostream& os, const Frame& frame) {
	os << " LVA: ";
	for (u4 i = 0; i < frame.lva.size(); i++) {
		os << (i == 0 ? "" : ",\n ") << i << ": " << frame.lva[i];
	}

	os << std::endl;

	os << " STACK: ";
	int i = 0;
	for (auto t : frame.stack) {
		os << (i == 0 ? "" : "\n  ") << t;
		i++;
	}
	return os << " ";
}

static void dotCfg(std::ostream& os, const ControlFlowGraph& cfg, int mid) {

	for (BasicBlock* bb : cfg) {
		os << "    m" << mid << bb->name << " [ label = \"<port0> " << bb->name;
		os << " |{ ";
		dotFrame(os, bb->in);
		os << " | ";

		for (auto it = bb->start; it != bb->exit; ++it) {
			Inst* inst = *it;
			os << *inst << std::endl;
			//os << endl;
		}

		os << " | ";
		dotFrame(os, bb->out);
		os << "}\" ]" << std::endl;

	}

	for (BasicBlock* bb : cfg) {
		for (BasicBlock* bbt : *bb) {
			os << "    m" << mid << bb->name << " -> m" << mid << bbt->name
					<< "" << std::endl;
		}
	}
}

void ClassFile::dot(std::ostream& os) const {
	os << "digraph Cfg {" << std::endl;
	os << "  graph [ label=\"Class " << getThisClassName() << "\" ]"
			<< std::endl;
	os << "  node [ shape = \"record\" ]" << std::endl;

	int methodId = 0;
	for (const Method* method : methods) {
		if (method->hasCode() && method->codeAttr()->cfg != NULL) {
			const String& methodName = getUtf8(method->nameIndex);
			const String& methodDesc = getUtf8(method->descIndex);

			os << "  subgraph method" << methodId << "{" << std::endl;
			os << "    graph [bgcolor=gray90, label=\"Method " << methodName
					<< methodDesc << "\"]" << std::endl;
			dotCfg(os, *method->codeAttr()->cfg, methodId);

			os << "  }" << std::endl;

			methodId++;
		}
	}

	os << "}" << std::endl;
}

}
