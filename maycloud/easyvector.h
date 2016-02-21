#ifndef EASYVECTOR_H
#define EASYVECTOR_H

#include <vector>
#include <string>

/**
 * Список строк (массив/вектор)
 *
 * Просто обертка вокруг vector<string>
 */
class EasyVector: public std::vector<std::string>
{
public:
	EasyVector();
};

#endif // EASYVECTOR_H
