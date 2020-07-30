#include <maycloud/macaddress64.h>
#include <string>

MACAddress64::MACAddress64(unsigned char b1,
			   unsigned char b2,
			   unsigned char b3,
			   unsigned char b4,
			   unsigned char b5,
			   unsigned char b6,
			   unsigned char b7,
			   unsigned char b8)
{
	mac[0] = b1;
	mac[1] = b2;
	mac[2] = b3;
	mac[3] = b4;
	mac[4] = b5;
	mac[5] = b6;
	mac[6] = b7;
	mac[7] = b8;
}

MACAddress64::MACAddress64(const std::vector<uint8_t> &v) {
    int size = std::min(8, (int) v.size());
    for (int i = 0; i < size; ++i) {
        mac[i] = v[i];
    }
}

unsigned char MACAddress64::operator [] (unsigned int i) const
{
	if(i < 8) return mac[i];
	else return 0;
}

TempString MACAddress64::toString()
{
	TempString str;
	str.printf("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
	return str;
}

std::string MACAddress64::toShortString() {
    char str[24];
    sprintf(str, "%x:%x:%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]);
    return str;
}
