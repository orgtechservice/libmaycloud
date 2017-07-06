#ifndef MAYCLOUD_PROCESSMANAGER_H
#define MAYCLOUD_PROCESSMANAGER_H

#include <maycloud/object.h>
#include <maycloud/config.h>
#include <maycloud/easylib.h>

#include <string>

#include <errno.h>
#include <unistd.h>

#include <stdio.h>

/**
* callback обработчика завершения подпроцесса
* @param pid PID завершенного процесса
* @param exited true - если процесс завершился сам, false если был завершен по сигналу
* @param exit_code если процесс завершился сам, то это код завершения, если завершен по сигналу, то это номер сигнала
* @param data указатель на пользовательский объект
*/
typedef void (*exit_callback_t)(pid_t pid, bool exited, int exit_code, ptr<Object> data);

class ProcessInfo: public Object
{
public:
	pid_t pid;
	exit_callback_t callback;
	ptr<Object> data;
};

typedef std::map<pid_t, ptr<ProcessInfo> > process_list_t;

class ProcessManager
{
private:
	process_list_t ps;
public:
	/**
	* Форкнуть процесс и включить потомка под контроль
	*/
	pid_t fork(exit_callback_t callback, ptr<Object> data);
	
	/**
	* Запустить внешний процесс и добавить его в отслеживание
	* Функция callback будет автоматически вызвана при его завершении
	* data — указатель на пользовательские данные
	*/
	pid_t exec(std::string path, const EasyVector &args, const EasyRow & env, exit_callback_t callback, ptr<Object> data);
	
	/**
	* Добавить существующий процесс в мониторинг
	* Функция callback будет автоматически вызвана при его завершении
	* data — указатель на пользовательские данные
	*/
	void bindProcess(pid_t pid, exit_callback_t callback, ptr<Object> data);
	
	/**
	* Таймер
	*
	* Проводит проверки и считывает статусы завешенных процессов
	*/
	void onProcessTimer();

	/**
	 * Тщательно проверить существование процесса
	 */
	bool processExists(pid_t pid, std::string command);

	/**
	 * Провести некоторые подготовительные действия перед выполнением стороннего процесса
	 */
	//virtual int prepareExec();
};

#endif // MAYCLOUD_PROCESSMANAGER_H
