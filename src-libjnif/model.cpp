//
// Created by Luigi on 10.10.17.
//

#include "jnif.hpp"

#include "analysis/SmtBuilder.hpp"
#include "analysis/ComputeFrames.hpp"
#include "analysis/FrameGenerator.hpp"

namespace jnif::model {


    u4 ConstPool::size() const {
        return entries.size();
    }

    ConstPool::Iterator ConstPool::iterator() const {
        return Iterator(*this, 1);
    }

    ConstPool::Index ConstPool::addClass(ConstPool::Index classNameIndex) {
        return _addSingle(Class({classNameIndex}));
    }

    ConstPool::Index ConstPool::addClass(const char* className) {
        Index classNameIndex = putUtf8(className);
        return addClass(classNameIndex);
    }

    ConstPool::Index ConstPool::addFieldRef(
            ConstPool::Index classIndex,
            ConstPool::Index nameAndTypeIndex) {
        return _addSingle(FIELDREF, MemberRef({classIndex, nameAndTypeIndex}));
    }

    ConstPool::Index ConstPool::addMethodRef(
            ConstPool::Index classIndex,
            ConstPool::Index nameAndTypeIndex) {
        return _addSingle(METHODREF, MemberRef({classIndex, nameAndTypeIndex}));
    }

    ConstPool::Index ConstPool::addMethodRef(
            ConstPool::Index classIndex,
            const char* name,
            const char* desc) {
        Index methodNameIndex = addUtf8(name);
        Index methodDescIndex = addUtf8(desc);
        Index nameAndTypeIndex = addNameAndType(methodNameIndex, methodDescIndex);
        return addMethodRef(classIndex, nameAndTypeIndex);
    }

    ConstPool::Index ConstPool::addInterMethodRef(
            ConstPool::Index classIndex,
            ConstPool::Index nameAndTypeIndex) {
        return _addSingle(INTERMETHODREF, MemberRef({classIndex, nameAndTypeIndex}));
    }

    ConstPool::Index ConstPool::addString(ConstPool::Index utf8Index) {
        return _addSingle(String({utf8Index}));
    }

    ConstPool::Index ConstPool::addString(const std::string& str) {
        Index utf8Index = addUtf8(str.c_str());
        return addString(utf8Index);
    }

    ConstPool::Index ConstPool::addStringFromClass(ConstPool::Index classIndex) {
        Index classNameIndex = getClassNameIndex(classIndex);
        Index classNameStringIndex = addString(classNameIndex);

        return classNameStringIndex;
    }

    ConstPool::Index ConstPool::addInteger(int value) {
        return _addSingle(Integer({value}));
    }

    ConstPool::Index ConstPool::addFloat(float value) {
        return _addSingle(Float({value}));
    }

    ConstPool::Index ConstPool::addLong(long value) {
        return _addDoubleEntry(Long({value}));
    }

    ConstPool::Index ConstPool::addDouble(double value) {
        return _addDoubleEntry(Double({value}));
    }

    ConstPool::Index ConstPool::addNameAndType(ConstPool::Index nameIndex,
                                               ConstPool::Index descIndex) {
        return _addSingle(NameAndType({nameIndex, descIndex}));
    }

    ConstPool::Index ConstPool::addUtf8(const char* utf8, int len) {
        std::string str(utf8, len);
        Index i = _addSingle(str);
        utf8s[str] = i;

        return i;
    }

    ConstPool::Index ConstPool::addUtf8(const char* str) {
        Index i = _addSingle(str);
        utf8s[std::string(str)] = i;

        return i;
    }

    ConstPool::Index ConstPool::addMethodHandle(u1 refKind, u2 refIndex) {
        return _addSingle(MethodHandle({refKind, refIndex}));
    }

    ConstPool::Index ConstPool::addMethodType(u2 descIndex) {
        return _addSingle(MethodType({descIndex}));
    }

    ConstPool::Index ConstPool::addInvokeDynamic(u2 bootstrapMethodAttrIndex,
                                                 u2 nameAndType) {
        return _addSingle(InvokeDynamic({bootstrapMethodAttrIndex, nameAndType}));
    }

