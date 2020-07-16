#include "maycloud/ipv6.h"


IPv6::IPv6(void *buf) {
    ipv6_.resize(16);
    for (int i = 0; i < 16; ++i) {
        ipv6_[i] = ((u_int8_t *) buf)[i];
    }
}

IPv6::IPv6(const std::string &str) {
    ipv6_.resize(16);
    inet_pton(AF_INET6, str.c_str(), ipv6_.data());
}

int IPv6::operator[](unsigned int byte_number) const {
    if (byte_number >= 0 && byte_number < 16) {
        return this->ipv6_[byte_number];
    }
    return 0;
}

std::string IPv6::toShortString() {
    char buf[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, ipv6_.data(), buf, INET6_ADDRSTRLEN);
    return buf;
}

void IPv6::makeEui64(const MACAddress &mac48) {
    std::vector<u_int8_t> eui64 = mac48.getEui64Vector();
    for (int i = 8; i < 16; ++i) {
        ipv6_[i] = eui64[i - 8];
    }
}

void IPv6::makeEui64(const std::string &mac_string) {
    MACAddress mac48(mac_string.c_str());
    makeEui64(mac48);
}

void IPv6::convertIpv6ShortForm(std::string &ipv6_string) {
    IPv6 ipv6_obj(ipv6_string);
    ipv6_string = ipv6_obj.toShortString();
}
