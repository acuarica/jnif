/*
 * ClassFile.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: luigi
 */
#include "ClassFile.hpp"
#include "Error.hpp"
#include "analysis/SmtBuilder.hpp"
#include "analysis/ComputeFrames.hpp"
#include "analysis/FrameGenerator.hpp"

#include "parser/ClassFileParser.hpp"

#include <fstream>

namespace jnif {

    Method::~Method() {
        JnifError::trace("Method::~Method");
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

        JnifError::raise("ERROR! get inst list");
    }

    ClassFile::ClassFile(const u1* classFileData, const int classFileLen) :
        version(0, 0), accessFlags(0), thisClassIndex(0), superClassIndex(0), sig(&attrs) {

        parser::ClassFileParser::parse(classFileData, classFileLen, this);
    }

    ClassFile::ClassFile(const char* fileName) : sig(&attrs) {
        std::ifstream is(fileName, std::ios::in | std::ios::binary |std::ios::ate);

        if (!is.is_open()) {
            int m;
            is >> m;
            throw "File not opened!";
        }

        int fileSize = is.tellg();
        u1* buffer = new u1[fileSize];

        is.seekg(0, std::ios::beg);
        if (!is.read((char*) buffer, fileSize)) {
            throw "File not opened!";
        }

        parser::ClassFileParser::parse(buffer, fileSize, this);
    }

    Field& ClassFile::addField(ConstPool::Index nameIndex, ConstPool::Index descIndex,
                               u2 accessFlags) {
        fields.emplace_back(accessFlags, nameIndex, descIndex, this);
        return fields.back();
    }

    Method& ClassFile::addMethod(ConstPool::Index nameIndex, ConstPool::Index descIndex,
                                 u2 accessFlags) {
        methods.emplace_back(accessFlags, nameIndex, descIndex, this);
        return methods.back();
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

}
