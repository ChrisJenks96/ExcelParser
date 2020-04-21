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

class XMLWorksheet : public XML
{
	public:
		bool Load();
		void Destroy();
	private:
};



#endif