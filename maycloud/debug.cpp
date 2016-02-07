
#include <maycloud/debug.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
* Выводить для отладки все получаемые/передаваемые данные
*/
bool DEBUG::DUMP_IO = false;

/**
* Выводить для отладки все получаемые/передаваемые станзы
*/
bool DEBUG::DUMP_STANZA = false;

/**
* Выводить для отладки все SQL-запросы
*/
bool DEBUG::DUMP_SQL = false;

/**
* Прочитать значение булевской опции
*/
static bool get_bool_opt_raw(const char *name)
{
	const char *v = getenv(name);
	if ( v )
	{
		if ( strcmp(v, "yes") == 0 ) return true;
		if ( strcmp(v, "no") == 0 ) return false;
		return true;
	}
	return false;
}

/**
* Прочитать значение булевской опции
*/
static bool get_bool_opt(const char *name)
{
	bool v = get_bool_opt_raw(name);
	const char *vs = v ? "yes" : "no";
	printf("[DEBUG] %s=%s\n", name, vs);
	return v;
}

/**
* Загрузить конфигурацию из переменных окружения
*/
void DEBUG::load_from_env()
{
	DUMP_IO = get_bool_opt("DUMP_IO");
	DUMP_STANZA = get_bool_opt("DUMP_STANZA");
	DUMP_SQL = get_bool_opt("DUMP_SQL");
}
