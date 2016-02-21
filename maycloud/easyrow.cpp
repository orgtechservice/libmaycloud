#include "maycloud/easyrow.h"


EasyRow::EasyRow()
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
