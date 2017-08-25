
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include <unistd.h>
#include <utime.h>

#include <iostream>
#include <fstream>
#include <tuple>

// #include "UnzipFile.hpp"
#include "Db.hpp"

#include <jnif.hpp>

using namespace std;
using namespace jnif;

class MavenClass {
public:

  MavenClass(const char* repo, const char* mavenClassPath) :
    repo(repo),
    db(mavenClassPath),
    // getclassname(db, "select classnameid from cp_classname where classname=?1"),
    // getmethoddesc(db,"select methoddescid from cp_methoddesc where methoddesc=?1"),
    // getmethodref(db,"select methodrefid from cp_methodref where classnameid=?1 and methodname=?2 and methoddescid=?3"),
    insclassname(db, "insert into cp_classname (classnameid, classname) values (?1, ?2)"),
    insmethoddesc(db, "insert into cp_methoddesc (methoddescid, methoddesc) values (?1, ?2)"),
    insmethodref(db, "insert into cp_methodref (methodrefid, classnameid, methodname, methoddescid) values (?1, ?2, ?3, ?4)"),
    inssignature(db, "insert into cp_signature (signature) values (?1)"),
    insjar(db, "insert into jar (coord, path) values (?1, ?2)"),
    insclass(db, "insert into class (jarid, minor_version, major_version, access, \
classnameid, signatureid, superclassid) \
values (?1, ?2, ?3, ?4, ?5, \
(select signatureid from cp_signature where signature=?6), ?7)"),
    insinterface(db, "insert into interface (classid,interfaceid)values (?1, ?2)"),
    insmethod(db, "insert into method (classid, access, methodname, methoddescid, \
signature, exceptions) values (?1, ?2, ?3, ?4, ?5, ?6)"),
    inscode(db, "insert into code (methodid, opcode, args) values (?1, ?2, ?3)"),
    jarc(0)
  {
  }

    int jarnotfound=0;
  void process(const char* path, const char* id) {
    jarc++;

    string jarpath = string(repo) + "/" + path;

    insjar.bindText(1, id);
    insjar.bindText(2, path);
    insjar.exec();
    const long jarid = db.lastInsertRowid();

    fprintf(stderr, "[#%d %s ... ", jarc, id);
    try {
      JarFile uf(jarpath.c_str());
      int csc = uf.forEach(this, jarid, [] (void* mc, int jid, void* buf, int s) {
          ((MavenClass*)mc)->processClassFile(jid, buf, s);
        });
      fprintf(stderr, "%d classes]\n", csc);
    } catch (const JarException& ex) {
        jarnotfound++;
      fprintf(stderr, "CANTOPEN]\n");
    }
  }

