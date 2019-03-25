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
	using params_map = std::map<std::string, std::string>;
	using multi_params_map = std::map<std::string, params_map>;

	using params_iterator = params_map::iterator;
	using multi_params_iterator = multi_params_map::iterator;
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
