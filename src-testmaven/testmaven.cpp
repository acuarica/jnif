
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

#include <sqlite3.h>
#include <jnif.hpp>
#include "unzip.h"

using namespace std;
using namespace jnif;

static sqlite3 *bytecodedb;
static sqlite3_stmt* insclassname;
static sqlite3_stmt* insmethoddesc;
static sqlite3_stmt* inssignature;
static sqlite3_stmt* insjar;
static sqlite3_stmt* insclass;
static sqlite3_stmt* insinterface;
static sqlite3_stmt* insmethod;
static sqlite3_stmt* inscode;

static bool isSuffix(const string& suffix, const string& text) {
	auto res = std::mismatch(suffix.rbegin(), suffix.rend(), text.rbegin());

	return res.first == suffix.rend();
}

static constexpr int ARG = 3;

void doInst(Inst& inst)  {
	const ConstPool& cf = *inst.constPool;

	switch (inst.kind) {
		case KIND_ZERO:
      sqlite3_bind_null(inscode, ARG);
			break;
		case KIND_BIPUSH:
      sqlite3_bind_int(inscode, ARG, int(inst.push()->value));
			break;
		case KIND_SIPUSH:
      sqlite3_bind_int(inscode, ARG, int(inst.push()->value));
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

      stringstream ss;
			ss << className << "." << name << ": " << desc;
      sqlite3_bind_text(inscode, ARG, ss.str().c_str(), -1, SQLITE_STATIC);
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
      const char* classname = cf.getClassName(inst.type()->classIndex);
      sqlite3_bind_text(inscode, ARG, classname, -1, SQLITE_STATIC);
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
      sqlite3_bind_null(inscode, ARG);
	}
}

