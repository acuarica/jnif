/*
 * Includes
 */
#include <jnif.hpp>

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
        cout << cf << endl;
    } catch (const JnifException& ex) {
        cerr << ex << endl;
        return 1;
    }

}
