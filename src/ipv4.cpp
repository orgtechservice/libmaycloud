#include <maycloud/ipv4.h>

unsigned int IPv4::operator [] (unsigned int i) const
{
	switch(i)
	{
		case 0:
			return (ip / 0x1000000) % 256;
		case 1:
			return (ip / 0x10000) % 256;
		case 2:
			return (ip / 256) % 256;
		case 3:
			return ip % 256;
		default:
			return 0;
	}
}

TempString IPv4::toString()
{
	TempString str;
	str.printf("%u.%u.%u.%u", (*this)[0], (*this)[1], (*this)[2], (*this)[3]);
	return str;
}
