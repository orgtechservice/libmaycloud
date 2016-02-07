#include <maycloud/error.h>

using namespace std;

namespace nanosoft
{
	/**
	* Сгенерировать исключение (runtime_error)
	* @param message сообщение об ошибке
	* @note сообщение об ошибке сразу же выводиться в stderr
	*/
	void fatal_error(const char *message)
	{
		cerr << message << endl;
		throw runtime_error(message);
	}
	
	/**
	* Сгенерировать исключение (runtime_error)
	* @param message сообщение об ошибке
	* @note сообщение об ошибке сразу же выводиться в stderr
	*/
	void fatal_error(const std::string &message)
	{
		cerr << message << endl;
		throw std::runtime_error(message);
	}
	
	/**
	* Вернуть стандартное сообщение о последней
	* ошибке вызова стандартной функции C
	*/
	const char * stderror()
	{
		return strerror(errno);
	}
	
	/**
	* Проверить статус вызова стандартной функции C
	* и в случае ошибки сгенерировать исключение
	* @param status если 0, то сгенерировать исключение
	* @note сообщение об ошибке сразу же выводиться в stderr
	*/
	void stdcheck(int status)
	{
		if ( status == 0 )
		{
			fatal_error(stderror());
		}
	}
}
