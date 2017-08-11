
#ifndef JNIF_UNZIPFILE_HPP
#define JNIF_UNZIPFILE_HPP

#include "unzip.h"

class ZipException {
public:

  ZipException(const char* message) : message(message) {
  }

  const char* message;

};

class UnzipFile {
public:

  typedef void (*ZipCallback)(void* args, int jarid, void* buffer, int size);

  UnzipFile(const char* zipPath) {
    _uf = unzOpen64(zipPath);
    if (_uf == NULL) {
      throw ZipException("Can't open file");
    }
  }

  ~UnzipFile() {
    unzClose(_uf);
  }

  int forEach(void* args, int jarid, ZipCallback callback) {
    unz_global_info64 gi;

    int err = unzGetGlobalInfo64(_uf, &gi);
    if (err != UNZ_OK) {
      fprintf(stderr, "error %d with zipfile in unzGetGlobalInfo \n", err);
      return -1;
    }

    for (uLong i = 0; i < gi.number_entry; i++) {
      if (_extractCurrentFile(args, jarid, callback) != UNZ_OK) {
        break;
      }

      if (i + 1 < gi.number_entry) {
        err = unzGoToNextFile(_uf);
        if (err != UNZ_OK) {
          fprintf(stderr, "error %d with zipfile in unzGoToNextFile\n",err);
          break;
        }
      }
    }

    return gi.number_entry;
  }

private:

  int _extractCurrentFile(void* args, int jarid, ZipCallback callback) {
    char filename_inzip[256];
    int err = UNZ_OK;

    unz_file_info64 file_info;
    err = unzGetCurrentFileInfo64(_uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
    int size = file_info.uncompressed_size;

    if (err != UNZ_OK) {
      fprintf(stderr, "error %d with zipfile in unzGetCurrentFileInfo\n", err);
      return err;
    }

    auto isSuffix = [](const std::string& suffix, const std::string& text) {
      auto res =std::mismatch(suffix.rbegin(), suffix.rend(), text.rbegin());
      return res.first == suffix.rend();
    };

    if (!isSuffix(".class", std::string(filename_inzip))) {
      return 0;
    }

    void* buf = (void*)malloc(size);
    if (buf == NULL) {
      fprintf(stderr, "PANIC: Error allocating memory\n");
      exit(1);
    }

    err = unzOpenCurrentFilePassword(_uf, NULL);
    if (err != UNZ_OK) {
      printf("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
      // return UNZ_INTERNALERROR;
    }

    err = unzReadCurrentFile(_uf, buf, size);
    if (err < 0) {
      printf("error %d with zipfile in unzReadCurrentFile\n",err);
    } else if (err == size) {
      err = UNZ_OK;
      callback(args, jarid, buf, size);
    } else {
      printf("error while reading: %d", err);
    }

    if (err==UNZ_OK) {
      err = unzCloseCurrentFile (_uf);
      if (err!=UNZ_OK) {
        printf("error %d with zipfile in unzCloseCurrentFile\n",err);
      }
    } else
        unzCloseCurrentFile(_uf);

    free(buf);
    return err;
  }

private:
  unzFile _uf;
};

#endif
