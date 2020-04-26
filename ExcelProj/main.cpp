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
	if (xlsxReader.Load("dictionary.xlsx", numWorksheets))
	{
		xmlSSReader.Load();
		for (int i = 0; i < numWorksheets; i++) {
			xmlWSReader[i].Load(xlsxReader.GetWorksheetName(i));
		}

		double finalCount = pc.GetCounter();

		//load in each worksheet
		//tell the end user the processing time of the excel file
		FILE* out = fopen("LOG.TXT", "a");
		for (int i = 0; i < numWorksheets; i++){			
			fprintf(out, "Worksheet[%i]: Cell count: %i\n", i, xmlWSReader[i].GetNumCells());
		}

		fprintf(out, "Excel Spreadsheet processing time: %f Milliseconds\n", finalCount);
		fclose(out);
	}

	for (int i = 0; i < numWorksheets; i++) {
		if (!xmlWSReader[i].isEmpty()) {
			xmlWSReader[i].Destroy();
		}
	}

	delete[] xmlWSReader;
	xmlSSReader.Destroy();
	xlsxReader.Destroy();

	return 0;
}