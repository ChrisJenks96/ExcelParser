#include "XML.h"

void XMLSharedString::GetSharedStr(int startOffset, int startIndex, int lastIndex)
{
	int countEnd = 0;
	char* sharedStrDataOffset = strstr(&buffer[startOffset], "<si><t>") + 7;
	for (int i = startIndex; i < lastIndex; i++)
	{
		//find the end of the shared string by finding the XML syntax '<'
		while (sharedStrDataOffset[countEnd] != '<') {
			countEnd++;
			if (countEnd > XMLSHAREDSTRING_MAXLENGTH)
				break;
		}

		//allocate the new string size and fill it up
		if (countEnd < XMLSHAREDSTRING_MAXLENGTH) {
			sharedStr[i] = new char[countEnd + 1];
			memcpy(sharedStr[i], sharedStrDataOffset, countEnd);
			//add null terminator to the string
			sharedStr[i][countEnd] = 0;
		}

		//if the data has been skewed and is out of range, nullify it
		else
			sharedStr[i] = NULL;

		//offset for the next data
		sharedStrDataOffset += (16 + countEnd);
		countEnd = 0;
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
		int countEnd = 0;
		//find the remaining quotation and that will be our offset
		while (sharedStrDataOffset[countEnd] != '"'){
			countEnd++;
		}

		//convert the string to num
		memcpy(numStr, sharedStrDataOffset, countEnd);
		numSharedStr = fast_atoi(numStr);
		//the number of strings to load
		sharedStr = new char*[numSharedStr];

		//single thread version
		GetSharedStr(0, 0, numSharedStr);

		delete buffer;//free(buffer);
		fclose(in);
		return true;
	}

	return false;
}

void XMLSharedString::Destroy()
{
	for (int i = 0; i < numSharedStr; i++){
		delete sharedStr[i];
	}
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
			int countEnd = 0;
			
			bufferOffset = strstr(bufferOffset, "c r");
			//while we havent hit the next column row data segment, process this current one
			while (currCell != (numCells-1))
			{
				int dataEnd = 0;
				//the column row data
				if (bufferOffset[countEnd] == 'c' && bufferOffset[countEnd+2] == 'r')
				{
					//we now move onto the next cell as we've found some c/r data
					currCell += 1;
					//offset to get to the data
					countEnd += 5;
					//NOTE: quotation marks arent being picked up4
					//find the next ' ' / greater than for the end of the data
					while (bufferOffset[countEnd + dataEnd] != ' ' && bufferOffset[countEnd + dataEnd] != '>') {
						dataEnd++;
					}

					//remove extra formatting after the data
					dataEnd -= 1;

					//get the cell id and zero it off
					memcpy(cells[currCell].name, &bufferOffset[countEnd], dataEnd);
					cells[currCell].name[dataEnd] = 0;

					//skip over the data we've just processed
					countEnd += dataEnd;

					//set this by default, as when we hit 't'/'v' we set it to true
					cells[currCell].strRefFlag = false;
					cells[currCell].value = -1;
				}

				else if (bufferOffset[countEnd] == 's' && bufferOffset[countEnd+1] == '='){
					//offset to pass over this id (not needed)
					countEnd += 6;
				}

				//whether the cell references a shared string
				else if (bufferOffset[countEnd] == 't' && bufferOffset[countEnd+1] == '='){
					//offset to pass over this id (not needed)
					countEnd += 5;
					cells[currCell].strRefFlag = true;
				}

				//the cell value
				else if (bufferOffset[countEnd] == '<' && bufferOffset[countEnd+1] == 'v' && bufferOffset[countEnd+2] == '>')
				{
					//offset to get to the data
					countEnd += 3;

					//find the end of the cell value
					while (bufferOffset[countEnd + dataEnd] != '<') {
						dataEnd++;
					}

					memcpy(numStr, &bufferOffset[countEnd], dataEnd);
					numStr[dataEnd] = 0;
					cells[currCell].value = fast_atoi(numStr);

					//skip over the data we've just processed
					countEnd += dataEnd;
				}

				countEnd++;
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