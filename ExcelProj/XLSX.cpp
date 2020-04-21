#include "XLSX.h"

bool XLSX::Load(const char* fn, int numWorksheets)
{
	FILE* in = fopen(fn, "rb");
	if (in)
	{
		this->numWorksheets = numWorksheets;
		XLSX_DATA tmpZip;
		//used if we dump the files using 7-ZIP
		int systemRet;

		while (!feof(in))
		{
			fread(&tmpZip, sizeof(ZIP_HEADER), 1, in);
			tmpZip.filename = (char*)malloc(tmpZip.zipHeader.fileNameLength + 1);
			fread(&tmpZip.filename[0], tmpZip.zipHeader.fileNameLength, 1, in);
			//add null terminator to our string to remove garbage
			tmpZip.filename[tmpZip.zipHeader.fileNameLength] = 0;

			bool xmlFound = false;
			if (strstr(tmpZip.filename, "xl/worksheets/") != NULL) {
				xmlFound = true;
			}

			if (strcmp(tmpZip.filename, "xl/sharedStrings.xml") == 0) {
				xmlFound = true;
			}

			if (xmlFound)
			{
				//get the size of our nested zip inside the XLSX file
				int dataZipSize = tmpZip.zipHeader.compressedSize + tmpZip.zipHeader.extraFieldLength;
				//create and read the whole zip file to the buffer
				char* buffer = (char*)malloc(dataZipSize);
				fread(&buffer[0], dataZipSize, 1, in);
				//change the extension from XML to ZIP (precautionary for 7-ZIP)
				//int headerFilenameLength = strlen(tmpZip.filename);
				//tmpZip.filename[headerFilenameLength - 3] = 'z';
				//tmpZip.filename[headerFilenameLength - 2] = 'i';
				//tmpZip.filename[headerFilenameLength - 1] = 'p';
				//write the buffer externally for 7-zip to reference
				FILE* out = fopen(tmpZip.filename, "wb");
				if (out)
				{
					//write the tmp zip header information to the file first
					fwrite(&tmpZip, sizeof(ZIP_HEADER), 1, out);
					fwrite(&tmpZip.filename[0], tmpZip.zipHeader.fileNameLength, 1, out);
					//write the remaining compressed data
					fwrite(&buffer[0], dataZipSize, 1, out);
					fclose(out);
					//uncompress the zip file, uncompressing the XML file inside of it
					char zipExtractCommand[64];
					//Based on the fact we have 7-Zip installed (64 bit version on windows)
					strcpy(&zipExtractCommand[0], "C:\\PROGRA~1\\7-ZIP\\7Z.EXE e ");
					strcpy(&zipExtractCommand[strlen(zipExtractCommand)], tmpZip.filename);
					systemRet = system(zipExtractCommand);

					//remove the original compressed file
					remove(tmpZip.filename);
				}

				else
					printf("dir error: %s does not exist\n", tmpZip.filename);
				free(buffer);
			}

			//if this isn't the file we're looking for, keep searching
			else
			{
				free(tmpZip.filename);
				//forward the seek pointer past the data (we've done the filename)
				fseek(in, tmpZip.zipHeader.compressedSize + tmpZip.zipHeader.extraFieldLength, SEEK_CUR);
			}
		}

		fclose(in);
		return (systemRet < 0) ? false : true;
	}

	return false;
}

void XLSX::Destroy()
{

}