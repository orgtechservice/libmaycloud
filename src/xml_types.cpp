
#include <maycloud/xml_types.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
* Проверить существование атрибута
* @param name имя атрибута
* @return TRUE - атрибут есть, FALSE - атрибута нет
*/
bool attributes_t::hasAttribute(const std::string &name) const
{
	return this->find(name) != this->end();
}

/**
* Проверить существование атрибута
* @param name имя атрибута
* @return TRUE - атрибут есть, FALSE - атрибута нет
*/
bool attributes_t::hasAttribute(const char *name) const
{
	return this->find(name) != this->end();
}

/**
* Вернуть значение атрибута
* @param name имя атрибута
* @param default_value значение по умолчанию если атрибута нет
* @return значение атрибута
*/
const std::string attributes_t::getAttribute(const std::string &name, const std::string &default_value) const
{
	const_iterator it = this->find(name);
	return (it != this->end()) ? it->second : default_value;
}

/**
* Вернуть значение атрибута
* @param name имя атрибута
* @param default_value значение по умолчанию если атрибута нет
* @return значение атрибута
*/
const std::string attributes_t::getAttribute(const char *name, const char *default_value) const
{
	const_iterator it = this->find(name);
	return (it != this->end()) ? it->second : default_value;
}

/**
* Установить значение атрибута
*
* Если атрибут уже есть, то сменить его значение
* Если атрибута нет, то добавить атрибут с указанным значением
*
* @param name имя атрибута
* @param value новое значение атрибута
*/
void attributes_t::setAttribute(const std::string &name, const std::string &value)
{
	(*this)[name] = value;
}

/**
* Установить значение атрибута
*
* Если атрибут уже есть, то сменить его значение
* Если атрибута нет, то добавить атрибут с указанным значением
*
* @param name имя атрибута
* @param value новое значение атрибута
*/
void attributes_t::setAttribute(const char *name, const std::string &value)
{
	(*this)[name] = value;
}

/**
* Установить значение атрибута
*
* Если атрибут уже есть, то сменить его значение
* Если атрибута нет, то добавить атрибут с указанным значением
*
* @param name имя атрибута
* @param value новое значение атрибута
*/
void attributes_t::setAttribute(const char *name, const char *value)
{
	(*this)[name] = value;
}

/**
* Установить значение атрибута с форматированием в стиле printf()
*
* Если атрибут уже есть, то сменить его значение
* Если атрибута нет, то добавить атрибут с указанным значением
*
* @param name имя атрибута
* @param fmt форматирующая строка
*/
void attributes_t::setAttributef(const char *name, const char *fmt, ...)
{
	char *value;
	va_list args;
	va_start(args, fmt);
	int len = vasprintf(&value, fmt, args);
	va_end(args);
	(*this)[name] = value;
	free(value);
}

/**
* Удалить атрибут
*
* Если атрибута нет, то ничего не делать - это не ошибка
*
* @param name имя удаляемого атрибута
*/
void attributes_t::removeAttribute(const std::string &name)
{
	this->erase(name);
}

