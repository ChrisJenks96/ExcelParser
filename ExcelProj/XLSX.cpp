#include "XLSX.h"

bool XLSX::Load(const char* fn, int numWorksheets)
{
	FILE* in = fopen(fn, "rb");
	if (in)
	{
		this->numWorksheets = numWorksheets;
		//we need to log and save the names of the worksheet names for loading them later
		int currWorksheetName = 0;
		worksheetNames = new char*[numWorksheets];
		int currWorksheet = -1;
		XLSX_DATA tmpZip;
		bool xmlFound = false;

		//NEW MINIZIP/UNZIP METHOD
		// Open the zip file
		unzFile zipfile = unzOpen(fn);
		if (zipfile == NULL){
			printf("%s: not found\n");
			return -1;
		}

		// Get info about the zip file
		unz_global_info global_info;
		if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
		{
			printf("could not read file global info\n");
			unzClose(zipfile);
			return -1;
		}

		// Buffer to hold data read from the zip file.
		char read_buffer[8192];
		// Loop to extract all files
		unsigned long i;
		for (i = 0; i < global_info.number_entry; ++i)
		{
			xmlFound = false;
			// Get info about current file.
			unz_file_info file_info;
			char filename[64];
			//our custom filename accessor
			char* actualFilenameOffset;
			if (unzGetCurrentFileInfo(zipfile, &file_info, 
				filename, 512, NULL, 0, NULL, 0) == UNZ_OK)
			{
				actualFilenameOffset = strstr(filename, "xl/sharedStrings.xml");
				if (actualFilenameOffset != NULL){
					//offset past the 'xl/'
					actualFilenameOffset += 3;
					xmlFound = true;
				}

				if (!xmlFound) {
					actualFilenameOffset = strstr(filename, "xl/worksheets/_rels/");
					if (actualFilenameOffset != NULL) {
						//offset the data offset pointer, so we get to the worksheet name
						actualFilenameOffset += 20;
						//remove the 'rels' on the end if its there
						int len = strlen(actualFilenameOffset)-1;
						//if the last element in filename isn't l for xml
						while (actualFilenameOffset[len-1] != 'm' || actualFilenameOffset[len] != 'l'){
							actualFilenameOffset[len--] = 0;
						}

						currWorksheet++;
						xmlFound = true;

						//add the worksheet name to this class for the worksheet reader
						len = strlen(actualFilenameOffset);
						worksheetNames[currWorksheet] = new char[len+1];
						memcpy(worksheetNames[currWorksheet], actualFilenameOffset, len);
						worksheetNames[currWorksheet][len] = 0;
					}
				}

				if (!xmlFound) {
					actualFilenameOffset = strstr(filename, "xl/worksheets/");
					if (actualFilenameOffset != NULL) {
						//offset the data offset pointer, so we get to the worksheet name
						actualFilenameOffset += 14;
						//remove the 'rels' on the end if its there
						int len = strlen(actualFilenameOffset) - 1;
						//if the last element in filename isn't l for xml
						while (actualFilenameOffset[len - 1] != 'm' || actualFilenameOffset[len] != 'l') {
							actualFilenameOffset[len--] = 0;
						}

						currWorksheet++;
						xmlFound = true;

						//add the worksheet name to this class for the worksheet reader
						len = strlen(actualFilenameOffset);
						worksheetNames[currWorksheet] = new char[len+1];
						memcpy(worksheetNames[currWorksheet], actualFilenameOffset, len);
						worksheetNames[currWorksheet][len] = 0;
					}
				}
			}

			if (currWorksheet >= numWorksheets) {
				break;
			}

			if (xmlFound)
			{
				if (unzOpenCurrentFile(zipfile) != UNZ_OK)
				{
					printf("could not open file\n");
					unzClose(zipfile);
					return -1;
				}

				// Open a file to write out the data.
				FILE* out = fopen(actualFilenameOffset, "wb");
				if (out == NULL)
				{
					printf("could not open destination file\n");
					unzCloseCurrentFile(zipfile);
					unzClose(zipfile);
					return -1;
				}

				int error = UNZ_OK;
				do
				{
					error = unzReadCurrentFile(zipfile, read_buffer, 8192);
					if (error < 0){
						printf("error %d\n", error);
						unzCloseCurrentFile(zipfile);
						unzClose(zipfile);
						return -1;
					}

					// Write data to file.
					if (error > 0){
						fwrite(read_buffer, error, 1, out); // You should check return of fwrite...
					}
				} while (error > 0);

				fclose(out);
				unzCloseCurrentFile(zipfile);
			}

			//move onto the next element in the xml file
			if ((i + 1) < global_info.number_entry){
				if (unzGoToNextFile(zipfile) != UNZ_OK){
					printf("cound not read next file\n");
					unzClose(zipfile);
					return -1;
				}
			}
		}

		unzClose(zipfile);
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