/**
 * Configurer
 * Revision on 19.03.2015
 * 
 * (c) Michael Solomatin
 */

#include <maycloud/configurer.h>

Configurer::Configurer()
{
	
}

Configurer::~Configurer()
{
	
}

bool Configurer::parseParam(std::string param)
{
	std::string paramName;
	std::string paramValue;
	
	int i = 0;
	while(param[i] != 0 && param[i] != '\n' && param[i] != '=')
	{
		paramName = paramName + param[i];
		i++;
	}
	i++;
	if(param[i] != '\'') return false;
	i++;
	while(param[i] != 0 && param[i] != '\n' && param[i] != '\'')
	{
		paramValue = paramValue + param[i];
		i++;
	}
	
	params[paramName] = paramValue;
	return true;
}

bool Configurer::loadFile(const char* fn)
{
	params.erase(params.begin(), params.end());
	
	file.open(fn, std::ifstream::in);
	
	char param[256];
	file.getline(param, 256);
	while(file.good())
	{
		printf("[Configurer]: %s\n", param);
		parseParam(param);
		file.getline(param, 256);
	}
	
	file.close();
}

std::string Configurer::searchParam(std::string name)
{
	std::string value;
	std::map<std::string, std::string>::iterator it = params.find(name);
	if(it != params.end()) value = it->second;
	return value;
}

std::string Configurer::searchParam(std::string name, std::string default_value)
{
	std::string value(default_value);
	std::map<std::string, std::string>::iterator it = params.find(name);
	if(it != params.end()) value = it->second;
	return value;
}
