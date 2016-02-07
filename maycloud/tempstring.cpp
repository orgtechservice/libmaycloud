#include <maycloud/tempstring.h>

void TempString::printf(const char* fmt,...)
{
	va_list args;
	va_start(args, fmt);
	
	vsnprintf(buff, buffer_size, fmt, args);
	
	va_end(args);
}

TempString TempString::operator + (const char ch)
{
	int len = strlen(buff);
	if(len != buffer_size - 1)
	{
		buff[len] = ch;
		buff[len + 1] = 0;
	}
	
	return *this;
}