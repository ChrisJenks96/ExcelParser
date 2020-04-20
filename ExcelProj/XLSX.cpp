#include "XLSX.h"

bool XLSX::Load(const char* fn, bool dumpFiles)
{
	FILE* in = fopen(fn, "rb");
	if (in)
	{
		//used if we dump the files using 7-ZIP
		int systemRet;
		fread(&header, sizeof(ZIP_HEADER), 1, in);
		header.filename = (char*)malloc(header.zipHeader.fileNameLength + 1);
		fread(&header.filename[0], header.zipHeader.fileNameLength, 1, in);
		//add null terminator to our string to remove garbage
		header.filename[header.zipHeader.fileNameLength] = 0;
		if (dumpFiles)
		{
			//reset seek pointer back to the start
			fseek(in, 0, SEEK_SET);
			//get the size of our nested zip inside the XLSX file
			int dataZipSize = sizeof(ZIP_HEADER) + header.zipHeader.fileNameLength + 
				header.zipHeader.compressedSize + header.zipHeader.extraFieldLength;
			//create and read the whole zip file to the buffer
			char* buffer = (char*)malloc(dataZipSize);
			fread(&buffer[0], dataZipSize, 1, in);
			//change the extension from XML to ZIP (precautionary for 7-ZIP)
			int headerFilenameLength = strlen(header.filename);
			header.filename[headerFilenameLength - 3] = 'z';
			header.filename[headerFilenameLength - 2] = 'i';
			header.filename[headerFilenameLength - 1] = 'p';
			//write the buffer externally for 7-zip to reference
			FILE* out = fopen(header.filename, "wb");
			fwrite(&buffer[0], dataZipSize, 1, out);
			fclose(out);

			//uncompress the zip file, uncompressing the XML file inside of it
			char zipExtractCommand[64];
			//Based on the fact we have 7-Zip installed (64 bit version on windows)
			strcpy(&zipExtractCommand[0], "C:\\PROGRA~1\\7-ZIP\\7Z.EXE e ");
			strcpy(&zipExtractCommand[strlen(zipExtractCommand)], header.filename);
			systemRet = system(zipExtractCommand);
		}

		fclose(in);
		return (systemRet < 0) ? false : true;
	}

	return false;
}

void XLSX::Destroy()
{
	free(header.filename);
}