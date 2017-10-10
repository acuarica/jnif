/*
 * Includes
 */

#include <jnif.hpp>


#include "../src-libjnif/Dominator.hpp"

using namespace std;
using namespace jnif;
using namespace jnif::stream;

class UnitTestClassPath: public IClassPath {
public:

    String getCommonSuperClass(const String&, const String&) {
        return "java/lang/Object";
    }

};

template <class TDir>
void print(ControlFlowGraph& cfg) {
    SDom<TDir> ds(cfg);
    cout << ds << endl;

    IDom<TDir> dt(ds);
    cout << dt;
}

int main(int argc, const char* argv[]) {
    auto c =  getcwd(nullptr, 0);
    cerr << c << endl;
    if (argc <= 1) {
        cerr << "Usage: " << endl;
        cerr << "  " << argv[0] << " <class file name>.class" << endl;
        return 1;
    }

    try {
        ClassFileStream cf(argv[1]);
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
        cout << cf << endl;
        auto it = cf.getMethod("m1");
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
            print<Forward>(cfg);
            cout << "----" << endl;
            print<Backward>(cfg);
        }
    } catch (const char* ex) {
        cerr << ex << endl;
        return 1;
    }

}
