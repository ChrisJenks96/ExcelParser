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
		buffer = new char[fileSize];//(char*)malloc(fileSize);
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
		memcpy(numStr, countOffset, countEnd);
		numSharedStr = atoi(numStr) - 1;
		//the number of strings to load
		sharedStr = new char*[numSharedStr];//(char**)malloc(numSharedStr);

		//not used anymore
		countOffset = NULL;

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
			sharedStr[i] = new char[countEnd + 1];//(char*)malloc(countEnd+1);
			memcpy(sharedStr[i], sharedStrDataOffset, countEnd);
			//add null terminator to the string
			sharedStr[i][countEnd] = 0;
			//offset for the next data
			sharedStrDataOffset += (16 + countEnd);
			countEnd = 0;
		}

		//not used anymore
		sharedStrDataOffset = NULL;

		delete buffer;//free(buffer);
		fclose(in);
		return true;
	}

	return false;
}

void XMLSharedString::Destroy()
{
	for (int i = 0; i < numSharedStr; i++){
		delete sharedStr[i];//free(sharedStr[i]);
	}
}

bool XMLWorksheet::Load(const char* fn)
{
	FILE* in = fopen(fn, "rb");
	if (in)
	{
		char* buffer;
		fseek(in, 0, SEEK_END);
		int fileSize = ftell(in);
		fseek(in, 0, SEEK_SET);
		buffer = new char[fileSize];//(char*)malloc(fileSize);
		fread(&buffer[0], fileSize, 1, in);

		numCells = 0;
		//find the remaining quotation and that will be our offset
		char* bufferOffset = buffer;
		while (bufferOffset != NULL) {
			bufferOffset = strstr(bufferOffset, "c r");
			//+1 so we don't get into inf loop
			if (bufferOffset != NULL) {
				bufferOffset += 1;
				numCells++;
			}

			else
				break;
		}

		if (numCells > 0)
		{
			isEmptyFlag = false;
			//faster than reallocing every loop/allocating a big number then resizing at the end (excel data size is too variable)
			int currCell = 0;
			cells = new XMLWORKSHEET_CELL[numCells];//(XMLWORKSHEET_CELL*)malloc(sizeof(XMLWORKSHEET_CELL) * numCells);

			//find the actual cell ID and value
			bufferOffset = buffer;
			int countEnd = 0;
			for (int i = 0; i < numCells; i++)
			{
				bufferOffset = strstr(bufferOffset, "c r");
				if (bufferOffset != NULL) {
					//skip past XML formatting data
					bufferOffset += 5;
					//find the end of the shared string by finding the XML syntax '<'
					while (bufferOffset[countEnd] != '>' && bufferOffset[countEnd] != 't') {
						countEnd++;
					}

					//we go 1/2 characters further to find whether the buffer has a 't' (string constant reference id)
					if (bufferOffset[countEnd] == '>') {
						cells[currCell].strRefFlag = false;
						countEnd -= 1;
					}

					else if (bufferOffset[countEnd] == 't') {
						cells[currCell].strRefFlag = true;
						countEnd -= 2;
					}

					//get the cell id and zero it off
					cells[currCell].name = new char[countEnd + 1];//(char*)malloc(countEnd + 1);
					memcpy(cells[currCell].name, bufferOffset, countEnd);
					cells[currCell].name[countEnd] = 0;

					//find the value inside the cell
					bufferOffset += countEnd;
					while (*bufferOffset != 'v') {
						bufferOffset++;
					}

					//offset +2 to get to the start of the data
					bufferOffset += 2;

					//find the end of the data we're searching for
					countEnd = 0;
					while (bufferOffset[countEnd] != '<') {
						countEnd++;
					}

					char numStr[16];
					memcpy(numStr, bufferOffset, countEnd);
					numStr[countEnd] = 0;
					cells[currCell].value = atoi(numStr);

					//increment to the next cell
					currCell += 1;
				}
			}
		}

		else
			isEmptyFlag = true;

		delete buffer;//free(buffer);
		fclose(in);
		return true;
	}

	return false;
}

void XMLWorksheet::Destroy()
{
	for (int i = 0; i < numCells; i++){
		delete cells[i].name;//free(cells[i].name);
	}

	delete[] cells;//free(cells);
}