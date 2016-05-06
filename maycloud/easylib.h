#ifndef EASYLIB_H
#define EASYLIB_H

#include <list>
#include <string>
#include <maycloud/easyvector.h>
#include <maycloud/easyrow.h>

#define cstr(s) s.c_str()

#define qstr(s) s

std::string implode(const std::string &sep, const std::list<std::string> &list);

/**
 * Вернуть переменные окружения в виде ассоциативного массива
 */
EasyRow easyEnviron(char **envp);

/**
 * Вернуть переменные окружения в виде ассоциативного массива
 */
EasyRow easyEnviron();

/**
 * Вызов exec()
 */
int easyExec(const std::string &filename, EasyVector args, char **envp);

/**
 * Вызов exec()
 *
 * переменные окружения наследуются из текущегое процесса, из переменной environ
 */
int easyExec(const std::string &filename, EasyVector args);

/**
 * Вызов exec()
 *
 * Переменные окружения задаются в виде списка строк вида "KEY=value"
 */
int easyExec(const std::string &filename, EasyVector args, EasyVector env);

/**
 * Вызов exec()
 *
 * Переменные окружения задаются в виде ассоциативного массива
 */
int easyExec(const std::string &filename, EasyVector args, EasyRow env);

#endif // EASYLIB_H
