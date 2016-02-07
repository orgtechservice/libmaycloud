#include <maycloud/randomstring.h>

namespace nanosoft
{
	std::string generateBase62ID(uint64_t id)
	{
		uint32_t id32 = (uint32_t) (id & 0x00000000FFFFFFFF);
		
		std::string ret;
		
		for(int i = 0; i < 6; i++)
		{
			char ch;
			char dig = (char) (id32 % 62);
			id32 /= 62;
			if(dig < 26) ch = 'a' + dig;
			else if(dig < 52) ch = 'A' + dig - 26;
			else ch = '0' + dig - 52;
			
			ret += ch;
		}
		
		return ret;
	}
	
	std::string generateSalt(int len)
	{
		char str[len + 1];
		str[len] = 0;
		
		for(int i = 0; i < len; i++)
		{
			while(1)
			{
				str[i] = (char) (rand() % 'z' + '0');
				
				if(str[i] >= '0' && str[i] <= '9') break;
				if(str[i] >= 'A' && str[i] <= 'Z') break;
				if(str[i] >= 'a' && str[i] <= 'z') break;
			}
		}
		
		return std::string(str);
	}

	std::string generateRandomString(uint64_t id, int salt_len)
	{
		std::string salt = generateSalt(salt_len);
		std::string szid = generateBase62ID(id);
		return szid + salt;
	}
}