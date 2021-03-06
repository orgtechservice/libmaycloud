#include <maycloud/asyncobject.h>
#include <maycloud/netdaemon.h>

using namespace std;

/**
* Конструктор
*/
AsyncObject::AsyncObject(): fd(-1), daemon(0) {
}

/**
* Конструктор
*/
AsyncObject::AsyncObject(int afd): fd(afd), daemon(0) {
}

/**
* Деструктор
*/
AsyncObject::~AsyncObject()
{
}

/**
* Установить демона
*/
void AsyncObject::setDaemon(NetDaemon *pDaemon)
{
	// если ничего не меняется, то осталвяем всё как есть и выходим
	if ( daemon == pDaemon ) return;
	
	// если объект подключается к демону с нуля,
	// то увеличить счетчик ссылок,
	// иначе надо деактивировать объект который возможно был добавлен в epoll
	if ( daemon == 0 )
	{
		this->lock();
	}
	else
	{
		if ( fd != -1 ) disableObject();
	}
	
	// меняем демона
	daemon = pDaemon;
	
	// если мы отключаемся от демона,
	// то уменьшить счетчик ссылок
	if ( pDaemon == 0 ) this->release();
}

/**
* Обработка системной ошибки
*/
void AsyncObject::stderror()
{
	onError(strerror(errno));
}

/**
* Установить файловый дескриптор
*/
void AsyncObject::setFd(int newFd)
{
	// если ничего не изменилось, то оставляем всё как есть и выходим
	if ( fd == newFd ) return;
	
	// перед сменой дескриптора не забываем деактировать объект
	// который возможно был добавлен в epoll
	if ( fd != -1 ) disableObject();
	fd = newFd;
}

/**
* Событие ошибки
*
* Вызывается в случае возникновения какой-либо ошибки
*/
void AsyncObject::onError(const char *message)
{
	fprintf(stderr, "AsyncObject[%d]: %s\n", fd, message);
}

/**
* Активировать объект в демоне
*/
bool AsyncObject::enableObject()
{
	if ( daemon )
	{
		return daemon->enableObject(this);
	}
	return false;
}

/**
* Деактивировать объект в демоне
*/
bool AsyncObject::disableObject()
{
	if ( daemon )
	{
		return daemon->disableObject(this);
	}
	return false;
}

/**
* Отсоединиться от сервера
*/
void AsyncObject::leaveDaemon()
{
	if ( daemon )
	{
		daemon->removeObject(this);
	}
}
