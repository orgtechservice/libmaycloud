#ifndef EASYVECTOR_H
#define EASYVECTOR_H

#include <vector>
#include <string>

#include <maycloud/object.h>

/**
 * Список строк (массив/вектор)
 *
 * По сути этот класс представляет из себя умный указатель на vector<string>,
 * в отличие от обычного указателя он никогда не бывает NULL
 */
class EasyVector: public std::vector<std::string>
{
private:
	class Vector: public Object, public std::vector<std::string>
	{
	public:
		void copy(const std::vector<std::string> &vec);
	};
	
	ptr<Vector> ref;
public:
	/**
	 * Конструктор по-умолчанию, создает новый пустой вектор
	 */
    EasyVector();
	
	/**
	 * Конструктор копий, копирует ссылку на вектор,
	 * в конечном итоге оба объекта начинают работать с одним векторомы
	 */
	EasyVector(const EasyVector &vec);
	
	/**
	 * Конструктор на основе vector<string>
	 *
	 * Создает новый объект и копирует в него данные из vector
	 */
	EasyVector(const std::vector<std::string> &vec);
	
	/**
	 * Деструктор
	 *
	 * Освобождает вектор. Вектор удаляется если на него больше никто
	 * не ссылается. Если есть еще ссылки, то вектор остается в памяти
	 */
	~EasyVector();
	
	/**
	 * Претворяемся что мы vector<>, возвращаем число элементов
	 */
	int size() const { return ref->size(); }
	
	/**
	 * Претворяемся что мы vector<>
	 */
	inline std::string& operator [] (int key) { return ref->operator [] (key); }
	
	/**
	 * Претворяемся что мы vector<>, добавляем новый элемент в конец списка
	 */
	void push_back(const std::string &value) { ref->push_back(value); }
	
	/**
	 * Добавляем новый элемент в конец списка, по сути алиас push_back() с более очевидным названием
	 */
	void append(const std::string &value) { ref->push_back(value); }
	
	/**
	 * Претворяемся что мы vector<>, удаляем все элементы
	 */
	void clear() { ref->clear(); }
	
	/**
	 * Вернуть копию вектора
	 */
	std::vector<std::string> copy() { return *(ref.getObject()); }
};

#endif // EASYVECTOR_H
