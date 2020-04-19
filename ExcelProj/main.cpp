#include <Windows.h>
#include "XLSX.h"

int main(int argc, char** argv)
{
	XLSX xlsxReader;
	xlsxReader.Load("C:\\Users\\Chris\\Desktop\\EXCELBOOKTEST.xlsx");
	return 0;
}