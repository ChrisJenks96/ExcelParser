#include "XLSX.h"

bool XLSX::Load(const char* fn, int numWorksheets)
{
	FILE* in = fopen(fn, "rb");
	if (in)
	{
		this->numWorksheets = numWorksheets;
		//we need to log and save the names of the worksheet names for loading them later
		int currWorksheetName = 0;
		worksheetNames = new char*[numWorksheets];//(char**)malloc(numWorksheets);
		int currWorksheet = -1;
		XLSX_DATA tmpZip;
		//used if we dump the files using 7-ZIP
		int systemRet;

		while (!feof(in) && currWorksheet < numWorksheets)
		{
			fread(&tmpZip, sizeof(ZIP_HEADER), 1, in);
			tmpZip.filename = new char[tmpZip.zipHeader.fileNameLength + 1];//(char*)malloc(tmpZip.zipHeader.fileNameLength + 1);
			fread(&tmpZip.filename[0], tmpZip.zipHeader.fileNameLength, 1, in);
			//add null terminator to our string to remove garbage
			tmpZip.filename[tmpZip.zipHeader.fileNameLength] = 0;

			bool xmlFound = false;
			if (strstr(tmpZip.filename, "xl/worksheets/") != NULL) {
				//alloc space for the name but remove the "xl/worksheets/" (14 characters)
				int filenameNewLength = tmpZip.zipHeader.fileNameLength;
				//this accounts for filenames with directory paths, work backwards to find the last subdirectory slash
				while (tmpZip.filename[filenameNewLength] != '/'){
					filenameNewLength--;
				}

				//add 1 as we dont want to include the '/' we have been searching for
				filenameNewLength += 1;

				//find out if theres another extension after xml
				char* xmlExtensionCheck = strstr(tmpZip.filename, "xml");
				int xmlExtensionCheckSize = strlen(xmlExtensionCheck);
				//if its not equal to 3 cxharacters (xml) then we need to cut it down
				if (xmlExtensionCheckSize != 3) {
					xmlExtensionCheckSize = (xmlExtensionCheckSize - 3);
				}
				//no need for an offset if it's already got the 3 character xml extension
				else
					xmlExtensionCheckSize = 0;

				worksheetNames[currWorksheetName] = new char[(tmpZip.zipHeader.fileNameLength - (filenameNewLength + xmlExtensionCheckSize)) + 1];//(char*)malloc((tmpZip.zipHeader.fileNameLength - 14) + 1);
				memcpy(worksheetNames[currWorksheetName], &tmpZip.filename[filenameNewLength], (tmpZip.zipHeader.fileNameLength - (filenameNewLength + xmlExtensionCheckSize)));
				worksheetNames[currWorksheetName][tmpZip.zipHeader.fileNameLength - (filenameNewLength + xmlExtensionCheckSize)] = 0;
				currWorksheetName++;
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
				char* buffer = new char[dataZipSize];//(char*)malloc(dataZipSize);
				fread(&buffer[0], dataZipSize, 1, in);
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
					
					currWorksheet++;
				}

				else
					printf("dir error: %s does not exist\n", tmpZip.filename);

				delete buffer;//free(buffer);
			}

			//if this isn't the file we're looking for, keep searching
			else
			{
				//forward the seek pointer past the data (we've done the filename)
				fseek(in, tmpZip.zipHeader.compressedSize + tmpZip.zipHeader.extraFieldLength, SEEK_CUR);
			}

			delete tmpZip.filename;//free(tmpZip.filename);
		}

		fclose(in);
		return true;
	}

	return false;
}

void XLSX::Destroy()
{
	for (int i = 0; i < numWorksheets; i++){
		delete worksheetNames[i];//free(worksheetNames[i]);
	}
}