int do_extract_currentfile(unzFile uf, long jarid) {
  char filename_inzip[256];
  int err = UNZ_OK;

  unz_file_info64 file_info;
  err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
  int size = file_info.uncompressed_size;

  if (err != UNZ_OK) {
    fprintf(stderr, "error %d with zipfile in unzGetCurrentFileInfo\n", err);
    return err;
  }

	if (!isSuffix(".class", string(filename_inzip))) {
    return 0;
  }

  u1* buf = (u1*)malloc(size);
  if (buf == NULL) {
    fprintf(stderr, "PANIC: Error allocating memory\n");
    exit(1);
  }

  err = unzOpenCurrentFilePassword(uf, NULL);
  if (err != UNZ_OK) {
    printf("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
    // return UNZ_INTERNALERROR;
  }

  err = unzReadCurrentFile(uf, buf, size);
  if (err < 0) {
    printf("error %d with zipfile in unzReadCurrentFile\n",err);
  } else if (err == size) {
    try {
      ClassFile cf(buf, size);

      sqlite3_bind_text(insclassname, 1, cf.getThisClassName(), -1, SQLITE_STATIC);
      sqlite3_step(insclassname);
      sqlite3_reset(insclassname);

      sqlite3_bind_text(insclassname, 1, cf.getSuperClassName().c_str(), -1, SQLITE_STATIC);
      sqlite3_step(insclassname);
      sqlite3_reset(insclassname);

      sqlite3_bind_int(insclass, 1, jarid);
      sqlite3_bind_int(insclass, 2, cf.version.minorVersion());
      sqlite3_bind_int(insclass, 3, cf.version.majorVersion());
      sqlite3_bind_int(insclass, 4, cf.accessFlags);
      sqlite3_bind_text(insclass, 5, cf.getThisClassName(), -1, SQLITE_STATIC);

      if (cf.sig.hasSignature()) {
        sqlite3_bind_text(inssignature, 1, cf.sig.signature(), -1, SQLITE_STATIC);
        sqlite3_step(inssignature);
        sqlite3_reset(inssignature);

        sqlite3_bind_text(insclass, 6, cf.sig.signature(), -1, SQLITE_STATIC);
      } else {
        sqlite3_bind_null(insclass, 6);
      }

      sqlite3_bind_text(insclass, 7, cf.getSuperClassName().c_str(), -1, SQLITE_STATIC);

      sqlite3_step(insclass);
      sqlite3_reset(insclass);

      long classid = sqlite3_last_insert_rowid(bytecodedb);

      for (u2 interIndex : cf.interfaces) {
        const char* interface = cf.getClassName(interIndex);
        sqlite3_bind_text(insclassname, 1, interface, -1, SQLITE_STATIC);
        sqlite3_step(insclassname);
        sqlite3_reset(insclassname);

        sqlite3_bind_int(insinterface, 1, classid);
        sqlite3_bind_text(insinterface, 2, interface, -1, SQLITE_STATIC);
        sqlite3_step(insinterface);
        sqlite3_reset(insinterface);
      }

      for (Method* m : cf.methods) {
        const char* methoddesc = m->getDesc();
        sqlite3_bind_text(insmethoddesc, 1, methoddesc, -1, SQLITE_STATIC);
        sqlite3_step(insmethoddesc);
        sqlite3_reset(insmethoddesc);

        sqlite3_bind_int(insmethod, 1, classid);
        sqlite3_bind_int(insmethod, 2, m->accessFlags);
        sqlite3_bind_text(insmethod, 3, m->getName(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insmethod, 4, methoddesc, -1, SQLITE_STATIC);
        if (m->sig.hasSignature()) {
          sqlite3_bind_text(insmethod, 5, m->sig.signature(), -1, SQLITE_STATIC);
        } else {
          sqlite3_bind_null(insmethod, 5);
        }
        sqlite3_bind_int(insmethod, 6, 21);

        sqlite3_step(insmethod);
        sqlite3_reset(insmethod);
        long methodid = sqlite3_last_insert_rowid(bytecodedb);

        if (m->hasCode()) {
          CodeAttr* code = m->codeAttr();
          for (Inst* inst : code->instList) {
            sqlite3_bind_int(inscode, 1, methodid);
            sqlite3_bind_int(inscode, 2, inst->opcode);
            doInst(*inst);

            sqlite3_step(inscode);
            sqlite3_reset(inscode);
          }
        }
      }
    } catch (const JnifException& e) {
      cerr << e << endl;
    }

    err = UNZ_OK;
  } else {
    printf("error while reading: %d", err);
  }

  if (err==UNZ_OK) {
    err = unzCloseCurrentFile (uf);
    if (err!=UNZ_OK) {
      printf("error %d with zipfile in unzCloseCurrentFile\n",err);
    }
  } else
      unzCloseCurrentFile(uf);

  free(buf);
  return err;
}

void do_extract(unzFile uf, long jarid) {
    unz_global_info64 gi;

    int err = unzGetGlobalInfo64(uf,&gi);
    if (err != UNZ_OK) {
      fprintf(stderr, "error %d with zipfile in unzGetGlobalInfo \n", err);
      return ;
    }

    for (uLong i = 0; i < gi.number_entry; i++) {
      if (do_extract_currentfile(uf, jarid) != UNZ_OK)
            break;

        if (i + 1 < gi.number_entry) {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK) {
              fprintf(stderr, "error %d with zipfile in unzGoToNextFile\n",err);
              break;
            }
        }
    }

    int classesc = gi.number_entry;
    fprintf(stderr, "%d classes]\n", classesc);
}

static int jarc = 0;

static int process(const char* repo, const char* path, const char* id){
  jarc++;

  string jarpath = string(repo) + "/" + path;

  sqlite3_bind_text(insjar, 1, id, -1, SQLITE_STATIC);
  sqlite3_bind_text(insjar, 2, path, -1, SQLITE_STATIC);
  sqlite3_step(insjar);
  long jarid = sqlite3_last_insert_rowid(bytecodedb);
  sqlite3_reset(insjar);

  fprintf(stderr, "[#%d %s ...", jarc, path);

  unzFile uf = unzOpen64(jarpath.c_str());

  if (uf == NULL) {
    fprintf(stderr, "CANTOPEN]\n");
  } else {
    do_extract(uf, jarid);
    unzClose(uf);
  }

  // if (jarc == 20) return 1;

  return 0;
}

static int callback(void* repo, int, char** argv, char**) {
  return process((const char*)repo, argv[3], argv[2]);
}

int main(int argc, const char* argv[]) {
  sqlite3 *db;
  char *zErrMsg = 0;

  if (argc < 5) {
    fprintf(stderr, "Not enough arguments: %d\n", argc);
    return 1;
  }

  printf("* Maven Index DB: %s\n", argv[1]);
  printf("* Maven Repo: %s\n", argv[2]);
  printf("* Select Artifacts Query: %s\n", argv[3]);
  printf("* Maven Class DB: %s\n", argv[4]);

  int rc = sqlite3_open(argv[1], &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }

  rc = sqlite3_open(argv[4], &bytecodedb);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Can't open bytecode database: %s\n", sqlite3_errmsg(bytecodedb));
    sqlite3_close(bytecodedb);
    return 1;
  }

  sqlite3_exec(bytecodedb, "pragma journal_mode = off", NULL, NULL, NULL);
  sqlite3_exec(bytecodedb, "pragma synchronous = off", NULL, NULL, NULL);
  sqlite3_exec(bytecodedb, "pragma locking_mode = exclusive", NULL, NULL, NULL);

  sqlite3_prepare_v2(bytecodedb, "insert into cp_classname (classname) values (?1)", -1, &insclassname, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into cp_methoddesc (methoddesc) values (?1)", -1, &insmethoddesc, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into cp_signature (signature) values (?1)", -1, &inssignature, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into jar (coord, path) values (?1, ?2)", -1, &insjar, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into class (jarid, minor_version, major_version, access, classnameid, signatureid, superclassid) values (?1, ?2, ?3, ?4, (select classnameid from cp_classname where classname=?5), (select signatureid from cp_signature where signature=?6), (select classnameid from cp_classname where classname=?7))", -1, &insclass, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into interface (classid, interfaceid) values (?1, (select classnameid from cp_classname where classname=?2))", -1, &insinterface, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into method (classid, access, methodname, methoddescid, signature, exceptions) values (?1, ?2, ?3, (select methoddescid from cp_methoddesc where methoddesc=?4), ?5, ?6)", -1, &insmethod, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into code (methodid, opcode, args) values (?1, ?2, ?3)", -1, &inscode, NULL);

  rc = sqlite3_exec(db, argv[3], callback, (void*)argv[2], &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  printf("Total artifacts processed: %d\n", jarc);

  sqlite3_finalize(insclassname);
  sqlite3_finalize(insmethoddesc);
  sqlite3_finalize(inssignature);
  sqlite3_finalize(insjar);
  sqlite3_finalize(insclass);
  sqlite3_finalize(insinterface);
  sqlite3_finalize(insmethod);
  sqlite3_finalize(inscode);

  sqlite3_close(db);
  sqlite3_close(bytecodedb);
}