  void processClassFile(long jarid, void* buffer, int size) {
    try {
      ClassFile cf((u1*)buffer, size);

      long cnid = getClassName(cf.getThisClassName());
      long scnid = getClassName(cf.getSuperClassName());

      insclass.bindLong(1, jarid);
      insclass.bindInt(2, cf.version.minorVersion());
      insclass.bindInt(3, cf.version.majorVersion());
      insclass.bindInt(4, cf.accessFlags);
      insclass.bindLong(5, cnid);

      if (cf.sig.hasSignature()) {
        inssignature.bindText(1, cf.sig.signature());
        inssignature.exec();

        insclass.bindText(6, cf.sig.signature());
      } else {
        insclass.bindNull(6);
      }

      insclass.bindLong(7, scnid);
      insclass.exec();

      long classid = db.lastInsertRowid();

      for (u2 interIndex : cf.interfaces) {
        const char* interface = cf.getClassName(interIndex);
        long interid = getClassName(interface);

        insinterface.bindLong(1, classid);
        insinterface.bindLong(2, interid);
        insinterface.exec();
      }

      for (Method* m : cf.methods) {
        long methoddescid = getMethodDesc(m->getDesc());

        insmethod.bindLong(1, classid);
        insmethod.bindInt(2, m->accessFlags);
        insmethod.bindText(3, m->getName());
        insmethod.bindLong(4, methoddescid);
        if (m->sig.hasSignature()) {
          insmethod.bindText(5, m->sig.signature());
        } else {
          insmethod.bindNull(5);
        }
        insmethod.bindInt(6, 21);
        insmethod.exec();
        long methodid = db.lastInsertRowid();

        if (m->hasCode()) {
          CodeAttr* code = m->codeAttr();
          for (Inst* inst : code->instList) {
            inscode.bindLong(1, methodid);
            inscode.bindInt(2, inst->opcode);
            doInst(*inst);

            inscode.exec();
          }
        }
      }
    } catch (const JnifException& e) {
      cerr << e << endl;
    }
  }

void doInst(Inst& inst)  {
  static constexpr int ARG = 3;
	const ConstPool& cf = *inst.constPool;

	switch (inst.kind) {
		case KIND_ZERO:
      inscode.bindNull(ARG);
			break;
		case KIND_BIPUSH:
    case KIND_SIPUSH:
      inscode.bindInt(ARG, int(inst.push()->value));
			break;
		// case KIND_LDC:
			// os << "#" << int(inst.ldc()->valueIndex);
			// break;
		// case KIND_VAR:
			// os << int(inst.var()->lvindex);
			// break;
		// case KIND_IINC:
			// os << int(inst.iinc()->index) << " " << int(inst.iinc()->value);
			// break;
		// case KIND_JUMP:
			// os << "label: " << inst.jump()->label2->label()->id;
			// break;
		// case KIND_TABLESWITCH:
			// os << "default: " << inst.ts()->def->label()->id << ", from: "
			// 		<< inst.ts()->low << " " << inst.ts()->high << ":";

			// for (int i = 0; i < inst.ts()->high - inst.ts()->low + 1; i++) {
			// 	Inst* l = inst.ts()->targets[i];
			// 	os << " " << l->label()->id;
			// }
			// break;
		// case KIND_LOOKUPSWITCH:
			// os << inst.ls()->defbyte->label()->id << " " << inst.ls()->npairs
			// 		<< ":";

			// for (u4 i = 0; i < inst.ls()->npairs; i++) {
			// 	u4 k = inst.ls()->keys[i];
			// 	Inst* l = inst.ls()->targets[i];
			// 	os << " " << k << " -> " << l->label()->id;
			// }
			// break;
		// case KIND_FIELD: {
			// string className, name, desc;
			// cf.getFieldRef(inst.field()->fieldRefIndex, &className, &name, &desc);
			// os << className << name << desc;
			// break;
		// }
		case KIND_INVOKE: {
			ConstIndex mid = inst.invoke()->methodRefIndex;
			String className, name, desc;
			if (cf.getTag(mid) == CONST_INTERMETHODREF) {
				cf.getInterMethodRef(inst.invoke()->methodRefIndex, &className, &name, &desc);
			} else {
				cf.getMethodRef(inst.invoke()->methodRefIndex, &className, &name, &desc);
			}

      long mrid = getMethodRef(className.c_str(), name.c_str(), desc.c_str());
      // stringstream ss;
			// ss << className << "." << name << ": " << desc << endl;
      inscode.bindLong(ARG, mrid);
			break;
		}
		// case KIND_INVOKEINTERFACE: {
			// String className, name, desc;
			// cf.getInterMethodRef(inst.invokeinterface()->interMethodRefIndex, &className, &name, &desc);

			// os << className << "." << name << ": " << desc << "("
			// 		<< int(inst.invokeinterface()->count) << ")";
			// break;
		// }
		// case KIND_INVOKEDYNAMIC: {
			// os << int(inst.indy()->callSite()) << "";
			// break;
		// }
		case KIND_TYPE: {
      const char* className = cf.getClassName(inst.type()->classIndex);
      long cnid = getClassName(className);
      inscode.bindLong(ARG, cnid);
			break;
		}
		// case KIND_NEWARRAY:
			// os << int(inst.newarray()->atype);
			// break;
		// case KIND_MULTIARRAY: {
			// string className = cf.getClassName(inst.multiarray()->classIndex);
			// os << className << " " << inst.multiarray()->dims;
			// break;
		// }
		// case KIND_PARSE4TODO:
			// Error::raise("FrParse4__TODO__Instr not implemented");
			// break;
		// case KIND_RESERVED:
			// Error::raise("FrParseReservedInstr not implemented");
			// break;
		// case KIND_FRAME:
			//	os << "Frame " << inst.frame.frame;
			// break;
		default:
      // sqlite3_bind_null(inscode, ARG);
      break;
	}
}

