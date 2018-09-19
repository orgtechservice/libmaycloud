#ifndef MAYCLOUD_MAP_H
#define MAYCLOUD_MAP_H

#include <maycloud/object.h>
#include <map>

#include <stdio.h>

/**
* Ассоциативный массив объектов
* 
* Данный класс основан на std::map, но в отличие от std::map хранит
* не сами объекты, а умные указатели с подсчетом ссылок. Он может использоваться
* как для хранения объектов, так и для создания всевозможных индексов.
*/
template <class key_t, class object_t>
class Map
{
public:
	typedef std::map<key_t, ptr<object_t> > map_t;
	typedef typename map_t::iterator it_t;
	typedef typename map_t::const_iterator cit_t;
	
	map_t map;
public:
	object_t* operator [] (key_t key) const {
		cit_t it = map.find(key);
		if ( it == map.cend() ) return 0;
		return it->second.getObject();
	}
	
	bool add(key_t key, ptr<object_t> obj) {
		it_t it = map.find(key);
		if ( it == map.end() ) {
			map[key] = obj;
			return true;
		}
		return false;
	}
	
	void set(key_t key, ptr<object_t> obj) {
		map[key] = obj;
	}
	
	void erase(key_t key) {
		map.erase(key);
	}
};

template <class key_t, class object_t>
object_t* lookup(const std::map<key_t, ptr<object_t> > &map, key_t key)
{
	typename std::map<key_t, ptr<object_t> >::const_iterator it = map.find(key);
	if ( it == map.end() ) return 0;
	return it->second.getObject();
}

#endif // MAYCLOUD_MAP_H
