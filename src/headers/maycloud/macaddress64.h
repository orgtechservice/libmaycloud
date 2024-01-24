#ifndef MACADRESS_64_H
#define MACADRESS_64_H

#include <maycloud/iputils.h>
#include <maycloud/tempstring.h>

#include <string.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstdint>

class MACAddress64
{
private:
	unsigned char mac[8];
public:
	inline MACAddress64() { memset(mac, 0, sizeof(mac)); }
	inline MACAddress64(const MACAddress64& b) { memcpy(mac, b.mac, sizeof(mac)); }
	inline MACAddress64(const char* b) { parse_mac8_str(b, mac); }
    explicit MACAddress64(const std::vector<u_int8_t> & v);
	
	MACAddress64(unsigned char b1, 
		     unsigned char b2,
		     unsigned char b3,
		     unsigned char b4,
		     unsigned char b5,
		     unsigned char b6,
		     unsigned char b7,
		     unsigned char b8);
	
	unsigned char operator [] (unsigned int i) const;
	inline MACAddress64 operator = (MACAddress64 ma) { memcpy(mac, ma.mac, sizeof(mac)); return *this; }
	inline MACAddress64 operator = (const unsigned char* newmac) { memcpy(mac, newmac, sizeof(mac)); return *this; }
	inline bool operator == (MACAddress64 ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) == 0;
	}
	inline bool operator != (MACAddress64 ma) const
	{ 
		return memcmp(mac, ma.mac, sizeof(mac)) != 0;
	}
	inline bool operator > (MACAddress64 ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) > 0;
	}
	inline bool operator >= (MACAddress64 ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) >= 0;
	}
	inline bool operator < (MACAddress64 ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) < 0;
	}
	inline bool operator <= (MACAddress64 ma) const
	{
		return memcmp(mac, ma.mac, sizeof(mac)) <= 0;
	}
	inline operator unsigned long long int () { return parse_mac_8(mac); }

    TempString toString();

	/**
	 * Возвращает MAC64 в короткой форме маленькими буквами
	 */
    std::string toShortString();
};

#endif // MACADRESS_64_H
