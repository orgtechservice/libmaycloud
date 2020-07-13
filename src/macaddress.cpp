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

TempString MACAddress::toShortString() const {
    TempString str;
    str.printf("%X:%X:%X:%X:%X:%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return str;
}

std::bitset<64> MACAddress::toEui64() {
    std::bitset<64> bs;

    for (unsigned int i = 0; i < 3; ++i) {
        bs |= ((uint64_t) mac[i]) << (i * 8);
    }

    bs |= (0xFFFEull << 24u);

    for (unsigned int i = 3; i < 5; ++i) {
        bs |= ((uint64_t) mac[i]) << (i * 8 + 16);
    }

    bs.flip(6);

    return bs;
}