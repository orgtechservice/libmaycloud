#include "maycloud/easyrow.h"

template class std::map<std::string, std::string>;

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
