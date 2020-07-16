#include <maycloud/macaddress.h>

MACAddress::MACAddress(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4, unsigned char b5, unsigned char b6)
{
	mac[0] = b1;
	mac[1] = b2;
	mac[2] = b3;
	mac[3] = b4;
	mac[4] = b5;
	mac[5] = b6;
}

MACAddress::MACAddress(const std::vector<u_int8_t> &vector) {
    int size = std::min(6, (int) vector.size());
    for (int i = 0; i < size; ++i) {
        mac[i] = vector[i];
    }
}

unsigned char MACAddress::operator [] (unsigned int i) const
{
	if(i < 6) return mac[i];
	else return 0;
}

TempString MACAddress::toString() const
{
	TempString str;
	str.printf("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return str;
}

std::string MACAddress::toShortString() const {
    char str[18];
    sprintf(str, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return str;
}

std::vector<u_int8_t> MACAddress::getEui64Vector() const {
    std::vector<u_int8_t> eui64(8);
    for (int i = 0; i < 3; ++i) {
        eui64[i] = mac[i];
    }

    eui64[3] = 0xff;
    eui64[4] = 0xfe;

    for (int i = 3; i < 6; ++i) {
        eui64[i + 2] = mac[i];
    }

    eui64[0] ^= 64u;

    return eui64;
}

MACAddress64 MACAddress::getEui64Mac64() const {
    MACAddress64 mac64(mac[0], mac[1], mac[2], 0xff, 0xfe, mac[3], mac[4], mac[5]);
    return mac64;
}

void MACAddress::convertMacShortForm(std::string &mac_string) {
    MACAddress mac_obj(mac_string.c_str());
    mac_string = mac_obj.toShortString();
}
