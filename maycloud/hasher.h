#ifndef HASHER_H
#define HASHER_H

#include <stdio.h>

class Hasher
{
protected:
	size_t hash;
	size_t module;
public:
	inline Hasher() { }
	inline Hasher(size_t newmodule) { hash = 0; module = newmodule; }
	inline Hasher(const Hasher& b) { hash = b.hash; module = b.module; }
	
	void feedChar(unsigned char c);
	void feedData(void* data, size_t length);
	void feedInt(unsigned int i);
	void feedLong(unsigned long i);
	void feedLongLong(unsigned long long i);
	
	inline size_t result() { return hash; }
	
	inline void setModule(size_t newmodule) { module = newmodule; }
	inline void reset() { hash = 0; }
};

#endif // HASHER_H
