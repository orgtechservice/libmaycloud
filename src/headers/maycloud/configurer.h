#ifndef CONFIGURER_H
#define CONFIGURER_H

/**
 * Configurer
 * 
 * (c) Mikhail Solomatin
 */

#include <string>
#include <fstream>
#include <map>

class Configurer
{
public:
	typedef std::map<std::string, std::string> params_map;
    typedef std::map<std::string, params_map> multi_params_map;

    typedef params_map::iterator params_iterator;
    typedef multi_params_map::iterator multi_params_iterator;
protected:
	/**
	 * Актуальная карта параметров.
	 * Если пустая строка, то параметры пишутся в общую карту.
	 */
	static std::string actual_map;

	/**
	 * Карта параметров, действующих для всей программы
	 */
	static params_map params;

	/**
	 * Карта параметров, действующих для частных случаев
	 */
	static multi_params_map local_params;

	bool parseParam(std::string param);

public:
	/**
	 * Конструктор, деструктор
	 */
	Configurer();
	~Configurer();
	
	bool loadFile(const char* fn);
	std::string searchParam(std::string name);
	std::string searchParam(std::string name, std::string default_value);
	std::string searchLocalParam(std::string node, std::string name);
	std::string searchLocalParam(std::string node, std::string name, std::string default_value);

	/**
	 * Вывести актуальную конфигурацию
	 */
	static void dumpConfig();
};

#endif // CONFIGURER_H
