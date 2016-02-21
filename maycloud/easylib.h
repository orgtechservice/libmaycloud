#ifndef EASYLIB_H
#define EASYLIB_H

#include <list>
#include <string>

#define cstr(s) s.c_str()

#define qstr(s) s

std::string implode(const std::string &sep, const std::list<std::string> &list);

#endif // EASYLIB_H
