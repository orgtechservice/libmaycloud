#ifndef NANOSOFT_ASYNCOBJECT_H
#define NANOSOFT_ASYNCOBJECT_H

#include <maycloud/object.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <errno.h>

#include <sys/types.h>

class NetDaemon;

/**
* Базовый класс для всех асинхронных объектов
*/
class AsyncObject: public Object
{
friend class NetDaemon;
private:
	/**
	* Файловый дескриптор объекта
	*/
	int fd;
	
	/**
	* Ссылка на демона
	*/
	NetDaemon *daemon;
	
	/**
	* Конструктор копий
	*
	* Не ищите его реализации, его нет и не надо.
	* Просто блокируем конкструктор копий по умолчанию
	*/
	AsyncObject(const AsyncObject &);
	
	/**
	* Оператор присваивания
	*
	* Блокируем аналогично конструктору копий
	*/
	AsyncObject& operator = (const AsyncObject &);
	
protected:
	
	/**
	* Вернуть указатель на демона
	*/
	NetDaemon* getDaemon() const { return daemon; }
	
	/**
	* Установить демона
	*/
	void setDaemon(NetDaemon *pDaemon);
	
	/**
	* Обработка системной ошибки
	*/
	void stderror();
	
	/**
	* Установить файловый дескриптор
	*/
	void setFd(int newFd);
	
	/**
	* Вернуть маску ожидаемых событий
	*/
	virtual uint32_t getEventsMask() = 0;
	
	/**
	* Обработчик события
	*/
	virtual void onEvent(uint32_t events) = 0;
	
	/**
	* Событие ошибки
	*
	* Вызывается в случае возникновения какой-либо ошибки
	*/
	virtual void onError(const char *message);
public:
	/**
	* Конструктор
	*/
	AsyncObject();
	
	/**
	* Конструктор
	*/
	AsyncObject(int afd);
	
	/**
	* Вернуть файловый дескриптор
	*/
	int getFd() const { return fd; }
	
	/**
	* Деструктор
	*/
	virtual ~AsyncObject();
	
	/**
	 * Активировать объект в демоне
	 */
	bool enableObject();
	
	/**
	 * Деактивировать объект в демоне
	 */
	bool disableObject();
	
	/**
	* Отсоединиться от сервера
	*/
	void leaveDaemon();
};

#endif // NANOSOFT_ASYNCOBJECT_H
