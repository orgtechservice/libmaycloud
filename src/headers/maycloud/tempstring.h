#ifndef TEMP_STRING_H
#define TEMP_STRING_H

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

class TempString
{
protected:
	static const int buffer_size = 1024;
	char buff[buffer_size];
public:
	inline TempString() { buff[0] = 0; }
	inline TempString(const TempString& b) { strncpy(buff, b.buff, buffer_size - 1); buff[buffer_size - 1] = 0; }
	inline TempString(const char* str) { strncpy(buff, str, buffer_size - 1); buff[buffer_size - 1] = 0; }
	
	char* c_str() { return buff; }
	void printf(const char* fmt,...);
	
	inline TempString operator = (TempString b) { strncpy(buff, b.buff, buffer_size - 1); buff[buffer_size - 1] = 0; return *this; }
	inline TempString operator + (TempString b) { strncat(buff, b.buff, buffer_size - 1); buff[buffer_size - 1] = 0; return *this; }
	TempString operator + (const char ch);
	
	inline bool operator == (TempString b) { return strncmp(buff, b.buff, buffer_size - 1) == 0; }
	inline bool operator != (TempString b) { return strncmp(buff, b.buff, buffer_size - 1) != 0; }
	
	inline operator bool () { return buff[0] != 0; }
};

#endif // TEMP_STRING_H
