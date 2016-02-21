#ifndef EASYROW_H
#define EASYROW_H

#include <map>
#include <string>

#include <maycloud/easylist.h>

/**
 * Класс представляющий запись таблицы в базе данных,
 * просто в виде ассоциативного массива
 *
 * По сути это просто ассоциативный массив строк map<string, string>
 * который в разных частях библиотеки может выполнять различные функции.
 * Первичное назначение - представлять запись в таблице базы данных.
 * Но это также может быть базовый элемент для компонент QListView, QTreeView, QTableView,
 * или представлять набор атрибутов XML-тега или что-нибудь еще.
 */
class EasyRow: public std::map<std::string, std::string>
{

public:
    EasyRow();
	
	/**
	 * Вернуть список ключей
	 */
	EasyList keys() const;
	
	/**
	 * Вернуть список значений
	 */
	EasyList values() const;
};


#endif // EASYROW_H
