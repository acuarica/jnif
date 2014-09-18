/*
 * UnzipFile.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef UNZIPFILE_HPP
#define UNZIPFILE_HPP


class UnzipFile {
public:

	class Iterator {
	public:

	};
	UnzipFile(const char* zipFileName) :
			_uf(unzOpen64(zipFileName)) {
		if (_uf == nullptr) {
			throw "Unable to open file";
		}
	}

	~UnzipFile() {
		unzClose(_uf);
	}

	Iterator begin() {

	}

	int do_list(unzFile uf) {
		uLong i;
		unz_global_info64 gi;
		int err;

		err = unzGetGlobalInfo64(uf, &gi);
		if (err != UNZ_OK)
			printf("error %d with zipfile in unzGetGlobalInfo \n", err);
		printf(
				"  Length  Method     Size Ratio   Date    Time   CRC-32     Name\n");
		printf(
				"  ------  ------     ---- -----   ----    ----   ------     ----\n");
		for (i = 0; i < gi.number_entry; i++) {
			char filename_inzip[256];
			unz_file_info64 file_info;
			uLong ratio = 0;
			const char *string_method;
			char charCrypt = ' ';
			err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip,
					sizeof(filename_inzip), NULL, 0, NULL, 0);
			if (err != UNZ_OK) {
				printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
				break;
			}
			if (file_info.uncompressed_size > 0)
				ratio = (uLong) ((file_info.compressed_size * 100)
						/ file_info.uncompressed_size);

			/* display a '*' if the file is crypted */
			if ((file_info.flag & 1) != 0)
				charCrypt = '*';

			if (file_info.compression_method == 0)
				string_method = "Stored";
			else if (file_info.compression_method == Z_DEFLATED) {
				uInt iLevel = (uInt) ((file_info.flag & 0x6) / 2);
				if (iLevel == 0)
					string_method = "Defl:N";
				else if (iLevel == 1)
					string_method = "Defl:X";
				else if ((iLevel == 2) || (iLevel == 3))
					string_method = "Defl:F"; /* 2:fast , 3 : extra fast*/
			} else if (file_info.compression_method == Z_BZIP2ED) {
				string_method = "BZip2 ";
			} else
				string_method = "Unkn. ";

			Display64BitsSize(file_info.uncompressed_size, 7);
			printf("  %6s%c", string_method, charCrypt);
			Display64BitsSize(file_info.compressed_size, 7);
			printf(" %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
					ratio, (uLong) file_info.tmu_date.tm_mon + 1,
					(uLong) file_info.tmu_date.tm_mday,
					(uLong) file_info.tmu_date.tm_year % 100,
					(uLong) file_info.tmu_date.tm_hour,
					(uLong) file_info.tmu_date.tm_min, (uLong) file_info.crc,
					filename_inzip);
			if ((i + 1) < gi.number_entry) {
				err = unzGoToNextFile(uf);
				if (err != UNZ_OK) {
					printf("error %d with zipfile in unzGoToNextFile\n", err);
					break;
				}
			}
		}

		return 0;
	}


	int do_extract_onefile(unzFile uf, const char* filename,
			int opt_extract_without_path, int opt_overwrite, const char* password) {
		int err = UNZ_OK;
		if (unzLocateFile(uf, filename, CASESENSITIVITY) != UNZ_OK) {
			printf("file %s not found in the zipfile\n", filename);
			return 2;
		}

		if (do_extract_currentfile(uf, &opt_extract_without_path, &opt_overwrite,
				password) == UNZ_OK)
			return 0;
		else
			return 1;
	}


	void extractFile(const int* popt_extract_without_path,
			int* popt_overwrite, const char* password) {

		char* filename_withoutpath;
		char* p;
		int err = UNZ_OK;
		FILE *fout = NULL;
		void* buf;
		uInt size_buf;

		char filenameInzip[256];
		unz_file_info64 fileInfo;
		err = unzGetCurrentFileInfo64(_uf, &fileInfo, filenameInzip,
				sizeof(filenameInzip), nullptr, 0, nullptr, 0);
		fileInfo.uncompressed_size

		if (err != UNZ_OK) {
			printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
			return err;
		}

		size_buf = WRITEBUFFERSIZE;
		buf = (void*) malloc(size_buf);
		if (buf == NULL) {
			printf("Error allocating memory\n");
			return UNZ_INTERNALERROR;
		}

		p = filename_withoutpath = filename_inzip;
		while ((*p) != '\0') {
			if (((*p) == '/') || ((*p) == '\\'))
				filename_withoutpath = p + 1;
			p++;
		}

		if ((*filename_withoutpath) == '\0') {
			if ((*popt_extract_without_path) == 0) {
				printf("creating directory: %s\n", filename_inzip);
				mymkdir(filename_inzip);
			}
		} else {
			err = unzOpenCurrentFilePassword(uf, password);
			if (err != UNZ_OK) {
				printf("error %d with zipfile in unzOpenCurrentFilePassword\n",
						err);
			}

			if (fout != NULL) {
				do {
					err = unzReadCurrentFile(_uf, buf, size_buf);
					if (err < 0) {
						printf("error %d with zipfile in unzReadCurrentFile\n",
								err);
						break;
					}
					if (err > 0)
						if (fwrite(buf, err, 1, fout) != 1) {
							printf("error in writing extracted file\n");
							err = UNZ_ERRNO;
							break;
						}
				} while (err > 0);
			}

			if (err == UNZ_OK) {
				err = unzCloseCurrentFile(uf);
				if (err != UNZ_OK) {
					printf("error %d with zipfile in unzCloseCurrentFile\n", err);
				}
			} else
				unzCloseCurrentFile(uf); /* don't lose the error */
		}

		free(buf);
		return err;
	}

	int do_extract(unzFile uf, int opt_extract_without_path, int opt_overwrite,
			const char* password) {
		uLong i;
		unz_global_info64 gi;
		int err;
		FILE* fout = NULL;

		err = unzGetGlobalInfo64(uf, &gi);
		if (err != UNZ_OK)
			printf("error %d with zipfile in unzGetGlobalInfo \n", err);

		for (i = 0; i < gi.number_entry; i++) {
			if (do_extract_currentfile(uf, &opt_extract_without_path,
					&opt_overwrite, password) != UNZ_OK)
				break;

			if ((i + 1) < gi.number_entry) {
				err = unzGoToNextFile(uf);
				if (err != UNZ_OK) {
					printf("error %d with zipfile in unzGoToNextFile\n", err);
					break;
				}
			}
		}

		return 0;
	}


private:

	unzFile _uf;
};



#endif
