#include <Windows.h>
#include "XLSX.h"
#include "XML.h"

//note: new allocates memory and calls constructor for object initialization

int main(int argc, char** argv)
{
	XLSX xlsxReader;
	XMLSharedString xmlSSReader;
	int numWorksheets = 3;
	XMLWorksheet* xmlWSReader = new XMLWorksheet[numWorksheets];

	//once the xlsx reader has uncompressed all the files, they will be on the root, so we know the file loc
	if (xlsxReader.Load("C:\\Users\\Chris\\Desktop\\EXCELBOOKTEST.xlsx", numWorksheets)){
		//load in the shared strings
		if (xmlSSReader.Load())
		{	
			//load in each worksheet
			for (int i = 0; i < numWorksheets; i++){
				xmlWSReader[i].Load((const char*)xlsxReader.GetWorksheetName(i));
			}
		}
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