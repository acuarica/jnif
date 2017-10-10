/*
 * ClassFile.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: luigi
 */
#include "ClassFile.hpp"
#include "../Error.hpp"
#include "../analysis/SmtBuilder.hpp"
#include "../analysis/ComputeFrames.hpp"
#include "../analysis/FrameGenerator.hpp"

#include "../parser/ClassFileParser.hpp"

#include <fstream>

namespace jnif::model {

    bool Method::isInit() const {
        String name = constPool.getUtf8(nameIndex);
        return hasCode() && name == "<init>";
    }

    bool Method::isMain() const {
        String name = constPool.getUtf8(nameIndex);
        String desc = constPool.getUtf8(descIndex);

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


}
