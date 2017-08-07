
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
static sqlite3_stmt* insjar;
static sqlite3_stmt* insclass;
static sqlite3_stmt* insmethod;
static sqlite3_stmt* inscode;

static bool isSuffix(const string& suffix, const string& text) {
	auto res = std::mismatch(suffix.rbegin(), suffix.rend(), text.rbegin());

	return res.first == suffix.rend();
}

int do_extract_currentfile(unzFile uf) {
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
    // try {
      // ClassFile cf(buf, size);

      // sqlite3_bind_text(insclass, 1, cf.getThisClassName(), -1, SQLITE_STATIC);
      // sqlite3_bind_int(insclass, 2, 21);
      // sqlite3_bind_int(insclass, 3, 21);
      // sqlite3_bind_int(insclass, 4, 21);
      // sqlite3_bind_int(insclass, 5, 21);
      // sqlite3_bind_int(insclass, 6, 21);

      // sqlite3_step(insclass);
      // sqlite3_reset(insclass);
    // } catch (const JnifException& e) {
      // cerr << e << endl;
    // }

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

void do_extract(unzFile uf) {
    unz_global_info64 gi;

    int err = unzGetGlobalInfo64(uf,&gi);
    if (err != UNZ_OK) {
      sqlite3_bind_text(insjar, 3, "ZIPERROR", -1, SQLITE_STATIC);
      fprintf(stderr, "error %d with zipfile in unzGetGlobalInfo \n", err);
      return ;
    }

    for (uLong i = 0; i < gi.number_entry; i++) {
        if (do_extract_currentfile(uf) != UNZ_OK)
            break;

        if (i + 1 < gi.number_entry) {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK) {
                printf("error %d with zipfile in unzGoToNextFile\n",err);
                break;
            }
        }
    }

    int classesc = gi.number_entry;
    sqlite3_bind_int(insjar, 3, classesc);
    // printf("%d entries]\n", classesc);
}

static int jarc = 0;

static int callback(void* repo, int, char** argv, char**) {
  jarc++;

  string jarpath = string((char*)repo) + "/" + argv[3];

  sqlite3_bind_text(insjar, 1, argv[2], -1, SQLITE_STATIC);
  sqlite3_bind_text(insjar, 2, argv[3], -1, SQLITE_STATIC);

  // printf("[#%d %s ...", jarc, jarpath.c_str());

  unzFile uf = unzOpen64(jarpath.c_str());

  if (uf == NULL) {
    printf("CANTOPEN]\n");
    sqlite3_bind_text(insjar, 3, "CANTOPEN", -1, SQLITE_STATIC);
  } else {
    do_extract(uf);
    unzClose(uf);
  }

  // sqlite3_step(insjar);
  // sqlite3_reset(insjar);

  return 0;
}

int main(int argc, const char* argv[]) {
  sqlite3 *db;
  char *zErrMsg = 0;

  if (argc < 5) {
    fprintf(stderr, "Not enough arguments: %d\n", argc);
    return 1;
  }

  printf("Maven Index DB: %s\n", argv[1]);
  printf("Maven Repo: %s\n", argv[2]);
  printf("Select Artifacts Query: %s\n", argv[3]);
  printf("Maven Class DB: %s\n", argv[4]);

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

  sqlite3_prepare_v2(bytecodedb, "insert into jar (id, path, log) values (?1, ?2, ?3)", -1, &insjar, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into class (version, access, classname, signature, superclass, interfaces) values (?1, ?2, ?3, ?4, ?5, ?6)", -1, &insclass, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into method (classid, access, methodname, methoddesc, signature, exceptions) values (?1, ?2, ?3, ?4, ?5, ?6)", -1, &insmethod, NULL);
  sqlite3_prepare_v2(bytecodedb, "insert into code (methodid, opcode, args) values (?1, ?2, ?3)", -1, &inscode, NULL);


  rc = sqlite3_exec(db, argv[3], callback, (void*)argv[2], &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  printf("Total artifacts processed: %d\n", jarc);

  sqlite3_finalize(insjar);
  sqlite3_finalize(insclass);
  sqlite3_finalize(insmethod);
  sqlite3_finalize(inscode);

  sqlite3_close(db);
  sqlite3_close(bytecodedb);
}
