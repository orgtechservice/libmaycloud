#ifndef EASYLIB_H
#define EASYLIB_H

#include <list>
#include <string>
#include <maycloud/easyvector.h>
#include <maycloud/easyrow.h>
#include <maycloud/logger.h>

#define cstr(s) s.c_str()

#define qstr(s) s

EasyVector explode(const std::string &delimiter, const std::string &str);

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
std::string getCmdOutput(const std::string& cmd);

/**
* Послать команду командному процессору операционной системы
*/
bool sendCmd(const std::string &cmd);

/**
* Послать команду командному процессору операционной системы
*/
bool sendCmd(const std::string &cmd, const std::string &error_message);

/**
* Послать команду командному процессору операционной системы и записать в лог
*/
bool sendCmdLog(const std::string &cmd);

/**
* Послать команду командному процессору операционной системы и записать в лог
*/
bool sendCmdLog(const std::string &cmd, const std::string &success_log_message);

/**
* Послать команду командному процессору операционной системы и записать в лог
*/
bool sendCmdLog(const std::string &cmd, const std::string &error_message, const std::string &success_log_message);

/**
 * Относится ли символ к подмножеству base64
 */
static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

/**
 * Раскодировать из base64
 */
std::string base64_decode(const std::string &encoded_string);

int av_tolower(int c);

char *urldecode(const char *url);

std::string urldecode(const std::string &url);

#endif // EASYLIB_H
