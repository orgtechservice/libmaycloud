#ifndef CONFIGURER_H
#define CONFIGURER_H

/**
 * Configurer
 * Revision on 19.03.2015
 * 
 * (c) Michael Solomatin
 */

#include <string>
#include <fstream>
#include <map>

class Configurer
{
protected:
	std::ifstream file;
	std::map<std::string, std::string> params;
	
	bool parseParam(std::string param);
	bool FileExists(const std::string& path);

public:
	/**
	 * Конструктор, деструктор
	 */
	Configurer();
	~Configurer();
	
	bool loadFile(const char* fn);
	std::string searchParam(std::string name);
	std::string searchParam(std::string name, std::string default_value);
};

#endif // CONFIGURER_H