    template<class... TArgs>
    ConstPool::Index ConstPool::_addSingle(TArgs... args) {
        int index = entries.size();

        JnifError::check(index < (1 << 16), "CP limit reach: index=", index);
        entries.emplace_back(args...);

        return (Index) index;
    }

    template<class... TArgs>
    ConstPool::Index ConstPool::_addDoubleEntry(TArgs... args) {
        Index index = entries.size();
        entries.emplace_back(args...);

        entries.emplace_back();

        return index;
    }

    ConstPool::Index ConstPool::getIndexOfUtf8(const char* utf8) {
        auto it = utf8s.find(utf8);
        if (it != utf8s.end()) {
            Index idx = it->second;
            JnifError::assert(getUtf8(idx) != utf8, "Error on get index of utf8");
            return idx;
        } else {
            return NULLENTRY;
        }
    }

    ConstPool::Index ConstPool::getIndexOfClass(const char* className) {
        auto it = classes.find(className);
        if (it != utf8s.end()) {
            ConstPool::Index idx = it->second;
            // JnifError::assert(getUtf8(idx) != utf8, "Error on get index of utf8");
            return idx;
        } else {
            return NULLENTRY;
        }
    }

    const ConstPool::Item* ConstPool::_getEntry(ConstPool::Index i) const {
        JnifError::check(i > NULLENTRY, "Null access to constant pool: index=", i);
        JnifError::check(i < entries.size(), "Index out of bounds: index=", i);

        const Item* entry = &entries[i];
        return entry;
    }

    const ConstPool::Item* ConstPool::_getEntry(
            ConstPool::Index index, u1 tag, const char* message) const {
        const Item* entry = _getEntry(index);
        JnifError::check(entry->tag == tag, "Invalid constant ", message,
                         ", expected: ", (int) tag, ", actual: ", (int) entry->tag);

        return entry;
    }


    Version::Version(u2 majorVersion, u2 minorVersion) : _major(majorVersion), _minor(minorVersion) {
    }

    u2 Version::majorVersion() const {
        return _major;
    }

    u2 Version::minorVersion() const {
        return _minor;
    }

    std::string Version::supportedByJdk() const {
        if (Version(45, 3) <= *this && *this < Version(45, 0)) {
            return "1.0.2";
        } else if (Version(45, 0) <= *this && *this <= Version(45, 65535)) {
            return "1.1.*";
        } else {
            u2 k = _major - 44;
            std::stringstream ss;
            ss << "1." << k;
            return ss.str();
        }
    }

    bool operator==(const Version& lhs, const Version& rhs) {
        return lhs._major == rhs._major && lhs._major == rhs._major;
    }

    bool operator<(const Version& lhs, const Version& rhs) {
        return lhs._major < rhs._major
               || (lhs._major == rhs._major && lhs._minor < rhs._minor);
    }

    bool operator<=(const Version& lhs, const Version& rha) {
        return lhs < rha || lhs == rha;
    }


    Member::Member(u2 accessFlags, ConstPool::Index nameIndex, ConstPool::Index descIndex, const ConstPool& constPool) :
            accessFlags(accessFlags),
            nameIndex(nameIndex),
            descIndex(descIndex),
            constPool(constPool),
            sig(&attrs) {
        JnifError::check(constPool.isUtf8(nameIndex));
        JnifError::check(constPool.isUtf8(descIndex));
    }

    bool Method::isInit() const {
        string name = constPool.getUtf8(nameIndex);
        return hasCode() && name == "<init>";
    }

    bool Method::isMain() const {
        string name = constPool.getUtf8(nameIndex);
        string desc = constPool.getUtf8(descIndex);

        return hasCode() && name == "main" && isStatic() && isPublic()
               && desc == "([Ljava/lang/String;)V";
    }

    const char* Member::getName() const {
        return constPool.getUtf8(nameIndex);
    }

    const char* Member::getDesc() const {
        return constPool.getUtf8(descIndex);
    }

    InstList& Method::instList() {
        for (Attr* attr : attrs) {
            if (attr->kind == ATTR_CODE) {
                return ((CodeAttr*) attr)->instList;
            }
        }

        JnifError::raise("ERROR! get inst list");
    }

    ClassFile::ClassFile() : sig(&attrs) {
    }

