#include <Windows.h>
#include "XLSX.h"
#include "XML.h"

//two main important types of files are:
	//-sheets which contain raw data and string references
	//-sharedStrings which contain the raw strings which the sheets reference via indexes

int main(int argc, char** argv)
{
	XLSX xlsxReader;
	XMLSharedString xmlSSReader;
	XMLWorksheet xmlWSReader;
	xlsxReader.Load("C:\\Users\\Chris\\Desktop\\EXCELBOOKTEST.xlsx", 3);
	xlsxReader.Destroy();
	//once the xlsx reader has uncompressed all the files, they will be on the root, so we know the file loc
	xmlSSReader.Load();
	xmlSSReader.Destroy();
	return 0;
}