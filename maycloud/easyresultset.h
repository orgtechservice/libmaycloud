#ifndef EASYRESULTSET_H
#define EASYRESULTSET_H

#include <list>
#include <maycloud/easyrow.h>

/**
 * Класс представляющий набор данных,
 * просто как список записей EasyRow
 *
 * По сути данный класс является списком элементов EasyRow (list<EasyRow>),
 * который в разных частях библиотеки может выполнять разные функции.
 * Первичное назначение - представлять набор данных (строк) которые возвращает
 * SQL-запрос. Но это также может быть просто набор данных для классов
 * QListView, QTreeView, QTableView или что-то еще.
 */
class EasyResultSet: public std::list<EasyRow>
{
public:
    EasyResultSet();
};

#endif // EASYRESULTSET_H
