#include "XML.h"

bool XMLSharedString::Load()
{
	FILE* in = fopen("sharedStrings.XML", "rb");
	if (in)
	{
		char numStr[8];
		char* buffer;
		fseek(in, 0, SEEK_END);
		int fileSize = ftell(in);
		fseek(in, 0, SEEK_SET);
		buffer = (char*)malloc(fileSize);
		fread(&buffer[0], fileSize, 1, in);

		//find the number of strings in the XML file
		//+7 is the offset to get to the start of the number
		char* countOffset = strstr(buffer, "count=") + 7;
		int countEnd = 0;
		//find the remaining quotation and that will be our offset
		while (countOffset[countEnd] != '"'){
			countEnd++;
		}

		//convert the string to num
		memcpy(&numStr[0], &countOffset[0], countEnd);
		numSharedStr = atoi(numStr) - 1;
		//the number of strings to load
		sharedStr = (char**)malloc(numSharedStr);

		//offset to the start of the shared string data
		char* sharedStrDataOffset = strstr(buffer, "<si><t>") + 7;
		countEnd = 0;
		for (int i = 0; i < numSharedStr; i++)
		{
			//find the end of the shared string by finding the XML syntax '<'
			while (sharedStrDataOffset[countEnd] != '<') {
				countEnd++;
			}

			//allocate the new string size and fill it up
			sharedStr[i] = (char*)malloc(countEnd+1);
			memcpy(&sharedStr[i][0], &sharedStrDataOffset[0], countEnd);
			//add null terminator to the string
			sharedStr[i][countEnd] = 0;
			//offset for the next data
			sharedStrDataOffset += (16 + countEnd);
			countEnd = 0;
		}

		fclose(in);
		return true;
	}

	return false;
}

void XMLSharedString::Destroy()
{
	for (int i = 0; i < numSharedStr; i++){
		free(sharedStr[i]);
	}

	sharedStr = NULL;
}

bool XMLWorksheet::Load()
{
	return true;
}

void XMLWorksheet::Destroy()
{

}