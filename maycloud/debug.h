#ifndef NANOSOFT_DEBUG_H
#define NANOSOFT_DEBUG_H

class DEBUG
{
public:
	/**
	* Выводить для отладки все получаемые/передаваемые данные
	*/
	static bool DUMP_IO;

	/**
	* Выводить для отладки все получаемые/передаваемые станзы
	*/
	static bool DUMP_STANZA;

	/**
	* Выводить для отладки все SQL-запросы
	*/
	static bool DUMP_SQL;
	
	/**
	* Загрузить конфигурацию из переменных окружения
	*/
	static void load_from_env();
};

#endif // NANOSOFT_DEBUG_H
