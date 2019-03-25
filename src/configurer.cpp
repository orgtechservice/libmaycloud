/**
 * Configurer
 * 
 * (c) Mikhail Solomatin
 */

#include <maycloud/configurer.h>
#include <maycloud/debug.h>

std::string Configurer::actual_map;
Configurer::params_map Configurer::params;
Configurer::multi_params_map Configurer::local_params;

/**
 * Вывести актуальную конфигурацию
 */
void Configurer::dumpConfig()
{
	for(params_iterator it = params.begin(); it != params.end(); it++)
	{
		printf("[Configurer]: %s=%s\n", it->first.c_str(), it->second.c_str());
	}

	for(multi_params_iterator it = local_params.begin(); it != local_params.end(); it++)
	{
		printf("\n[Configurer]: [%s]\n", it->first.c_str());
		for(params_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			printf("[Configurer]:\t%s=%s\n", it2->first.c_str(), it2->second.c_str());
		}
	}
}

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
	if(param[i] == '#' || param[i] == '\n' || param[i] == 0) return false;
	if(param[i] == '[')
	{
		++i;
		actual_map = "";
		while(param[i] != 0 && param[i] != '\n' && param[i] != ']')
		{
			actual_map = actual_map + param[i++];
		}
		return false;
	}

	while(param[i] != 0 && param[i] != '\n' && param[i] != '=')
	{
		paramName = paramName + param[i++];
	}
	++i;
	if(param[i] == '\'') ++i;
	while(param[i] != 0 && param[i] != '\n' && param[i] != '\'')
	{
		paramValue = paramValue + param[i++];
	}

	if(actual_map == "") params[paramName] = paramValue;
	else local_params[actual_map][paramName] = paramValue;
	return true;
}

bool Configurer::loadFile(const char* fn)
{
	actual_map = "";
	std::ifstream file(fn);
	char param[256];
	while(file.good())
	{
		file.getline(param, 256);
		parseParam(param);
	}
	file.close();
	if(DEBUG::DUMP_CONFIG) dumpConfig();
	return true;
}

std::string Configurer::searchParam(std::string name)
{
	std::string value;
	params_iterator it = params.find(name);
	if(it != params.end()) value = it->second;
	return value;
}

std::string Configurer::searchParam(std::string name, std::string default_value)
{
	std::string value(default_value);
	params_iterator it = params.find(name);
	if(it != params.end()) value = it->second;
	return value;
}

std::string Configurer::searchLocalParam(std::string node, std::string name)
{
	std::string value;
	multi_params_iterator multi_it = local_params.find(node);
	if(multi_it != local_params.end())
	{
		params_iterator it = multi_it->second.find(name);
		if(it != multi_it->second.end()) value = it->second;
	}
	return value;
}

std::string Configurer::searchLocalParam(std::string node, std::string name, std::string default_value)
{
	std::string value(default_value);
	multi_params_iterator multi_it = local_params.find(node);
	if(multi_it != local_params.end())
	{
		params_iterator it = multi_it->second.find(name);
		if(it != multi_it->second.end()) value = it->second;
	}
	return value;
}
