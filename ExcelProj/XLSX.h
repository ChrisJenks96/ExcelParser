#ifndef XLSX_H
#define XLSX_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

//Note: Currently testing on MS Office 2010

//Pragma pack because our 30 byte structure gets realigned to 32 bytes!!
#pragma pack(push, 1)
//https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT
typedef struct ZIP_HEADER
{
	//50 4B 03 04 Header in XLSX (ZIP magic header)
	char magic[4]; // 0
	uint16_t version;
	//general purpose bit flag
	uint16_t gpbFlag;
	uint16_t compressMethod;
	uint16_t lastModFileTime;
	uint16_t lastModFileDate;
	//checksum
	uint32_t crc32;
	uint32_t compressedSize;
	uint32_t uncompressedSize;
	uint16_t fileNameLength;
	uint16_t extraFieldLength;
} ZIP_HEADER;

typedef struct XLSX_DATA
{
	ZIP_HEADER zipHeader;
	//get size from ZIP_HEADER fileNameLength
	char* filename;
} XLSX_DATA;
#pragma pack(pop)

class XLSX
{
	public:
		XLSX() {}
		bool Load(const char* fn, int numWorksheets);
		void Destroy();
		int GetNumberWorksheets() { return numWorksheets; }
		~XLSX() {}
	private:
		int numWorksheets;
};

#endif