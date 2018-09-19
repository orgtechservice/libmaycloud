#ifndef EASYLIST_H
#define EASYLIST_H

#include <list>
#include <string>

/**
 * Список строк
 *
 * Просто обертка вокруг list<string>
 */
class EasyList: public std::list<std::string>
{
public:
	EasyList();
};

#endif // EASYLIST_H
