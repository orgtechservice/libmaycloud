#ifndef SAFEMAP_H
#define SAFEMAP_H

#include <map>
#include <string>
#include <stdio.h>

/**
 * Безопасныя карта, ассоциативный массив
 *
 * По сути этот список тот же std::map, но он дает безопасные указатели
 * на хранимые объекты. 
 * 
 * Библиотечный std::map работает с объектом как с
 * атомарным значением, если мы получаем итератор объекта и через него
 * меняем значение полей внутри объекта, то как мне кажется у нас нет
 * гарантий что примененые данные сохранятся в хранимом объете, а не какой
 * либо временной копии. 
 * 
 * Гарантий может не быть только в случае нестандартной или косячной реализации библиотеки.
 * Логика работы итераторов и практические тесты показывают, что через итератор меняются поля именно хранимого объекта.
 * Кроме того, существует мнение, что, вопреки стериотипам, stl уже давно хорошо отлажена.
 * Т.е. предыдущая реализация работы с std::map была написана с опорой на мануалы, мнения других специалистов и практический опыт.
 * В одной из ранних ревизий сохранилась реализация получания указателя на хранимый объект через итератор.
 * (прим. М.Соломатин)
 * 
 * Однако, меня лично смущает возможность некорректного поведения при переполнении std::map.
 * Хотя с другой стороны я пока не придумал, что надо сделать, чтобы добиться переполнения карты.
 * (прим. М.Соломатин)
 * 
 * В данном классе список возвращает точный указатель
 * на хранимый объект который будет гарантированно существовать до
 * соответствующего вызова remove().
 *
 * Память под элементы выделяется один раз при конструировании списка
 * и заносится в стек (связный список) для быстрого извлечения. Скорость
 * выделение/высвобождения памяти равна О(1), тем не менее время также
 * тратиться на индексирование, так что итоговая сложность отличается от О(1)
 *
 */
template <class type, class key_t = std::string>
class SafeMap
{
private:
	struct ref_t
	{
		ref_t *next;
	};
	
	typedef std::map<key_t, type*> index_t;
	typedef typename index_t::iterator index_iterator_t;
	
	/**
	 * Число элементов в списке
	 */
	int count;
	
	/**
	 * Максимальное число элементов в списке
	 */
	int capacity;
	
	/**
	 * Массив хранимых объектов
	 */
	type *items;
	
	/**
	 * Массив ссылок для распределения памяти
	 */
	ref_t *refs;
	
	/**
	 * Список свободных элементов
	 */
	ref_t *free;
	
	/**
	 * Индекс для поиска объекта по имени
	 */
	index_t index;
	
	void remove(type *item)
	{
		ref_t *ref = &refs[item - items];
		ref->next = free;
		free = ref;
	}
public:
	
	class iterator
	{
	private:
		index_iterator_t it;
		SafeMap<type, key_t> *map;
	public:
		iterator() {}
		iterator(SafeMap<type, key_t> *m): it(m->index.begin()), map(m) {}
		iterator(SafeMap<type, key_t> *m, index_iterator_t i): it(i), map(m) {}
		
		bool eof() { return it == map->index.end(); }
		void operator ++ () { ++it; }
		operator type* () { return it->second; }
		bool remove() {
			type *item = it->second;
			if ( item )
			{
				map->remove(item);
				map->index.erase(it++);
				return true;
			}
			return false;
		}
		iterator& operator = (SafeMap<type, key_t> *m)
		{
			it = m->index.begin();
			map = m;
			return *this;
		}
	};
	
	/**
	 * Конструктор
	 * @param maxlen максимальное число элементов которое может храниться
	 * в списке.
	 */
	SafeMap(int maxlen)
	{
		count = 0;
		capacity = maxlen;
		items = new type[maxlen];
		refs = new ref_t[maxlen];
		free = 0;
		for(int i = maxlen-1; i >= 0; i--)
		{
			ref_t *ref = &refs[i];
			ref->next = free;
			free = ref;
		}
	}
	
	/**
	 * Деструктор
	 */
	~SafeMap()
	{
		delete [] items;
		delete [] refs;
	}
	
	iterator begin()
	{
		iterator i(this, index.begin());
		return i;
		
	}
	iterator end() {
		iterator i(this, index.end());
		return i;
	}
	
	/**
	 * Поиск элемента по ключу
	 * 
	 * если элемент не найден, то возвращается NULL, так что надо быть
	 * осторожным и проверять результат перед использованием.
	 */
	type* find(const key_t &key) const
	{
		typename index_t::const_iterator it = index.find(key);
		if ( it != index.end() )
		{
			return it->second;
		}
		return 0;
	}
	
	/**
	 * Индексный оператор, по реализации соответствует функции find(),
	 * если элемент не найден, то возвращается NULL, так что надо быть
	 * осторожным и проверять результат перед использованием.
	 */
	type* operator [] (const key_t &key) const
	{
		return find(key);
	}
	
	/**
	 * Грязное добавление элемента
	 *
	 * Возвращаемый объект может содержать грязные данные
	 */
	type* dirtyAdd(const key_t &key)
	{
		if ( free )
		{
			ref_t *ref = free;
			free = free->next;
			type* item = &items[ref - refs];
			index[key] = item;
			count ++;
			return item;
		}
		return 0;
	}
	
	/**
	 * Добавление элемента
	 *
	 * Возвращаемый элемент конструируется конструктором по умолчанию
	 */
	type* add(const key_t &key)
	{
		type *item = dirtyAdd(key);
		if ( item ) *item = type();
		return item;
	}
	
	/**
	 * Добавление элемента с инициализацией
	 */
	type* add(const key_t &key, const type &init)
	{
		type *item = dirtyAdd(key);
		if ( item ) *item = init;
		return item;
	}
	
	/**
	 * Удалить элемент по ключу
	 */
	void remove(const key_t &key)
	{
		type *item = find(key);
		if ( item )
		{
			ref_t *ref = &refs[item - items];
			ref->next = free;
			free = ref;
			index.erase(key);
		}
	}
};

#endif // SAFEMAP_H
