#include "XML.h"

void XMLSharedString::GetSharedStr(int startOffset, int startIndex, int lastIndex)
{
	uint8_t dataEnd = 0;
	char* sharedStrDataOffset = strstr(&buffer[startOffset], "<si><t>") + 7;
	for (int i = startIndex; i < lastIndex; i++)
	{
		//find the end of the shared string by finding the XML syntax '<'
		while (*sharedStrDataOffset != '<') {
			sharedStr[i].data[dataEnd++] = *sharedStrDataOffset++;
		}

		sharedStr[i].data[dataEnd] = 0;
		//offset for the next data
		sharedStrDataOffset += 16;
		dataEnd = 0;
	}
}

bool XMLSharedString::Load()
{
	FILE* in = fopen("sharedStrings.XML", "rb");
	if (in)
	{
		char numStr[8];
		fseek(in, 0, SEEK_END);
		int fileSize = ftell(in);
		fseek(in, 0, SEEK_SET);

		buffer = new char[fileSize];
		fread(&buffer[0], fileSize, 1, in);

		//find the number of strings in the XML file
		//+13 is the offset to get to the start of the number
		char* sharedStrDataOffset = strstr(buffer, "uniqueCount=") + 13;
		//find the remaining quotation and that will be our offset
		uint8_t dataEnd = 0;
		while (*sharedStrDataOffset != '"'){
			numStr[dataEnd++] = *sharedStrDataOffset++;
		}

		//convert the string to num
		numStr[dataEnd] = 0;
		numSharedStr = fast_atoi(numStr);
		//the number of strings to load
		sharedStr = new fixedSharedStr[numSharedStr];
		//single thread version
		GetSharedStr(0, 0, numSharedStr);
		delete buffer;
		fclose(in);
		return true;
	}

	return false;
}

void XMLSharedString::Destroy()
{
	delete[] sharedStr;
}

bool XMLWorksheet::Load(char* fn)
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
			//we count 1 too many
			numCells -= 1;
			isEmptyFlag = false;
			//faster than reallocing every loop/allocating a big number then resizing at the end (excel data size is too variable)
			int currCell = -1;
			cells = new XMLWORKSHEET_CELL[numCells];
			//used for 'v' values
			char numStr[XMLWORKSHEETCELL_MAXLENGTH];

			//find the actual cell ID and value
			bufferOffset = buffer;			
			bufferOffset = strstr(bufferOffset, "c r");
			//while we havent hit the next column row data segment, process this current one
			uint8_t dataEnd = 0;
			while (bufferOffset != NULL)
			{
				//the column row data
				//if (bufferOffset[countEnd] == 'c' && bufferOffset[countEnd+2] == 'r')
				if (*bufferOffset == 'c' && *(bufferOffset+2) == 'r')
				{
					dataEnd = 0;
					//we now move onto the next cell as we've found some c/r data
					currCell += 1;
					//offset to get to the data
					bufferOffset += 5;//countEnd += 5;
					//get the cell id and zero it off
					while ((*bufferOffset) != 34){
						cells[currCell].name[dataEnd++] = *bufferOffset++;
					}

					cells[currCell].name[dataEnd] = 0;
					//set this by default, as when we hit 't'/'v' we set it to true
					cells[currCell].strRefFlag = false;
					cells[currCell].value = -1;			
					bufferOffset++;
				}

				else if (*bufferOffset == 's' && *(bufferOffset+1) == '='){
					//offset to pass over this id (not needed)
					bufferOffset += 6;
				}

				//whether the cell references a shared string
				else if (*bufferOffset == 't' && *(bufferOffset+1) == '=')
				{
					//offset to pass over this id (not needed)
					bufferOffset += 5;
					cells[currCell].strRefFlag = true;
				}

				//the cell value
				else if (*bufferOffset == '<' && *(bufferOffset+1) == 'v' && *(bufferOffset+2) == '>')
				{
					dataEnd = 0;
					//offset to get to the data
					bufferOffset += 3;

					while ((*bufferOffset) != '<') {
						numStr[dataEnd++] = *bufferOffset++;
					}

					numStr[dataEnd] = 0;
					cells[currCell].value = fast_atoi(numStr);

					//skip over the data we've just processed
					bufferOffset += (7 + dataEnd);
				}

				//we shouldnt hit this... means there is extra formatting, we just move to the next round of data
				else
					bufferOffset = strstr(bufferOffset, "c r") - 1;

				bufferOffset++;
			}
		}

		else
			isEmptyFlag = true;

		delete buffer;
		fclose(in);
		return true;
	}

	return false;
}

void XMLWorksheet::Destroy()
{
	delete[] cells;//free(cells);
}