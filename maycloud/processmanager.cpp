
#include <maycloud/processmanager.h>
#include <maycloud/logger.h>

#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
* Форкнуть процесс и включить потомка под контроль
*/
pid_t ProcessManager::fork(exit_callback_t callback, ptr<Object> data)
{
	pid_t pid = ::fork();
	if ( pid == -1 )
	{
		logger.unexpected("ProcessManager::fork() failed: %s", strerror(errno));
		return -1;
	}
	
	if ( pid != 0 )
	{
		ptr<ProcessInfo> p = new ProcessInfo();
		p->pid = pid;
		p->callback = callback;
		p->data = data;
		ps[pid] = p;
	}
	
	return pid;
}

/**
* Запустить внешний процесс и добавить его в отслеживание
* Функция callback будет автоматически вызвана при его завершении
* data — указатель на пользовательские данные
*/
pid_t ProcessManager::exec(std::string path, const EasyVector &args, const EasyRow & env, exit_callback_t callback, ptr<Object> data)
{
	pid_t pid = this->fork(callback, data);
	if ( pid == 0 )
	{
		//prepareExec();
		int r = easyExec(path, args, env);
		if ( r == -1 )
		{
			logger.unexpected("ProcessManager::exec(%s) failed to exec process: %s", path.c_str(), strerror(errno));
		}
		exit(1);
	}
	return pid;
}

/**
* Добавить существующий процесс в мониторинг
* Функция callback будет автоматически вызвана при его завершении
* data — указатель на пользовательские данные
*/
void ProcessManager::bindProcess(pid_t pid, exit_callback_t callback, ptr<Object> data)
{
	ptr<ProcessInfo> p = new ProcessInfo();
	p->pid = pid;
	p->callback = callback;
	p->data = data;
	ps[pid] = p;
}

/**
* Таймер
*
* Проводит проверки и считывает статусы завешенных процессов
*/
void ProcessManager::onProcessTimer()
{
	process_list_t::const_iterator it;
	
	// прочитаем статусы зомби-процессов
	while ( 1 )
	{
		int status;
		int pid = waitpid(-1, &status, WNOHANG);
		if ( pid == -1 )
		{
			if ( errno == ECHILD )
			{
				// нет дочерних процессов
				break;
			}
			
			//logger.unexpected("ProcessManager::onTimer() waitpid failed: %s", strerror(errno));
			printf("ProcessManager::onTimer() waitpid failed: %s\n", strerror(errno));
			break;
		}
		
		if ( pid == 0 )
		{
			// нет потомков-зомби
			break;
		}
		
		it = ps.find(pid);
		if ( it == ps.end() )
		{
			logger.unexpected("ProcessManager::onTimer() unknown child, pid=%d", pid);
			continue;
		}
		
		if( WIFEXITED(status) )
		{
			int exit_code = WEXITSTATUS(status);
			it->second->callback(it->second->pid, true, exit_code, it->second->data);
			ps.erase(pid);
		}
		
		if ( WIFSIGNALED(status) )
		{
			int sig = WTERMSIG(status);
			it->second->callback(it->second->pid, false, sig, it->second->data);
			ps.erase(pid);
		}
	}
	
	// Проверим незавершенные процессы, может некоторые из них осиротели и завершились без сигнала
	std::list<pid_t> keys;
	for(it = ps.begin(); it != ps.end(); ++it)
	{
		pid_t pid = it->second->pid;
		int r = kill(pid, 0);
		if( (r == -1) && (errno == ESRCH) )
		{
			keys.push_back(pid);
			it->second->callback(pid, false, 0, it->second->data);
		}
	}
	
	for(std::list<pid_t>::const_iterator kit = keys.begin(); kit != keys.end(); ++kit)
	{
		ps.erase(*kit);
	}
}

/**
 * Тщательно проверить существование процесса
 */
bool ProcessManager::processExists(pid_t pid, std::string command) {
	// Тривиальные случаи
	if(pid == 0) return false;
	if(pid == 1) return true;

	// Остальные
	if(kill(pid, 0) == 0) {
		char path[40];
		char cmdline[1000];
		memset(cmdline, 0, 1000);
		sprintf(path, "/proc/%i/cmdline", pid);
		FILE *cmdline_file = fopen(path, "rb");
		if(cmdline_file == 0) {
			// Процесс исчез между вызовами kill и fopen!
			return false;
		}

		size_t limit = fread(cmdline, sizeof(char), 999, cmdline_file);
		fclose(cmdline_file);

		for(size_t i = 0; i < limit; i++)
		{
			if(cmdline[i] == 0) cmdline[i] = ' ';
		}
		
		// Поиск подстроки (напомню, fgets гарантирует добавление ноль-терминатора)
		char *position = strstr(cmdline, command.c_str());
		
		if(position != 0) {
			return true;
		} else {
			return false;
		}
		//------------------------------------------------
	} else {
		// Здесь могла произойти ошибка, связанная с тем, что мы не можем лапать чужие <s>части тела</s> процессы
		if(errno == 0) {
			return false;
		} else {
			switch(errno)
			{
				// Процесс запущен, но у нас нет прав, чтобы его трогать.
				// Скоррее всего, это какой-то левый процесс, и нечего к нему лезть вообще.
				// Нужно протестировать это как следует.
				case EPERM:
					fprintf(stderr, "Process <%i, %s> exists, but running under different user, so I cannot control it\n", pid, command.c_str());
					//return true;
				// Процесс и так мёртвый, всё ОК.
				case ESRCH:
					return false;
			}
		}
	}
}

/**
 * Провести некоторые подготовительные действия перед выполнением стороннего процесса
 */
//int ProcessManager::prepareExec() { return 0; }
