#ifndef MACADRESS_H
#define MACADRESS_H

#include <maycloud/iputils.h>
#include <maycloud/tempstring.h>
#include <maycloud/macaddress64.h>

#include <string.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstdint>

class MACAddress
{
private:
	unsigned char mac[6];
public:
	inline MACAddress() { memset(mac, 0, sizeof(mac)); }
	inline MACAddress(const MACAddress& b) { memcpy(mac, b.mac, sizeof(mac)); }
	inline MACAddress(const char* b) { parse_mac_str(b, mac); }
	MACAddress(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4, unsigned char b5, unsigned char b6);
	explicit MACAddress(const std::vector<u_int8_t> & vector);

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

	/**
	 * Возвращает MAC в корокткой записи маленькими буквами
	 */
    std::string toShortString() const;

    /**
     * Преобразует MAC-48 в EUI-64
     * @return vector из 8 байтов
     */
    std::vector<u_int8_t> getEui64Vector() const ;

    /**
     * Преобразует MAC-48 в EUI-64
     * @return экземпляр MACAddress64
     */
    MACAddress64 getEui64Mac64() const;

    /**
     * Преобразует IPv6 в короткую форму и маленкьие буквы
     */
    static void convertMacShortForm(std::string &mac_string);

};

#endif // MACADRESS_H
