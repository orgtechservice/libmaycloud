#include "maycloud/easyresultset.h"

/**
* Конструктор по умолчанию, создает пустой набор данных
*/
EasyResultSet::EasyResultSet()
{
	ref = new ResultSet();
}

/**
* Конструктор копий, копирует ссылку на реальный объект,
* в результате оба объекта ссылаются реальный набор данных
*/
EasyResultSet::EasyResultSet(const EasyResultSet &r): ref(r.ref)
{
}

/**
* Деструктор, совобождает ссылку. Если на объект никто не ссылается
* то он удаляется. Если ссылки еще есть, то продолжает жить.
*/
EasyResultSet::~EasyResultSet()
{
}

/**
* Очистить список
*/
void EasyResultSet::clear()
{
	ref->clear(); 
}

/**
* Сбросить сисок, данные не очищаются, сбрасывается указатель
* и создается новый чистый объект. Если на старые данные никто не
* ссылается, то они осовбождаются.
*/
void EasyResultSet::reset()
{
	ref = new ResultSet();
}
