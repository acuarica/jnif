/*
 miniunz.c
 Version 1.1, February 14h, 2010
 sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

 Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

 Modifications of Unzip for Zip64
 Copyright (C) 2007-2008 Even Rouault

 Modifications for Zip64 support on both zip and unzip
 Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
 */

#if (!defined(_WIN32)) && (!defined(WIN32)) && (!defined(__APPLE__))
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BIT
#define _FILE_OFFSET_BIT 64
#endif
#endif

#ifdef __APPLE__
// In darwin and perhaps other BSD variants off_t is a 64 bit value, hence no need for specific 64 bit functions
#define FOPEN_FUNC(filename, mode) fopen(filename, mode)
#define FTELLO_FUNC(stream) ftello(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko(stream, offset, origin)
#else
#define FOPEN_FUNC(filename, mode) fopen64(filename, mode)
#define FTELLO_FUNC(stream) ftello64(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef _WIN32
# include <direct.h>
# include <io.h>
#else
# include <unistd.h>
# include <utime.h>
#endif

#include "unzip.h"

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif
/*
 mini unzip, demo of unzip package

 usage :
 Usage : miniunz [-exvlo] file.zip [file_to_extract] [-d extractdir]

 list the file in the zipfile, and print the content of FILE_ID.ZIP or README.TXT
 if it exists
 */

/* change_file_date : change the date/time of a file
 filename : the filename of the file where date/time must be modified
 dosdate : the new date at the MSDos format (4 bytes)
 tmu_date : the SAME new date at the tm_unz format */
void change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date) {
#ifdef _WIN32
	HANDLE hFile;
	FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

	hFile = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE,
			0,NULL,OPEN_EXISTING,0,NULL);
	GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
	DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
	LocalFileTimeToFileTime(&ftLocal,&ftm);
	SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
	CloseHandle(hFile);
#else
#ifdef unix || __APPLE__
	struct utimbuf ut;
	struct tm newdate;
	newdate.tm_sec = tmu_date.tm_sec;
	newdate.tm_min=tmu_date.tm_min;
	newdate.tm_hour=tmu_date.tm_hour;
	newdate.tm_mday=tmu_date.tm_mday;
	newdate.tm_mon=tmu_date.tm_mon;
	if (tmu_date.tm_year > 1900)
	newdate.tm_year=tmu_date.tm_year - 1900;
	else
	newdate.tm_year=tmu_date.tm_year;
	newdate.tm_isdst=-1;

	ut.actime=ut.modtime=mktime(&newdate);
	utime(filename,&ut);
#endif
#endif
}

/* mymkdir and change_file_date are not 100 % portable
 As I don't know well Unix, I wait feedback for the unix portion */

int makedir(char *newdir) {
	char *buffer;
	char *p;
	int len = (int) strlen(newdir);

	if (len <= 0)
		return 0;

	buffer = (char*) malloc(len + 1);
	if (buffer == NULL) {
		printf("Error allocating memory\n");
		return UNZ_INTERNALERROR;
	}
	strcpy(buffer, newdir);

	if (buffer[len - 1] == '/') {
		buffer[len - 1] = '\0';
	}
	if (mymkdir(buffer) == 0) {
		free(buffer);
		return 1;
	}

	p = buffer + 1;
	while (1) {
		char hold;

		while (*p && *p != '\\' && *p != '/')
			p++;
		hold = *p;
		*p = 0;
		if ((mymkdir(buffer) == -1) && (errno == ENOENT)) {
			printf("couldn't create directory %s\n", buffer);
			free(buffer);
			return 0;
		}
		if (hold == 0)
			break;
		*p++ = hold;
	}
	free(buffer);
	return 1;
}



int main(int argc, char *argv[]) {
	const char *filename_to_extract = NULL;
	const char *password = NULL;
	const char *dirname = NULL;

	UnzipFile zf;

	if (opt_do_list == 1)
		ret_value = do_list(uf);
	else if (opt_do_extract == 1) {
		if (opt_extractdir && chdir(dirname)) {
			printf("Error changing into %s, aborting\n", dirname);
			exit(-1);
		}

		if (filename_to_extract == NULL)
			ret_value = do_extract(uf, opt_do_extract_withoutpath,
					opt_overwrite, password);
		else
			ret_value = do_extract_onefile(uf, filename_to_extract,
					opt_do_extract_withoutpath, opt_overwrite, password);
	}

	return 0;
}
