#include <maycloud/hasher.h>

void Hasher::feedChar(unsigned char c)
{
	hash = (hash * 256 + c) % module;
}

void Hasher::feedData(void* data, size_t length)
{
	unsigned char *s = static_cast<unsigned char *>(data);
	while ( length-- ) feedChar(*s++);
}

void Hasher::feedInt(unsigned int i)
{
	feedData(&i, sizeof(i));
}

void Hasher::feedLong(unsigned long int i)
{
	feedData(&i, sizeof(i));
}

void Hasher::feedLongLong(unsigned long long i)
{
	feedData(&i, sizeof(i));
}
