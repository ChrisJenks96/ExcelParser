#include <Windows.h>
#include "XLSX.h"
#include "XML.h"
#include "Time.h"

//note: new allocates memory and calls constructor for object initialization

int main(int argc, char** argv)
{
	PerformanceCounter pc;
	XLSX xlsxReader;
	XMLSharedString xmlSSReader;
	int numWorksheets = 1;
	XMLWorksheet* xmlWSReader = new XMLWorksheet[numWorksheets];

	pc.StartCounter();

	//once the xlsx reader has uncompressed all the files, they will be on the root, so we know the file loc
	if (xlsxReader.Load("whostat2005_mortality.xlsx", numWorksheets)){
		//load in the shared strings
		if (xmlSSReader.Load())
		{	
			//load in each worksheet
			//tell the end user the processing time of the excel file
			FILE* out = fopen("LOG.TXT", "wb");
			for (int i = 0; i < numWorksheets; i++){
				xmlWSReader[i].Load((const char*)xlsxReader.GetWorksheetName(i));
				if (out) {
					fprintf(out, "Worksheet[%i]: Cell count: %i\n", i, xmlWSReader[i].GetNumCells());
				}
			}

			fprintf(out, "Excel Spreadsheet processing time: %f Milliseconds", pc.GetCounter());
			fclose(out);
		}
	}

	for (int i = 0; i < numWorksheets; i++) {
		if (!xmlWSReader[i].isEmpty()) {
			xmlWSReader[i].Destroy();
		}

		//remove the file (7-zip asks us for overwriting in cmd which is sooooo slow)
		remove(xlsxReader.GetWorksheetName(i));
	}

	//this will always be a constant
	remove("sharedStrings.xml");

	delete[] xmlWSReader;
	xmlSSReader.Destroy();
	xlsxReader.Destroy();

	return 0;
}