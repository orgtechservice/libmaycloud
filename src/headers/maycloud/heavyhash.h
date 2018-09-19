#ifndef HEAVY_HASH_H
#define HEAVY_HASH_H

#include <maycloud/hasher.h>

#include <stdint.h>
#include <sys/types.h>

/**
 * Хеш-таблица c поддержкой удаления элементов.
 */
template<class keytype, class itemtype>
class HeavyHash
{
protected:
	/**
	 * Максимальное число хранимых элементов
	 */
	size_t capacity;
	
	/**
	 * Текущее число элементов
	 */
	size_t count;
	
	/**
	 * Элемент таблицы
	 */
	struct item_t
	{
		/**
		 * Ключ (пока пусть будет)
		 */
		keytype key;
		
		/**
		 * Значение
		 */
		itemtype value;
		
		/**
		 * Указатель на следующий элемент
		 */
		item_t* next;
	};
	typedef item_t *item_p;
	
	/**
	 * "Куча", из которой мы будем выделять элементы для списков коллизий
	 */
	item_t* heap;
	
	/**
	 * Список свободных элементов в "куче"
	 */
	item_t* free;
	
	/**
	 * Собственно, хэш таблица, которая являет собой ничто иное как таблицу списков коллизий
	 */
	item_p *hash_table;
	
	/**
	 * Выделение элемента из "кучи"
	 */
	item_t* allocItem();
	
	/**
	 * Возврат элемента в "кучу"
	 */
	void releaseItem(item_t* item);
	
	/**
	 * Поиск элемента хеш-таблицы
	 */
	item_t* find(keytype key);
	
	/**
	 * Добавление нового элемента в хеш-таблицу
	 */
	void addValue(keytype key, itemtype value);
public:
	/**
	 * Конструктор
	 */
	HeavyHash(size_t cap);
	
	/**
	 * Деструктор
	 */
	~HeavyHash();
	
	/**
	 * Получение хеша
	 */
	size_t getHash(keytype& key) const;
	
	/**
	 * Вставка элемента в хеш-таблицу с заменой существующего
	 */
	void hashInsert(keytype key, itemtype value);
	
	/**
	 * Замена значения существующего элемента хеш-таблицы
	 */
	void hashReplace(keytype key, itemtype value);
	
	/**
	 * Удаление элемента из хеш-таблицы
	 */
	void hashDelete(keytype key);
	
	/**
	 * Поиск элемента в хеш-таблице по ключу
	 */
	itemtype hashSearch(keytype key);
};

/**
 * Работа с классом HeavyHash
 */

/**
 * Конструктор
 */
template<class keytype, class itemtype>
HeavyHash<keytype, itemtype>::HeavyHash(size_t cap)
{
	capacity = cap;
	heap = new item_t[capacity];
	hash_table = new item_p[capacity];
	
	// Связываем элементы "кучи"
	for(size_t i = 1; i < capacity; i++) heap[i - 1].next = &heap[i];
	
	// Список свободных элементов равен всей "куче"
	free = &heap[0];
}

/**
 * Деструктор
 */
template<class keytype, class itemtype>
HeavyHash<keytype, itemtype>::~HeavyHash()
{
	delete [] heap;
	delete [] hash_table;
}

/**
 * Получение хеша
 */
template<class keytype, class itemtype>
size_t HeavyHash<keytype, itemtype>::getHash(keytype& key) const
{
	Hasher hasher(capacity);
	hasher.feedData(&key, sizeof(key));
		
	return hasher.result();
}

/**
 * Выделение элемента из "кучи"
 */
template<class keytype, class itemtype>
typename HeavyHash<keytype, itemtype>::item_t* HeavyHash<keytype, itemtype>::allocItem()
{
	item_t* p = NULL;
	if( free != NULL )
	{
		p = free;
		free = free->next;
	}
	return p;
}

/**
 * Возврат элемента в "кучу"
 */
template<class keytype, class itemtype>
void HeavyHash<keytype, itemtype>::releaseItem(item_t* item)
{
	item_t* p = free;
	free = item;
	free->next = p;
}

/**
 * Поиск элемента хеш-таблицы
 */
template<class keytype, class itemtype>
typename HeavyHash<keytype, itemtype>::item_t* HeavyHash<keytype, itemtype>::find(keytype key)
{
	size_t hash = getHash(key);
	item_t* p = hash_table[hash];
	while( p != NULL )
	{
		if(p->key == key) break;
		p = p->next;
	}
	return p;
}

/**
 * Поиск элемента в хеш-таблице
 */
template<class keytype, class itemtype>
itemtype HeavyHash<keytype, itemtype>::hashSearch(keytype key)
{
	item_t* p = find(key);
	return p ? p->value : itemtype();
}

/**
 * Добавление нового элемента в хеш-таблицу
 */
template<class keytype, class itemtype>
void HeavyHash<keytype, itemtype>::addValue(keytype key, itemtype value)
{
	item_t* p = allocItem();
	
	if( p != NULL )
	{
		p->key = key;
		p->value = value;
		
		size_t hash = getHash(key);
		p->next = hash_table[hash];
		hash_table[hash] = p;
	}
}

/**
 * Вставка элемента в хеш-таблицу с заменой существующего
 */
template<class keytype, class itemtype>  
void HeavyHash<keytype, itemtype>::hashInsert(keytype key, itemtype value)
{
	item_t* p = find(key);
	if( p != NULL ) p->value = value;
	else addValue(key, value);
}

/**
 * Замена значения существующего элемента хеш-таблицы
 */
template<class keytype, class itemtype>
void HeavyHash<keytype, itemtype>::hashReplace(keytype key, itemtype value)
{
	item_t* p = find(key);
	if( p != NULL ) p->value = value;
}

/**
 * Удаление элемента из хеш-таблицы
 */
template<class keytype, class itemtype>  
void HeavyHash<keytype, itemtype>::hashDelete(keytype key)
{
	size_t hash = getHash(key);
	item_t* p = hash_table[hash];
	item_t* deleted;
	
	// Если первый же элемент в списке коллизий удаляемый
	if(p != NULL)
	{
		if( p->key == key )
		{
			hash_table[hash] = p->next;
			releaseItem(p);
			return;
		}
	}
	// Проходимся по списку коллизий в поисках удаляемого значения
	while( p != NULL )
	{
		if(p->next != NULL)
		{
			if(p->next->key == key) break;
		}
		p = p->next;
	}
	
	// Если значение найдено
	if( p != NULL )
	{
		deleted = p->next;
		p->next = deleted->next;
		releaseItem(deleted);
	}
}

#endif // HEAVY_HASH_H
