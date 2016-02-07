#ifndef IPUTILS_H
#define IPUTILS_H

#include <stdio.h>
#include <string.h>

/**
* Парсинг IPv4-адреса
*/
unsigned int parse_ip(const char *ip);

/**
 * Парсинг мас-адреса, представленного в виде массива из шести байт
 */
unsigned long long int parse_mac_6(const unsigned char* mac);

/**
 * Парсинг мас-адреса, представленного в виде строки, в последовательность из шести байт
 */
bool parse_mac_str(const char* mac, unsigned char* rez_mac);

inline unsigned int make_ip(const unsigned char ip[4])
{
	return (ip[0] & 0xFF) * 0x1000000 + (ip[1] & 0xFF) * 0x10000 + (ip[2] & 0xFF) * 0x100 + (ip[3] & 0xFF);
}

inline unsigned int make_ip(unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4)
{
	return ((a1*256+a2)*256+a3)*256+a4;
}

#endif // IPUTILS_H
