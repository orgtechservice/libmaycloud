#ifndef RANDOM_STRING_H
#define RANDOM_STRING_H

#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace nanosoft
{
	std::string generateBase62ID(uint64_t id);
	std::string generateSalt(int len);
	std::string generateRandomString(uint64_t id, int salt_len);
}

#endif // RANDOM_STRING_H