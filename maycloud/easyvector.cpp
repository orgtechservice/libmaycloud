#include "maycloud/easyvector.h"

void EasyVector::Vector::copy(const std::vector<std::string> &vec)
{
	std::vector<std::string>::operator = (vec);
}

/**
* Конструктор по-умолчанию, создает новый пустой вектор
*/
EasyVector::EasyVector()
{
	ref = new Vector();
}

/**
* Конструктор копий, копирует ссылку на вектор,
* в конечном итоге оба объекта начинают работать с одним векторомы
*/
EasyVector::EasyVector(const EasyVector &vec)
{
	ref = vec.ref;
}

/**
* Конструктор на основе vector<string>
*
* Создает новый объект и копирует в него данные из vector
*/
EasyVector::EasyVector(const std::vector<std::string> &vec)
{
	ref = new Vector();
	ref->copy(vec);
}

/**
* Деструктор
*
* Освобождает вектор. Вектор удаляется если на него больше никто
* не ссылается. Если есть еще ссылки, то вектор остается в памяти
*/
EasyVector::~EasyVector()
{
}
