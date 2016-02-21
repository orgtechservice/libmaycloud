#include "maycloud/easyrow.h"

#include <cstdio>

void EasyRow::Row::copy(const std::map<std::string, std::string> &row)
{
	std::map<std::string, std::string>::operator = (row);
}

/**
* Конструктор по-умолчанию, создает новую пустую строку
*/
EasyRow::EasyRow()
{
	ref = new Row();
}

/**
* Конструктор копий, копирует ссылку на строку,
* в конечном итоге оба объекта начинают работать с одной строкой
*/
EasyRow::EasyRow(const EasyRow &row)
{
	ref = row.ref;
}

/**
* Конструктор на основе map<string, string>
*
* Создает новый объект и копирует в него данные из map
*/
EasyRow::EasyRow(const std::map<std::string, std::string> &row)
{
	ref = new Row();
	ref->copy(row);
}

/**
* Деструктор
*
* Освобождает строку. Строка удаляется если на неё больше никто
* не ссылается. Если есть еще ссылки, то строка остается в памяти
*/
EasyRow::~EasyRow()
{
}

/**
* Вернуть список ключей
*/
EasyList EasyRow::keys() const
{
	EasyList result;
	
	const_iterator it = begin();
	while ( it != end() )
	{
		result.push_back(it->first);
		++it;
	}
	
	return result;
}

/**
* Вернуть список значений
*/
EasyList EasyRow::values() const
{
	EasyList result;
	
	const_iterator it = begin();
	while ( it != end() )
	{
		result.push_back(it->second);
		++it;
	}
	
	return result;
}
