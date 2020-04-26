#ifndef XML_H
#define XML_H

#include <stdio.h>
#include <malloc.h>
#include <string>

#include "Fast.h"

class XML
{
	public:
		XML() {}
		virtual bool Load() = 0;
		virtual void Destroy() = 0;
		~XML() {}
	private:
};

//the length of a string (normally quite small, but must cater for sentences etc..)
#define XMLSHAREDSTRING_MAXLENGTH 128
//the length of a worksheet cell value (normally floating point numbers/large strings get close to this max)
#define XMLWORKSHEETCELL_MAXLENGTH 32
//instead of using new/delete, use a max width of the name (limitation, but its a big number)
#define XMLWORKSHEETCELL_NAMELENGTH 16

typedef struct fixedSharedStr{
	char data[XMLSHAREDSTRING_MAXLENGTH];
} fixedSharedStr;

class XMLSharedString : public XML
{
	public:
		bool Load();
		void Destroy();
	private:
		void GetSharedStr(int startOffset, int startIndex, int lastIndex);
		char* buffer;
		int numSharedStr;
		fixedSharedStr* sharedStr;
};

typedef struct XMLWORKSHEET_CELL
{
	//if the cell t="s" then it references a string constant via index
	bool strRefFlag;
	//e.g. "A1"
	char name[XMLWORKSHEETCELL_NAMELENGTH];
	//index to constant string or actual value
	int value; 
} XMLWORKSHEET_CELL;

class XMLWorksheet : public XML
{
	public:
		bool Load() { return false; }
		bool Load(char* fn);
		bool isEmpty() { return isEmptyFlag; }
		int GetNumCells() { return numCells; }
		void Destroy();
	private:
		bool isEmptyFlag;
		int numCells;
		XMLWORKSHEET_CELL* cells;
};



#endif