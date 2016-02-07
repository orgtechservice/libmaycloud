#include <maycloud/object.h>

/**
* Конструктор
*/
Object::Object()
{
	ref_count = 0;
}

/**
* Деструктор
*/
Object::~Object()
{
}

/**
* Заблокировать объект
*/
void Object::lock()
{
	ref_count++;
}

/**
* Освободить объект
*/
void Object::release()
{
	ref_count--;
	if ( ref_count == 0 ) onFree();
}

/**
* Обработка обнуления счетчика ссылок
*
* По умолчанию удаляет объект
*/
void Object::onFree()
{
	delete this;
}
