#include <Windows.h>
#include "XLSX.h"

//two main important types of files are:
	//-sheets which contain raw data and string references
	//-sharedStrings which contain the raw strings which the sheets reference via indexes

int main(int argc, char** argv)
{
	XLSX xlsxReader;
	xlsxReader.Load("C:\\Users\\Chris\\Desktop\\EXCELBOOKTEST.xlsx", 3);
	xlsxReader.Destroy();
	return 0;
}