    ClassFile::ClassFile(const char* className) : thisClassIndex(addClass(className)), sig(&attrs) {
    }

    ClassFile::ClassFile(const char* className, const char* superClassName, u2 accessFlags, const Version& version)
            : thisClassIndex(addClass(className)), superClassIndex(addClass(superClassName)), accessFlags(accessFlags),
              version(version), sig(&attrs) {
    }

    Field& ClassFile::addField(ConstPool::Index nameIndex, ConstPool::Index descIndex, u2 accessFlags) {
        fields.emplace_back(accessFlags, nameIndex, descIndex, *this);
        return fields.back();
    }

    Method& ClassFile::addMethod(ConstPool::Index nameIndex, ConstPool::Index descIndex, u2 accessFlags) {
        methods.emplace_back(accessFlags, nameIndex, descIndex, *this);
        return methods.back();
    }

    std::list<Method>::iterator ClassFile::getMethod(const char* methodName) {
        return std::find_if(
                methods.begin(),
                methods.end(),
                [&](const Method& m) { return m.getName() == std::string(methodName); }
        );
    }

    void ClassFile::computeFrames(IClassPath* classPath) {
        computeSize();

        FrameGenerator fg(*this, classPath);

        for (Method& method : methods) {
            CodeAttr* code = method.codeAttr();

            if (code != NULL) {
                bool hasJsrOrRet = code->instList.hasJsrOrRet();
                if (hasJsrOrRet) {
                    return;
                }

                fg.computeFrames(code, &method);
            }
        }
    }

