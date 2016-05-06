#include "maycloud/easylib.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

std::string implode(const std::string &sep, const std::list<std::string> &list)
{
	std::string result;
	std::list<std::string>::const_iterator it = list.begin();
	if ( it == list.end() ) return result;
	
	result = *it;
	++it;
	
	while ( it != list.end() )
	{
		result += sep + *it;
		++it;
	}
	
	return result;
}

/**
 * Вернуть переменные окружения в виде ассоциативного массива
 */
EasyRow easyEnviron(char **envp)
{
	EasyRow env;
	
	for(; *envp; envp++)
	{
		char *line = *envp;
		char *x = strchr(line, '=');
		if ( x )
		{
			env[ std::string(line, x) ] = x + 1;
		}
		else
		{
			env[line] = "";
		}
	}
	
	return env;
}

/**
 * Вернуть переменные окружения в виде ассоциативного массива
 */
EasyRow easyEnviron()
{
	return easyEnviron(environ);
}

int easyExec(const std::string &filename, EasyVector args, char **envp)
{
	typedef char * const *argv_t;
	
	const char **argv;
	
	int argc = args.size();
	argv = new const char*[argc+1];
	for(int i = 0; i < argc; i++)
	{
		argv[i] = args[i].c_str();
	}
	argv[argc] = 0;
	
	return execve(filename.c_str(), (argv_t)argv, (argv_t)envp);
}

/**
* Вызов exec()
*
* переменные окружения наследуются из текущегое процесса, из переменной environ
*/
int easyExec(const std::string &filename, EasyVector args)
{
	return easyExec(filename, args, environ);
}

/**
* Вызов exec()
*
* Переменные окружения задаются в виде списка строк вида "KEY=value"
*/
int easyExec(const std::string &filename, EasyVector args, EasyVector env)
{
	char **envp;
	
	int envc = env.size();
	envp = new char*[envc+1];
	for(int i = 0; i < envc; i++)
	{
		envp[i] = const_cast<char*>(env[i].c_str());
	}
	envp[envc] = 0;
	
	return easyExec(filename, args, envp);
}


/**
* Вызов exec()
*
* Переменные окружения задаются в виде ассоциативного массива
*/
int easyExec(const std::string &filename, EasyVector args, EasyRow env)
{
	EasyVector e;
	for(EasyRow::const_iterator it = env.begin(); it != env.end(); ++it)
	{
		e.append(it->first + "=" + it->second);
	}
	return easyExec(filename, args, e);
}
