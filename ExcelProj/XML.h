#ifndef XML_H
#define XML_H

#include <stdio.h>
#include <malloc.h>
#include <string>

class XML
{
	public:
		XML() {}
		virtual bool Load() = 0;
		virtual void Destroy() = 0;
		~XML() {}
	private:
};

class XMLSharedString : public XML
{
	public:
		bool Load();
		void Destroy();
	private:
		int numSharedStr;
		char** sharedStr;
};

typedef struct XMLWORKSHEET_CELL
{
	//if the cell t="s" then it references a string constant via index
	bool strRefFlag;
	//e.g. "A1"
	char* name;
	//index to constant string or actual value
	int value; 
} XMLWORKSHEET_CELL;

class XMLWorksheet : public XML
{
	public:
		bool Load() { return false; }
		bool Load(const char* fn);
		bool isEmpty() { return isEmptyFlag; }
		void Destroy();
	private:
		bool isEmptyFlag;
		int numCells;
		XMLWORKSHEET_CELL* cells;
};



#endif