#ifndef FAST_H
#define FAST_H

//https://stackoverflow.com/questions/16826422/c-most-efficient-way-to-convert-string-to-int-faster-than-atoi
static int fast_atoi(const char * str)
{
	//e.g. 130
	/*val = 0 * 10 + ('1'49 - 48) = 0 * 10 + (1) = 1
	val = 1 * 10 + ('3'51 - 48) = 1 * 10 + (3) = 13
	val = 13 * 10 + ('0'48 - 40) = 13 * 10 + (0) = 130*/
	int val = 0;
	while (*str) {
		//48 = '0'
		val = val * 10 + (*str++ - 48);
	}

	return val;
}

#endif
