#include <maycloud/iputils.h>

/**
* Парсинг IPv4-адреса
*/
unsigned int parse_ip(const char *ip)
{
	int bytes[4];
	int r = sscanf(ip, "%d.%d.%d.%d", &bytes[0], &bytes[1], &bytes[2], &bytes[3]);
	if ( r == EOF ) return 0;
	return (bytes[0] & 0xFF) * 0x1000000 + (bytes[1] & 0xFF) * 0x10000 + (bytes[2] & 0xFF) * 0x100 + (bytes[3] & 0xFF);
}

/**
 * Парсинг мас-адреса, представленного в виде строки, в последовательность из шести байт
 */
bool parse_mac_str(const char* mac, unsigned char* rez_mac)
{
	int len = strlen(mac);
	char crez = 0;
	
	if(len < 17) return false;
	
	int i = 0;
	int bc = 0;
	int cc = 0;
	while(i < len && bc < 6)
	{
		if(mac[i] >= 0x30 && mac[i] <= 0x39)
		{
		  crez = (crez << 4) + (mac[i] - 0x30);
		  cc++;
		}
		if(mac[i] >= 0x41 && mac[i] <= 0x46)
		{
		  crez = (crez << 4) + (mac[i] - 0x37);
		  cc++;
		}
		if(mac[i] >= 0x61 && mac[i] <= 0x66)
		{
		  crez = (crez << 4) + (mac[i] - 0x57);
		  cc++;
		}
		
		if(cc == 2)
		{
		    rez_mac[bc] = crez;
		    
		    bc++;
		    cc = 0;
		}
		  
		i++;
	}
	return true;
}

/**
 * Парсинг мас-адреса, представленного в виде массива из шести байт
 */
unsigned long long int parse_mac_6(const unsigned char* mac)
{
	unsigned long long int rez = 0;
	unsigned long long int rez2 = 0;
	
	int ofst = 40;
	
	for(int i = 0; i < 6; i++)
	{
		rez2 = (int) mac[i];
		rez2 <<= ofst;
		rez += rez2;
		ofst -= 8;
	}
	
	return rez;
}