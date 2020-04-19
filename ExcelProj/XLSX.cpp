#include "XLSX.h"

bool XLSX::Load(const char* fn)
{
	FILE* in = fopen(fn, "rb");
	if (in)
	{
		fread(&header, sizeof(ZIP_HEADER), 1, in);
		header.filename = (char*)malloc(header.zipHeader.fileNameLength);
		fread(&header.filename[0], header.zipHeader.fileNameLength, 1, in);
		//add null terminator to our string to remove garbage
		header.filename[header.zipHeader.fileNameLength] = 0;
		return true;
	}

	return false;
}

void XLSX::Destroy()
{
	free(header.filename);
	header.filename = NULL;
}