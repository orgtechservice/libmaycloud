#ifndef IPV4_H
#define IPV4_H

#include <maycloud/iputils.h>
#include <maycloud/tempstring.h>

class IPv4
{
private:
	unsigned int ip;
public:
	inline IPv4():ip(0) { }
	inline IPv4(const IPv4& b):ip(b.ip) { }
	inline IPv4(unsigned int b):ip(b) { }
	inline IPv4(const char* b):ip(parse_ip(b)) { }
	inline IPv4(unsigned int b1, unsigned int b2, unsigned int b3, unsigned int b4):ip(make_ip(b1, b2, b3, b4)) { }
	
	unsigned int operator [] (unsigned int i) const;
	inline operator unsigned int () const { return ip; }
	
	inline IPv4 operator = (IPv4 ipv4) { ip = ipv4.ip; return *this; }
	inline IPv4 operator = (unsigned int newip) { ip = newip; return *this; }
	
	inline IPv4 operator & (const IPv4& mask) { return ip & mask; }
	inline IPv4 operator ~ () { return ~ip; }
	
	inline bool operator == (IPv4 ipv4) const { return ip == ipv4.ip; } 
	inline bool operator != (IPv4 ipv4) const { return ip != ipv4.ip; }
	inline bool operator < (const IPv4& ipv4) { return ip < ipv4.ip; }
	inline bool operator <= (const IPv4& ipv4) { return ip <= ipv4.ip; }
	inline bool operator > (const IPv4& ipv4) { return ip > ipv4.ip; }
	inline bool operator >= (const IPv4& ipv4) { return ip >= ipv4.ip; }
	
	
	TempString toString();
};

#endif // IPV4_H
