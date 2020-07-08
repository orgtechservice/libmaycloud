#ifndef EASYLIB_H
#define EASYLIB_H

#include <list>
#include <string>
#include <maycloud/easyvector.h>
#include <maycloud/easyrow.h>
#include <maycloud/logger.h>

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
 * Распарсить команду, представленную в виде строки, в
 * набор аргументов EasyVector.
 * 
 * @param args выходной набор аргументов  
 */
void parseCmdString(const std::string &cmd, EasyVector& args);

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

/**
* Получить вывод команды командной строки
*/
std::string getCmdOutput(const char *cmd);

/**
* Послать команду командному процессору операционной системы
*/
bool sendCmd(const std::string &cmd);

/**
* Послать команду командному процессору операционной системы
*/
bool sendCmd(const std::string &cmd, const std::string &errorMessage);

/**
* Послать команду командному процессору операционной системы и записать в лог
*/
bool sendCmdLog(const std::string &cmd);

/**
* Послать команду командному процессору операционной системы и записать в лог
*/
bool sendCmdLog(const std::string &cmd, const std::string &successLogMessage);

/**
* Послать команду командному процессору операционной системы и записать в лог
*/
bool sendCmdLog(const std::string &cmd, const std::string &errorMessage, const std::string &successLogMessage);

#endif // EASYLIB_H
