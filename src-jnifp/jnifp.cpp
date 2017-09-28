/*
 * Includes
 */

#include <jnif.hpp>

#include <Dominator.hpp>

using namespace std;
using namespace jnif;

class UnitTestClassPath: public IClassPath {
public:

    String getCommonSuperClass(const String&, const String&) {
        return "java/lang/Object";
    }

};

int main(int argc, const char* argv[]) {
    if (argc <= 1) {
        cerr << "Usage: " << endl;
        cerr << "  " << argv[0] << " <class file name>.class" << endl;
        return 1;
    }

    try {
        ClassFile cf(argv[1]);
        UnitTestClassPath cp;
        for (Method& m : cf.methods) {
            if (m.hasCode()) {
                InstList& instList = m.codeAttr()->instList;

                int id = 1;
                for (Inst* inst : instList) {
                    inst->id = id;
                    id++;
                }
            }
        }

        cf.computeFrames(&cp);
        // cout << cf << endl;
        auto it = cf.getMethod("m");
        if (it != cf.methods.end()) {
            const Method& m = *it;
            InstList& instList = m.codeAttr()->instList;
            cout << m;
            Inst* inst = instList.getInst(30);
            if (inst != nullptr) {
                cout << "@ " << inst->_offset << ": " << *inst << endl;
                // std::set<Inst*> ls;
                // ls.push_back(inst);
                // for (Inst* ii : ls) {
                // ls.insert(inst->consumes.begin(), inst->consumes.end());
                // }
            }

            ControlFlowGraph& cfg = *m.codeAttr()->cfg;
            Dominator ds(cfg);



            // auto ds = cfg.dominance(nullptr);
            for (auto d : ds) {
                cout << d.first->name << endl;
                for (auto dp : d.second) {
                    cout << "  " << dp->name << endl;
                }
            }
        }
    } catch (const JnifException& ex) {
        cerr << ex << endl;
        return 1;
    }

}
