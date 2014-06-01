/*
 * jnif.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */

#include "jnif.hpp"

#include <stdio.h>
#include <execinfo.h>
#include <unistd.h>

using namespace std;

namespace jnif {

void Error::_backtrace(std::ostream& os) {
	void* array[20];
	size_t size;

	size = backtrace(array, 20);

	char** symbols = backtrace_symbols(array, size);
	for (size_t i = 0; i < size; i++) {
		const char* symbol = symbols[i];
		os << "    " << symbol << endl;
	}

	free(symbols);
}

ConstIndex ConstPool::getIndexOfUtf8(const char* utf8) {
	auto it = utf8s.find(utf8);
	if (it != utf8s.end()) {
		ConstIndex idx = it->second;
		Error::assert(getUtf8(idx) != utf8, "Error on get index of utf8");
		return idx;
	} else {
		return NULLENTRY;
	}

//		ConstPool& cp = *this;
//		for (ConstPool::Iterator it = cp.iterator(); it.hasNext(); it++) {
//			ConstIndex i = *it;
//			//ConstPool::Tag tag = cp.getTag(i);
//
//			//const Entry* entry = &cp.entries[i];
//
//			if (isUtf8(i) && getUtf8(i) == String(utf8)) {
//				return i;
//			}
//		}
//
//		return NULLENTRY;
}

const ConstItem* ConstPool::_getEntry(ConstIndex i) const {
	Error::check(i > NULLENTRY, "Null access to constant pool: index = ", i);
	Error::check(i < entries.size(), "Index out of bounds: index = ", i);

	const ConstItem* entry = &entries[i];

	return entry;
}

const ConstItem* ConstPool::_getEntry(ConstIndex index, u1 tag,
		const char* message) const {
	const ConstItem* entry = _getEntry(index);

	Error::check(entry->tag == tag, "Invalid constant ", message,
			", expected: ", (int) tag, ", actual: ", (int) entry->tag);

	return entry;
}

InstList& Method::instList() {
	for (Attr* attr : attrs) {
		if (attr->kind == ATTR_CODE) {
			return ((CodeAttr*) attr)->instList;
		}
	}

	Error::raise("ERROR! get inst list");
}

void ClassHierarchy::addClass(const ClassFile& classFile) {
	ClassEntry e;
	e.className = classFile.getThisClassName();

	if (classFile.superClassIndex == ConstPool::NULLENTRY) {
		Error::check(e.className == "java/lang/Object",
				"invalid class name for null super class: ", e.className,
				"asdfasf");
		e.superClassName = "0";
	} else {
		e.superClassName = classFile.getClassName(classFile.superClassIndex);
	}

	for (ConstIndex interIndex : classFile.interfaces) {
		const string& interName = classFile.getClassName(interIndex);
		e.interfaces.push_back(interName);
	}

	classes[e.className] = e;
	//classes.push_front(e);
}

const String& ClassHierarchy::getSuperClass(const String& className) const {
	auto it = getEntry(className);
	Error::assert(it != classes.end(), "Class not defined");

	return it->second.superClassName;
}

bool ClassHierarchy::isAssignableFrom(const string& sub,
		const string& sup) const {

	string cls = sub;
	while (cls != "0") {
		if (cls == sup) {
			return true;
		}

		cls = getSuperClass(cls);
	}

	return false;
}

bool ClassHierarchy::isDefined(const String& className) const {
//	const ClassHierarchy::ClassEntry* e = getEntry(className);
//	return e != nullptr;
	auto it = getEntry(className);
	return it != classes.end();
}

std::map<String, ClassHierarchy::ClassEntry>::const_iterator ClassHierarchy::getEntry(
		const String& className) const {

	auto it = classes.find(className);

	return it;

//	if (it != classes.end()) {
//		return &it->second;
//	} else {
//		return nullptr;
//	}
//	for (const ClassHierarchy::ClassEntry& e : *this) {
//		if (e.className == className) {
//			return &e;
//		}
//	}
//
//	return nullptr;
}

string Version::supportedByJdk() const {
	if (Version(45, 3) <= *this && *this < Version(45, 0)) {
		return "1.0.2";
	} else if (Version(45, 0) <= *this && *this <= Version(45, 65535)) {
		return "1.1.*";
	} else {
		u2 k = majorVersion - 44;
		stringstream ss;
		ss << "1." << k;
		return ss.str();
	}
}

std::ostream& operator<<(std::ostream& os, const ClassHierarchy&) {
//	for (const ClassHierarchy::ClassEntry& e : ch) {
//		os << "Class: " << e.className << ", ";
//		os << "Super: " << e.superClassName << ", ";
//		os << "Interfaces: { ";
//		for (const string& interName : e.interfaces) {
//			os << interName << " ";
//		}
//
//		os << " }" << endl;
//	}

	return os;
}

Attrs::~Attrs() {
	for (Attr* attr : attrs) {
		delete attr;
	}
}

CodeAttr::~CodeAttr() {
	if (cfg != nullptr) {
		delete cfg;
	}
}

ClassFile::~ClassFile() {
	for (Field* field : fields) {
		delete field;
	}

	for (Method* method : methods) {
		delete method;
	}
}

Field* ClassFile::addField(ConstIndex nameIndex, ConstIndex descIndex,
		u2 accessFlags) {
	Field* field = new Field(accessFlags, nameIndex, descIndex, this);
	fields.push_back(field);
	return field;
}

Method* ClassFile::addMethod(ConstIndex nameIndex, ConstIndex descIndex,
		u2 accessFlags) {
	Method* method = new Method(accessFlags, nameIndex, descIndex, this);
	methods.push_back(method);
	return method;
}

static ostream& dotFrame(ostream& os, const Frame& frame) {
	os << " LVA: ";
	for (u4 i = 0; i < frame.lva.size(); i++) {
		os << (i == 0 ? "" : ", ") << i << ": " << frame.lva[i];
	}

	os << " STACK: ";
	int i = 0;
	for (auto t : frame.stack) {
		os << (i == 0 ? "" : "  ") << t;
		i++;
	}
	return os << " ";
}

static void dotCfg(ostream& os, const ControlFlowGraph& cfg, int methodId) {

	for (BasicBlock* bb : cfg) {
		os << "    m" << methodId << bb->name << " [ label = \"<port0> "
				<< bb->name;
		os << " | ";
		dotFrame(os, bb->in);
		os << " | ";
		dotFrame(os, bb->out);
		os << "\" ]" << endl;

//		for (auto it = bb->start; it != bb->exit; it++) {
//			Inst* inst = *it;
//			printInst(*inst);
//			os << endl;
//		}
	}

	for (BasicBlock* bb : cfg) {
		for (BasicBlock* bbt : *bb) {
			os << "    m" << methodId << bb->name << " -> m" << methodId
					<< bbt->name << "" << endl;
		}
	}
}

void ClassFile::dot(ostream& os) const {
	os << "digraph Cfg {" << endl;
	os << "  graph [ label=\"Class " << getThisClassName() << "\" ]" << endl;
	os << "  node [ shape = \"record\" ]" << endl;

	int methodId = 0;
	for (const Method* method : methods) {
		if (method->hasCode() && method->codeAttr()->cfg != nullptr) {
			const string& methodName = getUtf8(method->nameIndex);
			const string& methodDesc = getUtf8(method->descIndex);

			os << "  subgraph method" << methodId << "{" << endl;
			os << "    graph [bgcolor=gray90, label=\"Method " << methodName
					<< methodDesc << "\"]" << endl;
			dotCfg(os, *method->codeAttr()->cfg, methodId);

			os << "  }" << endl;

			methodId++;
		}
	}

	os << "}" << endl;
}

/**
 * Type implementation
 */

Type Type::objectType(const String& className, u2 cpindex) {
	Error::check(!className.empty(),
			"Expected non-empty class name for object type");

	return Type(TYPE_OBJECT, className, cpindex);
}

Type Type::arrayType(const Type& baseType, u4 dims) {
	//u4 d = baseType.dims + dims;
	Error::check(dims > 0, "Invalid dims: ", dims);
	Error::check(dims <= 255, "Invalid dims: ", dims);
	Error::check(!baseType.isTop(), "Cannot construct an array type of ", dims,
			" dimension(s) using as a base type Top (", baseType, ")");
//		Error::check(!baseType.isArray(), "base type is already an array: ",
//				baseType);

	return Type(baseType, dims);
}

String Type::getClassName() const {
	Error::check(isObject(), "Type is not object type to get class name: ",
			*this);

	if (isArray()) {
		stringstream ss;
		for (u4 i = 0; i < dims; i++) {
			ss << "[";
		}

		if (tag == TYPE_OBJECT) {
			ss << "L" << className << ";";
		} else {
			ss << className;
		}

		return ss.str();
	} else {
		return className;
	}
}

u2 Type::getCpIndex() const {
	Error::check(isObject(), "Type is not object type to get cp index: ",
			*this);
	return classIndex;
}

Type Type::elementType() const {
	Error::check(isArray(), "Type is not array: ", *this);

	return Type(*this, dims - 1);
}

Type Type::stripArrayType() const {
	Error::check(isArray(), "Type is not array: ", *this);

	return Type(*this, 0);
}

Type Type::fromConstClass(const String& className) {
	Error::assert(!className.empty(), "Invalid string class");

	if (className[0] == '[') {
		const char* classNamePtr = className.c_str();
		Type arrayType = fromFieldDesc(classNamePtr);
		Error::assert(arrayType.isArray(), "Not an array: ", arrayType);
		return arrayType;
	} else {
		return Type::objectType(className);
	}
}

Type Type::fromFieldDesc(const char*& fieldDesc) {
	const char* originalFieldDesc = fieldDesc;

	int dims = 0;
	while (*fieldDesc == '[') {
		Error::check(*fieldDesc != '\0',
				"Reach end of string while searching for array. Field descriptor: ",
				originalFieldDesc);
		fieldDesc++;
		dims++;
	}

	Error::check(*fieldDesc != '\0', "");

	auto parseBaseType = [&] () -> Type {
		switch (*fieldDesc) {
			case 'Z':
			return Type::booleanType();
			case 'B':
			return Type::byteType();
			case 'C':
			return Type::charType();
			case 'S':
			return Type::shortType();
			case 'I':
			return Type::intType();
			case 'D':
			return Type::doubleType();
			case 'F':
			return Type::floatType();
			case 'J':
			return Type::longType();
			case 'L': {
				fieldDesc++;

				const char* classNameStart = fieldDesc;
				int len = 0;
				while (*fieldDesc != ';') {
					Error::check(*fieldDesc != '\0', "");
					fieldDesc++;
					len++;
				}

				string className (classNameStart, len);
				return Type::objectType(className);
			}
			default:
			Error::raise("Invalid field desc ", originalFieldDesc);
		}};

	Type t = [&]() {
		Type baseType = parseBaseType();
		if (dims == 0) {
			return baseType;
		} else {
			return Type::arrayType(baseType, dims);
		}
	}();

	fieldDesc++;

	return t;
}

Type Type::fromMethodDesc(const char* methodDesc, vector<Type>* argsType) {
	const char* originalMethodDesc = methodDesc;

	Error::check(*methodDesc == '(', "Invalid beginning of method descriptor: ",
			originalMethodDesc);
	methodDesc++;

	while (*methodDesc != ')') {
		Error::check(*methodDesc != '\0', "Reached end of string: ",
				originalMethodDesc);

		Type t = fromFieldDesc(methodDesc);
		argsType->push_back(t);
	}

	Error::check(*methodDesc == ')', "Expected ')' in method descriptor: ",
			originalMethodDesc);
	methodDesc++;

	Error::check(*methodDesc != '\0', "Reached end of string: ",
			originalMethodDesc);

	Type returnType = [&]() {
		if (*methodDesc == 'V') {
			methodDesc++;
			return Type::voidType();
		} else {
			return fromFieldDesc(methodDesc);
		}
	}();

	Error::check(*methodDesc == '\0', "Expected end of string: %s",
			originalMethodDesc);

	return returnType;
}

Type Type::_topType(TYPE_TOP);
Type Type::_intType(TYPE_INTEGER, "I");
Type Type::_floatType(TYPE_FLOAT, "F");
Type Type::_longType(TYPE_LONG, "J");
Type Type::_doubleType(TYPE_DOUBLE, "D");
Type Type::_booleanType(TYPE_BOOLEAN, "Z");
Type Type::_byteType(TYPE_BYTE, "B");
Type Type::_charType(TYPE_CHAR, "C");
Type Type::_shortType(TYPE_SHORT, "S");
Type Type::_nullType(TYPE_NULL);
Type Type::_uninitThisType(TYPE_UNINITTHIS);

long Type::nextTypeId = 2;

/**
 * Frame implementation
 */
Type Frame::pop() {
	Error::check(stack.size() > 0, "Trying to pop in an empty stack.");

	Type t = stack.front();
	stack.pop_front();
	return t;
}

Type Frame::popOneWord() {
	Type t = pop();
	Error::check(t.isOneWord() || t.isTop(), "Type is not one word type: ", t,
			", frame: ", *this);
	return t;
}

Type Frame::popTwoWord() {
	Type t1 = pop();
	Type t2 = pop();

	Error::check(
			(t1.isOneWord() && t2.isOneWord())
					|| (t1.isTwoWord() && t2.isTop()),
			"Invalid types on top of the stack for pop2: ", t1, t2, *this);
	//Error::check(t2.isTop(), "Type is not Top type: ", t2, t1, *this);

	return t1;
}

Type Frame::popInt() {
	Type t = popOneWord();
	Error::assert(t.isInt(), "invalid int type on top of the stack: ", t);
	return t;
}

Type Frame::popFloat() {
	Type t = popOneWord();
	Error::assert(t.isFloat(), "invalid float type on top of the stack");
	return t;
}

Type Frame::popLong() {
	Type t = popTwoWord();
	Error::check(t.isLong(), "invalid long type on top of the stack");
	return t;
}

Type Frame::popDouble() {
	Type t = popTwoWord();
	Error::check(t.isDouble(), "Invalid double type on top of the stack: ", t);

	return t;
}

void Frame::popType(const Type& type) {
	if (type.isInt()) {
		popInt();
	} else if (type.isFloat()) {
		popFloat();
	} else if (type.isLong()) {
		popLong();
	} else if (type.isDouble()) {
		popDouble();
	} else if (type.isObject()) {
		popRef();
	} else {
		Error::raise("invalid pop type: ", type);
	}
}

void Frame::pushType(const Type& type) {
	if (type.isInt()) {
		pushInt();
	} else if (type.isFloat()) {
		pushFloat();
	} else if (type.isLong()) {
		pushLong();
	} else if (type.isDouble()) {
		pushDouble();
	} else if (type.isNull()) {
		pushNull();
	} else if (type.isObject()) {
		push(type);
	} else {
		Error::raise("invalid push type: ", type);
	}
}

void Frame::setVar(u4* lvindex, const Type& t) {
	Error::assert(t.isOneOrTwoWord(), "Setting var on non one-two word ");

	if (t.isOneWord()) {
		_setVar(*lvindex, t);
		(*lvindex)++;
	} else {
		_setVar(*lvindex, t);
		_setVar(*lvindex + 1, Type::topType());
		(*lvindex) += 2;
	}
}

void Frame::setVar2(u4 lvindex, const Type& t) {
	setVar(&lvindex, t);
}

void Frame::setRefVar(u4 lvindex, const Type& type) {
	Error::check(type.isObject() || type.isNull(), "Type must be object type: ",
			type);
	setVar(&lvindex, type);
}

void Frame::cleanTops() {
	for (u4 i = 0; i < lva.size(); i++) {
		Type t = lva[i];
		if (t.isTwoWord()) {
			Type top = lva[i + 1];
			Error::assert(top.isTop(), "Not top for two word: ", top);
			lva.erase(lva.begin() + i + 1);
		}
	}

	for (int i = lva.size() - 1; i >= 0; i--) {
		Type t = lva[i];
		if (t.isTop()) {

			lva.erase(lva.begin() + i);
		} else {
			return;
		}
	}
}

void Frame::_setVar(u4 lvindex, const Type& t) {
	Error::check(lvindex < 256, "");

	if (lvindex >= lva.size()) {
		lva.resize(lvindex + 1, Type::topType());
	}

	lva[lvindex] = t;
}

void Inst::checkCast(bool cond, const char* kindName) const {
	//Error::assert(cond, "Inst is not a ", kindName, ": ", *this);
	Error::assert(cond, "Inst is not a ", kindName,
			": <missing instruction due to const>");
}

Inst* InstList::Iterator::operator*() {
	Error::assert(position != nullptr, "Dereferencing * on nullptr");
	return position;
}

Inst* InstList::Iterator::operator->() const {
	Error::assert(position != nullptr, "Dereferencing -> on nullptr");

	return position;
}

InstList::Iterator& InstList::Iterator::operator++() {
	Error::assert(position != nullptr, "Doing ++ at nullptr");
	position = position->next;

	return *this;
}

InstList::Iterator& InstList::Iterator::operator--() {
	if (position == nullptr) {
		position = last;
	} else {
		position = position->prev;
	}

	Error::assert(position != nullptr, "Doing -- at nullptr after last");

	return *this;
}

InstList::~InstList() {
	for (Inst* inst = first; inst != nullptr;) {
		Inst* next = inst->next;
		delete inst;
		inst = next;
	}
}

void InstList::addInst(Inst* inst, Inst* pos) {
	Error::assert((first == nullptr) == (last == nullptr),
			"Invalid head/tail/size: head: ", first, ", tail: ", last,
			", size: ", size);

	Error::assert((first == nullptr) == (size == 0),
			"Invalid head/tail/size: head: ", first, ", tail: ", last,
			", size: ", size);

	Inst* p;
	Inst* n;
	if (first == nullptr) {
		Error::assert(pos == nullptr, "Invalid pos");

		p = nullptr;
		n = nullptr;
		first = inst;
		last = inst;
	} else {
		if (pos == nullptr) {
			p = last;
			n = nullptr;
			last = inst;
		} else {
			p = pos->prev;
			n = pos;
		}
	}

	inst->prev = p;
	inst->next = n;

	if (inst->prev != nullptr) {
		inst->prev->next = inst;
	}

	if (inst->next != nullptr) {
		inst->next->prev = inst;
	}

	size++;
}

void BasicBlock::addTarget(BasicBlock* target) {
	Error::check(cfg == target->cfg, "invalid owner for basic block");

	targets.push_back(target);
}

//static void setBranchTargets(InstList& instList) {
//	for (Inst* inst : instList) {
//		if (inst->isJump()) {
//			Error::assert(inst->jump()->label2->label()->isBranchTarget, "jmp");
//			//inst->jump()->label2->label()->isBranchTarget = true;
//		} else if (inst->isTableSwitch()) {
//			Error::assert(inst->ts()->def->label()->isBranchTarget, "ts");
//			//inst->ts()->def->label()->isBranchTarget = true;
//			for (Inst* target : inst->ts()->targets) {
//				Error::assert(target->label()->isBranchTarget, "ts:target");
//				//target->label()->isBranchTarget = true;
//			}
//		} else if (inst->isLookupSwitch()) {
//			Error::assert(inst->ls()->defbyte->label()->isBranchTarget, "ls");
//			//inst->ls()->defbyte->label()->isBranchTarget = true;
//			for (Inst* target : inst->ls()->targets) {
//				Error::assert(target->label()->isBranchTarget, "ls:target");
//				//target->label()->isBranchTarget = true;
//			}
//		}
//	}
//}

static void buildBasicBlocks(InstList& instList, ControlFlowGraph& cfg) {
	//setBranchTargets(instList);

	int bbid = 0;
	auto beginBb = instList.begin();

	auto getBasicBlockName = [&](int bbid) {
		stringstream ss;
		ss << "BB" << bbid;

		return ss.str();
	};

	auto addBasicBlock2 = [&](InstList::Iterator eit) {
		if (beginBb != eit) {
			string name = getBasicBlockName(bbid);
			cfg.addBasicBlock(beginBb, eit, name);

			beginBb = eit;
			bbid++;
		}
	};

	for (auto it = instList.begin(); it != instList.end(); ++it) {
		Inst* inst = *it;

		if (inst->isLabel()
				&& (inst->label()->isBranchTarget || inst->label()->isTryStart)) {
			addBasicBlock2(it);
		}

		if (inst->isBranch()) {
			auto eit = it;
			++eit;
			addBasicBlock2(eit);
		}

		if (inst->isExit()) {
			auto eit = it;
			++eit;
			addBasicBlock2(eit);
		}
	}
}

static void buildCfg(InstList& instList, ControlFlowGraph& cfg) {
	buildBasicBlocks(instList, cfg);

	auto addTarget2 = [&] (BasicBlock* bb, Inst* inst) {
		Error::assert(inst->isLabel(), "Expected label instruction");
		int labelId = inst->label()->id;
		BasicBlock* tbbid = cfg.findBasicBlockOfLabel(labelId);
		bb->addTarget(tbbid);
	};

	for (BasicBlock* bb : cfg) {
		if (bb->start == instList.end()) {
			Error::assert(bb->name == "Entry" || bb->name == "Exit", "");
			Error::assert(bb->exit == instList.end(), "");
			continue;
		}

		InstList::Iterator e = bb->exit;
		--e;
		Error::assert(e != instList.end(), "");

		Inst* last = *e;

		if (bb->start == instList.begin()) {
			cfg.entry->addTarget(bb);
		}

		if (last->isJump()) {
			addTarget2(bb, last->jump()->label2);

			if (last->opcode != OPCODE_goto) {
				Error::assert(bb->next != nullptr, "next bb is null");
				bb->addTarget(bb->next);
			}
		} else if (last->isTableSwitch()) {
			addTarget2(bb, last->ts()->def);

			for (Inst* target : last->ts()->targets) {
				addTarget2(bb, target);
			}
		} else if (last->isLookupSwitch()) {
			addTarget2(bb, last->ls()->defbyte);

			for (Inst* target : last->ls()->targets) {
				addTarget2(bb, target);
			}
		} else if (last->isExit()) {
			bb->addTarget(cfg.exit);
		} else {
			Error::assert(bb->next != nullptr, "next bb is null");
			bb->addTarget(bb->next);
		}
	}
}

ControlFlowGraph::ControlFlowGraph(InstList& instList) :
		entry(addConstBb(instList, "Entry")), exit(
				addConstBb(instList, "Exit")), instList(instList) {
	buildCfg(instList, *this);
}

ControlFlowGraph::~ControlFlowGraph() {
	//cerr << "~cfg";

	for (auto bb : *this) {
		delete bb;
	}
}

BasicBlock* ControlFlowGraph::addBasicBlock(InstList::Iterator start,
		InstList::Iterator end, const String& name) {
	BasicBlock * const bb = new BasicBlock(start, end, name, this);

	if (basicBlocks.size() > 0) {
		BasicBlock* prevbb = basicBlocks.back();
		prevbb->next = bb;
	}

	basicBlocks.push_back(bb);

	return bb;
}

BasicBlock* ControlFlowGraph::findBasicBlockOfLabel(int labelId) const {
	for (BasicBlock* bb : *this) {
		if (bb->start == instList.end()) {
			Error::assert(bb->name == "Entry" || bb->name == "Exit", "");
			Error::assert(bb->exit == instList.end(), "");
			continue;
		}

		Inst* inst = *bb->start;
		if (inst->isLabel() && inst->label()->id == labelId) {
			return bb;
		}
	}

	//InstList& il = (InstList&) instList;

	Error::raise("Invalid label id: ", labelId, " for the instruction list: ",
			", in cfg: ", *this, instList);
}

}
