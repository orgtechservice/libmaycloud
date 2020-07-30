#ifndef LIBMAYCLOUD_IPV6_H
#define LIBMAYCLOUD_IPV6_H

#include <bitset>
#include <utility>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <maycloud/macaddress.h>

class IPv6 {
    std::vector<uint8_t> ipv6_;
public:

    IPv6() { ipv6_.resize(16); }

    explicit IPv6(std::vector<uint8_t> v) : ipv6_(std::move(v)) {};

    explicit IPv6(void *buf);

    explicit IPv6(const std::string &str);

    inline bool operator==(const IPv6 &ipv6) const { return this->ipv6_ == ipv6.ipv6_; }

    inline bool operator!=(const IPv6 &ipv6) const { return this->ipv6_ != ipv6.ipv6_; }

    int operator[](unsigned int byte_number) const;

    /*
     * Вернуть IPv6 в виде строки в короткой форме маленькими буквами
     */
    std::string toShortString();

    /**
     * Вернуть IPv6 в vector байтов
     * @return vector байтов
     */
    inline std::vector<uint8_t> toVector() { return ipv6_; }

    /**
     * Преобразовать IPv6 через EUI-64
     * @param mac48 экземпляр MACAddress
     */
    void makeEui64(const MACAddress &mac48);

    /**
     * Преобразовать IPv6 через EUI-64
     * @param mac_string mac-48 в виде строки
     */
    void makeEui64(const std::string &mac_string);

    /**
     * Преобразует IPv6 в короткую форму и маленкьие буквы
     * @param ipv6_string
     */
    static void convertIpv6ShortForm(std::string &ipv6_string);
};

#endif //LIBMAYCLOUD_IPV6_H
















