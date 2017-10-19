/*
 * Includes
 */
#include <jnif.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace jnif;

using jnif::JnifError;

static void testConstPool() {
    jnif::model::ConstPool cp;

    auto si = cp.addString("String Test");
    auto ii = cp.addInteger(1);
    auto fi = cp.addFloat(2.1);
    auto li = cp.addLong(3);
    auto di = cp.addDouble(4.2);

    assertEquals(cp.size(), 9u);
    assertEquals(string("String Test"), string(cp.getString(si)));
    assertEquals(cp.getInteger(ii), 1);
    assertEquals(cp.getFloat(fi), 2.1f);
    assertEquals(cp.getLong(li), 3l);
    assertEquals(cp.getDouble(di), 4.2);
}

class UnitTestClassPath: public jnif::model::IClassPath {
public:

    string getCommonSuperClass(const string&, const string&) {
        return "java/lang/Object";
    }

};

static void testEmptyModel() {
    ClassFile cf("jnif/EmptyModel", ClassFile::OBJECT);

    assertEquals(string("java/lang/Object"), string(cf.getSuperClassName()));
    assertEquals(Version(51, 0), cf.version);
}

static void testPrinterModel() {
    ClassFile emptyCf("jnif/test/generated/Class1", ClassFile::OBJECT);
    ofstream os;
    os << emptyCf;

    ClassFile cf2("jnif/test/generated/Class2", "jnif/test/generated/Class");
    cf2.addMethod("main", "([Ljava/lang/String;)V", Method::STATIC | Method::PUBLIC);
    ofstream os2;
    os2 << cf2;
}

static void testException() {
//	try {
//		JnifException ex();
//		//ex << "arg1: " << 1 << ", arg2: " << "2" << ", arg3: " << 3.4;
//
//		throw ex;
//	} catch (const JnifException& ex) {
//		cerr << ex << endl;
//	}
}

static void testJoinFrameObjectAndEmpty() {
    UnitTestClassPath cp;

    const Type& s = TypeFactory::objectType("TypeS");
    const Type& t = TypeFactory::objectType("TypeT");

    Frame lhs;
    lhs.setVar2(0, s, nullptr);
    lhs.setVar2(1, t, nullptr);

    Frame rhs;
    rhs.setVar2(0, s, nullptr);

    lhs.join(rhs, &cp);

    Frame res;
    res.lva.resize(2, std::make_pair(TypeFactory::topType(), std::set<Inst*>()));
    res.setVar2(0, s, nullptr);

    JnifError::assertEquals(res, lhs);
}

static void testJoinFrameException() {
    UnitTestClassPath cp;

    const Type& classType = TypeFactory::objectType("testunit/Class");
    const Type& exType = TypeFactory::objectType("java/lang/Exception");

    Frame lhs;
    lhs.setVar2(0, TypeFactory::nullType(), nullptr);
    lhs.setVar2(1, exType, nullptr);

    Frame rhs;
    rhs.setVar2(0, classType, nullptr);

    lhs.join(rhs, &cp);

    Frame res;
    res.lva.resize(2, std::make_pair(TypeFactory::topType(), std::set<Inst*>()));
    res.setVar2(0, classType, nullptr);

    JnifError::assertEquals(res, lhs);
}

static void testJoinFrame() {
    UnitTestClassPath cp;

    ClassFile cf("testunit/Class", ClassFile::OBJECT);
    Method& m = cf.addMethod("method", "()Ltestunit/Class;",
                             Method::PUBLIC | Method::STATIC);
    ConstPool::Index cidx = cf.addUtf8("Code");
    CodeAttr* code = new CodeAttr(cidx, &cf);
    m.attrs.add(code);
    InstList& instList = m.codeAttr()->instList;

    ConstPool::Index idx = cf.addClass("testunit/Class");
    ConstPool::Index exidx = cf.addClass("java/lang/Exception");
    ConstPool::Index initidx = cf.addMethodRef(idx, "<init>", "()V");

    LabelInst* l = instList.createLabel();

    LabelInst* tryLabel = instList.createLabel();
    tryLabel->isTryStart = true;

    LabelInst* endLabel = instList.createLabel();
    // endLabel->isTryEnd = true;

    LabelInst* handlerLabel = instList.createLabel();
    handlerLabel->isCatchHandler = true;

    instList.addLabel(tryLabel);
    instList.addType(Opcode::NEW, idx);
    instList.addZero(Opcode::dup);
    instList.addInvoke(Opcode::invokespecial, initidx);
    instList.addZero(Opcode::astore_0);
    instList.addLabel(endLabel);
    instList.addJump(Opcode::GOTO, l);
    instList.addLabel(handlerLabel);
    instList.addZero(Opcode::astore_1);
    instList.addZero(Opcode::aconst_null);
    instList.addZero(Opcode::astore_0);
    instList.addLabel(l);
    instList.addZero(Opcode::aload_0);
    instList.addZero(Opcode::areturn);

    code->exceptions.push_back({tryLabel, endLabel, handlerLabel, exidx});

    cout << cf;

    cf.computeFrames(&cp);
}

