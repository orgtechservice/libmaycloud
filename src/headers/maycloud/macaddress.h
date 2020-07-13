#ifndef MACADRESS_H
#define MACADRESS_H

#include <maycloud/iputils.h>
#include <maycloud/tempstring.h>

#include <string.h>
#include <string.h>
#include <stdio.h>
#include <bitset>

class MACAddress
{
private:
	unsigned char mac[6];
public:
	inline MACAddress() { memset(mac, 0, sizeof(mac)); }
	inline MACAddress(const MACAddress& b) { memcpy(mac, b.mac, sizeof(mac)); }
	inline MACAddress(const char* b) { parse_mac_str(b, mac); }
	MACAddress(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4, unsigned char b5, unsigned char b6);
	
	unsigned char operator [] (unsigned int i) const;
	inline MACAddress operator = (MACAddress ma) { memcpy(mac, ma.mac, sizeof(mac)); return *this; }
	inline MACAddress operator = (const unsigned char* newmac) { memcpy(mac, newmac, sizeof(mac)); return *this; }
	inline bool operator == (MACAddress ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) == 0;
	}
	inline bool operator != (MACAddress ma) const
	{ 
		return memcmp(mac, ma.mac, sizeof(mac)) != 0;
	}
	inline bool operator > (MACAddress ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) > 0;
	}
	inline bool operator >= (MACAddress ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) >= 0;
	}
	inline bool operator < (MACAddress ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) < 0;
	}
	inline bool operator <= (MACAddress ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) <= 0;
	}
	inline operator unsigned long long int () { return parse_mac_6(mac); }

    TempString toString() const;

    TempString toShortString() const;

    /**
     * Преобразует MAC-48 в EUI-64
     * @return последовательность битов
     */
    std::bitset<64> toEui64();
};

#endif // MACADRESS_H
