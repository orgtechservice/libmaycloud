#ifndef NANOSOFT_ERROR_H
#define NANOSOFT_ERROR_H

#include <errno.h>

#include <string.h>

#include <string>
#include <stdexcept>
#include <iostream>

namespace nanosoft
{
	/**
	* Сгенерировать исключение (runtime_error)
	* @param message сообщение об ошибке
	* @note сообщение об ошибке сразу же выводиться в stderr
	*/
	void fatal_error(const char *message);
	void fatal_error(const std::string &message);
	
	/**
	* Вернуть стандартное сообщение о последней
	* ошибке вызова стандартной функции C
	*/
	const char * stderror();
	
	/**
	* Проверить статус вызова стандартной функции C
	* и в случае ошибки сгенерировать исключение
	* @param status если 0, то сгенерировать исключение
	* @note сообщение об ошибке сразу же выводиться в stderr
	*/
	void stdcheck(int status);
}

#endif // NANOSOFT_ERROR_H