    long getClassName(const string& className) {
        return getByValue(classNameConstPool, className);
    }

    long getMethodDesc(const string& methodDesc) {
        return getByValue(methodDescConstPool, methodDesc);
    }

    long getMethodRef(const string& className, const string& methodName, const string& methodDesc) {
        long cnid = getClassName(className);
        long mdid = getMethodDesc(methodDesc);
        return getByValue(methodRefConstPool, make_tuple(cnid, methodName, mdid));
    }

    void insertConstPool() {
        for (auto& e : classNameConstPool) {
            insclassname.bindLong(1, e.second);
            insclassname.bindText(2, e.first.c_str());
            insclassname.exec();
        }

        for (auto& e : methodDescConstPool) {
            insmethoddesc.bindLong(1, e.second);
            insmethoddesc.bindText(2, e.first.c_str());
            insmethoddesc.exec();
        }

        for (auto& e : methodRefConstPool) {
            insmethodref.bindLong(1, e.second);
            insmethodref.bindLong(2, get<0>(e.first));
            insmethodref.bindText(3, get<1>(e.first).c_str());
            insmethodref.bindLong(4, get<2>(e.first));
            insmethodref.exec();
        }
    }

    template <typename TKey>
    static long getByValue(map<TKey, long>& cpMap, const TKey& value) {
        auto it = cpMap.find(value);
        if (it != cpMap.end()) {
            return it->second;
        } else {
            long id = cpMap.size() + 1;
            cpMap[value] = id;
            return id;
        }
    }

    const char* repo;
    Db db;

    Stmt insclassname;
    Stmt insmethoddesc;
    Stmt insmethodref;
    Stmt inssignature;
    Stmt insjar;
    Stmt insclass;
    Stmt insinterface;
    Stmt insmethod;
    Stmt inscode;

    int jarc;

    map<string, long> classNameConstPool;
    map<string, long> methodDescConstPool;
    map<tuple<long, string, long>, long> methodRefConstPool;

};

int main(int argc, const char* argv[]) {
  if (argc < 5) {
    fprintf(stderr, "Not enough arguments: %d\n", argc);
    return 1;
  }

  const char* mavenIndexPath = argv[1];
  const char* repo = argv[2];
  const char* selectArts = argv[3];
  const char* mavenClassPath = argv[4];

  printf("* Maven Index DB: %s\n", mavenIndexPath);
  printf("* Maven Repo: %s\n", repo);
  printf("* Select Artifacts: %s\n", selectArts);
  printf("* Maven Class DB: %s\n", mavenClassPath);

  try {
    Db db(mavenIndexPath);
    MavenClass mc(repo, mavenClassPath);

    mc.db.start();
    db.exec(selectArts, [] (void* mc, int, char** argv, char**) {
        ((MavenClass*)mc)->process(argv[3], argv[2]);
        return 0;
      }, &mc);
    fprintf(stderr, "* Inserting into constant pool tables ... ");
    mc.insertConstPool();
    fprintf(stderr, "[DONE]");
    mc.db.commit();

    printf(": %d\n", mc.jarnotfound);
    printf("Total artifacts processed: %d\n", mc.jarc);
  } catch (const DbException& e) {
    cerr << e << endl;
  }
}