    static std::ostream& dotFrame(std::ostream& os, const Frame& frame) {
        os << " LVA: ";
        for (u4 i = 0; i < frame.lva.size(); i++) {
            os << (i == 0 ? "" : ",\n ") << i << ": " << frame.lva[i].first;
        }

        os << std::endl;

        os << " STACK: ";
        int i = 0;
        for (auto t : frame.stack) {
            os << (i == 0 ? "" : "\n  ") << t.first;
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
        for (const Method& method : methods) {
            if (method.hasCode() && method.codeAttr()->cfg != NULL) {
                const std::string& methodName = getUtf8(method.nameIndex);
                const std::string& methodDesc = getUtf8(method.descIndex);

                os << "  subgraph method" << methodId << "{" << std::endl;
                os << "    graph [bgcolor=gray90, label=\"Method " << methodName
                   << methodDesc << "\"]" << std::endl;
                dotCfg(os, *method.codeAttr()->cfg, methodId);

                os << "  }" << std::endl;

                methodId++;
            }
        }

        os << "}" << std::endl;
    }


    void Inst::checkCast(bool cond, const char* kindName) const {
        JnifError::assert(cond, "Inst is not a ", kindName, ": ", *this);
    }


    Inst* InstList::Iterator::operator*() {
        JnifError::assert(position != NULL, "Dereferencing * on NULL");
        return position;
    }

    Inst* InstList::Iterator::operator->() const {
        JnifError::assert(position != NULL, "Dereferencing -> on NULL");

        return position;
    }

    InstList::Iterator& InstList::Iterator::operator++() {
        JnifError::assert(position != NULL, "Doing ++ at NULL");
        position = position->next;

        return *this;
    }

    InstList::Iterator& InstList::Iterator::operator--() {
        if (position == NULL) {
            position = last;
        } else {
            position = position->prev;
        }

        JnifError::assert(position != NULL, "Doing -- at NULL after last");

        return *this;
    }

    InstList::~InstList() {
        JnifError::trace("~InstList");

        for (Inst* inst = first; inst != NULL;) {
            Inst* next = inst->next;
            inst->~Inst();
            inst = next;
        }
    }

    LabelInst* InstList::createLabel() {
        LabelInst* inst = _create<LabelInst>(constPool, nextLabelId);

        nextLabelId++;
        return inst;
    }

    void InstList::addLabel(LabelInst* inst, Inst* pos) {
        addInst(inst, pos);
    }

    LabelInst* InstList::addLabel(Inst* pos) {
        LabelInst* inst = createLabel();
        addInst(inst, pos);

        return inst;
    }

    ZeroInst* InstList::addZero(Opcode opcode, Inst* pos) {
        ZeroInst* inst = _create<ZeroInst>(opcode, constPool);

        addInst(inst, pos);

        return inst;
    }

    PushInst* InstList::addBiPush(u1 value, Inst* pos) {
        PushInst* inst = _create<PushInst>(OPCODE_bipush, KIND_BIPUSH, value,
                                           constPool);
        addInst(inst, pos);

        return inst;
    }

    PushInst* InstList::addSiPush(u2 value, Inst* pos) {
        PushInst* inst = _create<PushInst>(OPCODE_sipush, KIND_SIPUSH, value,
                                           constPool);
        addInst(inst, pos);

        return inst;
    }

    LdcInst* InstList::addLdc(Opcode opcode, ConstPool::Index valueIndex, Inst* pos) {
        LdcInst* inst = _create<LdcInst>(opcode, valueIndex, constPool);
        addInst(inst, pos);

        return inst;
    }

    VarInst* InstList::addVar(Opcode opcode, u1 lvindex, Inst* pos) {
        VarInst* inst = _create<VarInst>(opcode, lvindex, constPool);
        addInst(inst, pos);

        if (opcode == OPCODE_ret) {
            jsrOrRet = true;
        }

        return inst;
    }

    IincInst* InstList::addIinc(u1 index, u1 value, Inst* pos) {
        IincInst* inst = _create<IincInst>(index, value, constPool);
        addInst(inst, pos);

        return inst;
    }

    WideInst* InstList::addWideVar(Opcode varOpcode, u2 lvindex, Inst* pos) {
        WideInst* inst = _create<WideInst>(varOpcode, lvindex, constPool);
        addInst(inst, pos);

        return inst;
    }

    WideInst* InstList::addWideIinc(u2 index, u2 value, Inst* pos) {
        WideInst* inst = _create<WideInst>(index, value, constPool);
        addInst(inst, pos);

        return inst;
    }

    JumpInst* InstList::addJump(Opcode opcode, LabelInst* targetLabel, Inst* pos) {
        JumpInst* inst = _create<JumpInst>(opcode, targetLabel, constPool);
        addInst(inst, pos);
        branchesCount++;

        targetLabel->isBranchTarget = true;

        if (opcode == OPCODE_jsr) {
            jsrOrRet = true;
        }

        return inst;
    }

    FieldInst* InstList::addField(Opcode opcode, ConstPool::Index fieldRefIndex, Inst* pos) {
        FieldInst* inst = _create<FieldInst>(opcode, fieldRefIndex, constPool);
        addInst(inst, pos);
        return inst;
    }

    InvokeInst* InstList::addInvoke(Opcode opcode, ConstPool::Index methodRefIndex, Inst* pos) {
        InvokeInst* inst = _create<InvokeInst>(opcode, methodRefIndex, constPool);
        addInst(inst, pos);
        return inst;
    }

    InvokeInterfaceInst* InstList::addInvokeInterface(ConstPool::Index interMethodRefIndex, u1 count, Inst* pos) {
        InvokeInterfaceInst* inst = _create<InvokeInterfaceInst>(interMethodRefIndex, count, constPool);
        addInst(inst, pos);
        return inst;
    }

    InvokeDynamicInst* InstList::addInvokeDynamic(ConstPool::Index callSite, Inst* pos) {
        InvokeDynamicInst* inst = _create<InvokeDynamicInst>(callSite, constPool);
        addInst(inst, pos);
        return inst;
    }

    TypeInst* InstList::addType(Opcode opcode, ConstPool::Index classIndex, Inst* pos) {
        TypeInst* inst = _create<TypeInst>(opcode, classIndex, constPool);
        addInst(inst, pos);
        return inst;
    }

    NewArrayInst* InstList::addNewArray(u1 atype, Inst* pos) {
        NewArrayInst* inst = _create<NewArrayInst>(OPCODE_newarray, atype, constPool);
        addInst(inst, pos);
        return inst;
    }

    MultiArrayInst* InstList::addMultiArray(ConstPool::Index classIndex, u1 dims, Inst* pos) {
        MultiArrayInst* inst = _create<MultiArrayInst>(OPCODE_multianewarray, classIndex, dims, constPool);
        addInst(inst, pos);
        return inst;
    }

    TableSwitchInst* InstList::addTableSwitch(LabelInst* def, int low, int high, Inst* pos) {
        TableSwitchInst* inst = _create<TableSwitchInst>(def, low, high, constPool);
        addInst(inst, pos);
        branchesCount++;

        def->isBranchTarget = true;

        return inst;
    }

    LookupSwitchInst* InstList::addLookupSwitch(LabelInst* def, u4 npairs, Inst* pos) {
        LookupSwitchInst* inst = _create<LookupSwitchInst>(def, npairs, constPool);
        addInst(inst, pos);
        branchesCount++;

        def->isBranchTarget = true;

        return inst;
    }

    Inst* InstList::getInst(int offset) {
        for (Inst* inst : *this) {
            if (inst->_offset == offset && !inst->isLabel()) {
                return inst;
            }
        }

        return nullptr;
    }

    template<typename TInst, typename ... TArgs>
    TInst* InstList::_create(const TArgs& ... args) {
        return constPool->_arena.create<TInst>(args ...);
    }

    void InstList::addInst(Inst* inst, Inst* pos) {
        JnifError::assert((first == NULL) == (last == NULL),
                          "Invalid head/tail/size: head: ", first, ", tail: ", last,
                          ", size: ", _size);

        JnifError::assert((first == NULL) == (_size == 0),
                          "Invalid head/tail/size: head: ", first, ", tail: ", last,
                          ", size: ", _size);

        Inst* p;
        Inst* n;
        if (first == NULL) {
            JnifError::assert(pos == NULL, "Invalid pos");

            p = NULL;
            n = NULL;
            //first = inst;
            //last = inst;
        } else {
            if (pos == NULL) {
                p = last;
                n = NULL;
                //last = inst;
            } else {
                p = pos->prev;
                n = pos;
            }
        }

        inst->prev = p;
        inst->next = n;

        if (inst->prev != NULL) {
            inst->prev->next = inst;
        } else {
            first = inst;
        }

        if (inst->next != NULL) {
            inst->next->prev = inst;
        } else {
            last = inst;
        }

        _size++;
    }



    u2 Type::getCpIndex() const {
        JnifError::check(isObject(), "Type is not object type to get cp index: ",
                         *this);
        return classIndex;
    }

    string Type::getClassName() const {
        JnifError::check(isObject(), "Type is not object type to get class name: ",
                         *this);
        if (isArray()) {
            std::stringstream ss;
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

    Type Type::elementType() const {
        JnifError::check(isArray(), "Type is not array: ", *this);

        Type type = Type(*this, dims - 1);
        return type;
    }

    Type Type::stripArrayType() const {
        JnifError::check(isArray(), "Type is not array: ", *this);

        Type type = Type(*this, 0);
        return type;
    }

    long Type::nextTypeId = 2;

    Type TypeFactory::uninitThisType() {
        return Type(TYPE_UNINITTHIS);
    }

    Type TypeFactory::uninitType(short offset, class Inst* label) {
        return Type(TYPE_UNINIT, offset, label);
    }

    Type TypeFactory::objectType(const string& className, u2 cpindex) {
        JnifError::check(
                !className.empty(),
                "Expected non-empty class name for object type");

        return Type(TYPE_OBJECT, className, cpindex);
    }

    Type TypeFactory::arrayType(const Type& baseType, u4 dims) {
        //u4 d = baseType.dims + dims;
        JnifError::check(dims > 0, "Invalid dims: ", dims);
        JnifError::check(dims <= 255, "Invalid dims: ", dims);
        JnifError::check(!baseType.isTop(), "Cannot construct an array type of ", dims,
                         " dimension(s) using as a base type Top (", baseType, ")");
//		JnifError::check(!baseType.isArray(), "base type is already an array: ",
//				baseType);

        return Type(baseType, dims);
    }

    Type TypeFactory::fromConstClass(const string& className) {
        JnifError::assert(!className.empty(), "Invalid string class");

        if (className[0] == '[') {
            const char* classNamePtr = className.c_str();
            const Type& arrayType = fromFieldDesc(classNamePtr);
            JnifError::assert(arrayType.isArray(), "Not an array: ", arrayType);
            return arrayType;
        } else {
            return objectType(className);
        }
    }

    Type TypeFactory::_parseBaseType(const char*& fieldDesc,
                                     const char* originalFieldDesc) {
        switch (*fieldDesc) {
            case 'Z':
                return booleanType();
            case 'B':
                return byteType();
            case 'C':
                return charType();
            case 'S':
                return shortType();
            case 'I':
                return intType();
            case 'D':
                return doubleType();
            case 'F':
                return floatType();
            case 'J':
                return longType();
            case 'L': {
                fieldDesc++;

                const char* classNameStart = fieldDesc;
                int len = 0;
                while (*fieldDesc != ';') {
                    JnifError::check(*fieldDesc != '\0', "");
                    fieldDesc++;
                    len++;
                }

                string className(classNameStart, len);
                return objectType(className);
            }
            default:
                JnifError::raise("Invalid field desc ", originalFieldDesc);
        }
    }

    Type TypeFactory::_getType(const char*& fieldDesc,
                               const char* originalFieldDesc, int dims) {
        const Type& baseType = _parseBaseType(fieldDesc, originalFieldDesc);
        if (dims == 0) {
            return baseType;
        } else {
            return arrayType(baseType, dims);
        }
    }

    Type TypeFactory::fromFieldDesc(const char*& fieldDesc) {
        const char* originalFieldDesc = fieldDesc;

        int dims = 0;
        while (*fieldDesc == '[') {
            JnifError::check(*fieldDesc != '\0',
                             "Reach end of string while searching for array. Field descriptor: ",
                             originalFieldDesc);
            fieldDesc++;
            dims++;
        }

        JnifError::check(*fieldDesc != '\0', "");

        const Type& t = _getType(fieldDesc, originalFieldDesc, dims);

        fieldDesc++;

        return t;
    }

    Type TypeFactory::_getReturnType(const char*& methodDesc) {
        if (*methodDesc == 'V') {
            methodDesc++;
            return voidType();
        } else {
            return fromFieldDesc(methodDesc);
        }
    }

    Type TypeFactory::fromMethodDesc(const char* methodDesc,
                                     std::vector<Type>* argsType) {
        const char* originalMethodDesc = methodDesc;

        JnifError::check(*methodDesc == '(', "Invalid beginning of method descriptor: ",
                         originalMethodDesc);
        methodDesc++;

        while (*methodDesc != ')') {
            JnifError::check(*methodDesc != '\0', "Reached end of string: ",
                             originalMethodDesc);

            const Type& t = fromFieldDesc(methodDesc);
            argsType->push_back(t);
        }

        JnifError::check(*methodDesc == ')', "Expected ')' in method descriptor: ",
                         originalMethodDesc);
        methodDesc++;

        JnifError::check(*methodDesc != '\0', "Reached end of string: ",
                         originalMethodDesc);

        const Type& returnType = _getReturnType(methodDesc);

        JnifError::check(*methodDesc == '\0', "Expected end of string: %s",
                         originalMethodDesc);

        return returnType;
    }

    Type TypeFactory::_topType(TYPE_TOP);
    Type TypeFactory::_intType(TYPE_INTEGER, "I");
    Type TypeFactory::_floatType(TYPE_FLOAT, "F");
    Type TypeFactory::_longType(TYPE_LONG, "J");
    Type TypeFactory::_doubleType(TYPE_DOUBLE, "D");
    Type TypeFactory::_booleanType(TYPE_BOOLEAN, "Z");
    Type TypeFactory::_byteType(TYPE_BYTE, "B");
    Type TypeFactory::_charType(TYPE_CHAR, "C");
    Type TypeFactory::_shortType(TYPE_SHORT, "S");
    Type TypeFactory::_nullType(TYPE_NULL);
    Type TypeFactory::_voidType(TYPE_VOID);



    Attrs::~Attrs() {
        for (Attr* attr : attrs) {
            attr->~Attr();
        }
    }

    CodeAttr::~CodeAttr() {
        if (cfg != nullptr) {
            delete cfg;
        }
    }

    const char* SignatureAttr::signature() const {
        return constPool->getUtf8(signatureIndex);
    }

    const char* SourceFileAttr::sourceFile() const {
        return constPool->getUtf8(sourceFileIndex);
    }


}