static void testJoinStack() {
    ClassFile cf("testunit/Class", ClassFile::OBJECT);
    ConstPool::Index emptyString = cf.addString("");

    Method& m = cf.addMethod("method", "()Ltestunit/Class;", Method::PUBLIC);
    auto cidx = cf.addUtf8("Code");
    CodeAttr* code = new CodeAttr(cidx, &cf);
    m.attrs.add(code);
    InstList& instList = m.codeAttr()->instList;

    auto idx = cf.addClass("testunit/Class");
    auto exidx = cf.addClass("java/lang/Throwable");
    auto useidx = cf.addMethodRef(idx, "use", "(Z)V");

    auto loopHeader = instList.createLabel();
    auto loopExit = instList.createLabel();
    auto ifTrue = instList.createLabel();
    auto ifEnd = instList.createLabel();
    auto afterTry = instList.createLabel();

    auto tryStart = instList.createLabel();
    tryStart->isTryStart = true;

    auto tryEnd = instList.createLabel();
    // tryEnd->isTryEnd = true;

    auto catchHandler = instList.createLabel();
    catchHandler->isCatchHandler = true;

    //     0: (  1) aconst_null
    //     1: ( 76) astore_1
    //	label loopHeader, B: Yes, TS: No, TE: No, C: No
    //     2: ( 43) aload_1
    //     3: (199) ifnonnull label: loopExit
    //	label tryStart, B: No, TS: Yes, TE: No, C: No
    //     6: ( 42) aload_0
    //     7: ( 43) aload_1
    //     8: (198) ifnull label: ifTrue
    //    11: (  4) iconst_1
    //    12: (167) goto label: ifEnd
    //	label ifTrue, B: Yes, TS: No, TE: No, C: No
    //    15: (  3) iconst_0
    //	label ifEnd, B: Yes, TS: No, TE: No, C: No
    //    16: (182) invokevirtual #2 frheapagent/UtilMain.use: (Z)V
    //	label tryEnd, B: No, TS: No, TE: Yes, C: No
    //    19: (167) goto label: afterTry
    //	label catchHandler, B: No, TS: No, TE: No, C: Yes
    //    22: ( 77) astore_2
    //	label afterTry, B: Yes, TS: No, TE: No, C: No
    //    23: ( 18) ldc #4
    //    25: ( 76) astore_1
    //    26: (167) goto label: loopHeader
    //	label loopExit, B: Yes, TS: No, TE: No, C: No
    //    29: (177) return

    instList.addZero(Opcode::aconst_null);
    instList.addZero(Opcode::astore_1);
    instList.addLabel(loopHeader);
    instList.addZero(Opcode::aload_1);
    instList.addJump(Opcode::ifnonnull, loopExit);
    instList.addLabel(tryStart);
    instList.addZero(Opcode::aload_0);
    instList.addZero(Opcode::aload_1);
    instList.addJump(Opcode::ifnull, ifTrue);
    instList.addZero(Opcode::iconst_1);
    instList.addJump(Opcode::GOTO, ifEnd);
    instList.addLabel(ifTrue);
    instList.addZero(Opcode::iconst_0);
    instList.addLabel(ifEnd);
    instList.addInvoke(Opcode::invokevirtual, useidx);
    instList.addLabel(tryEnd);
    instList.addJump(Opcode::GOTO, afterTry);
    instList.addLabel(catchHandler);
    instList.addZero(Opcode::astore_2);
    instList.addLabel(afterTry);
    instList.addLdc(Opcode::ldc, emptyString);
    instList.addZero(Opcode::astore_1);
    instList.addJump(Opcode::GOTO, loopHeader);
    instList.addLabel(loopExit);
    instList.addZero(Opcode::RETURN);

    code->exceptions.push_back({tryStart, tryEnd, catchHandler, exidx});

    UnitTestClassPath cp;

    try {
        cf.computeFrames(&cp);
    } catch (const JnifException& ex) {
        //ofstream os("build/stack.dot");
        //cf.dot(os);
        throw ex;
    }
}

typedef void (TestFunc)();

static void run(TestFunc* testFunc, const string& testName) {
    cerr << "Running test " << testName << " ";

    try {
        testFunc();
    } catch (const JnifException& ex) {
        cerr << ex << endl;
        exit(1);
    }

    cerr << " [OK]" << endl;
}

#define RUN(testName) run(&testName, #testName)

int main(int, const char*[]) {
    RUN(testEmptyModel);
    RUN(testPrinterModel);
    RUN(testException);
    RUN(testJoinFrameObjectAndEmpty);
    RUN(testJoinFrameException);
    RUN(testJoinFrame);
    RUN(testJoinStack);
    RUN(testConstPool);

    return 0;
}
