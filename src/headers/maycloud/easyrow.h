#ifndef EASYROW_H
#define EASYROW_H

#include <map>
#include <string>

#include <maycloud/object.h>
#include <maycloud/easylist.h>

/**
 * Класс представляющий запись таблицы в базе данных,
 * просто в виде ассоциативного массива
 *
 * По сути это просто ассоциативный массив строк map<string, string>
 * который в разных частях библиотеки может выполнять различные функции.
 * Первичное назначение - представлять запись в таблице базы данных.
 * Но это также может быть базовый элемент для компонент QListView, QTreeView, QTableView,
 * или представлять набор атрибутов XML-тега или что-нибудь еще.
 *
 * @NOTE Класс использует семантику умных указателей. По логике реализации
 * он является указателем на автоматически создаваемый объект.
 * Когда один объект копирует другого, оба объекта начинают ссылаться
 * на одни и те же реальные данные и оба могут модифицировать данные.
 */
class EasyRow
{
private:
	class Row: public Object, public std::map<std::string, std::string>
	{
	public:
		void copy(const std::map<std::string, std::string> &row);
	};
	
	ptr<Row> ref;
public:
	
	/**
	 * Претворяемся что мы map<>
	 */
	typedef Row::const_iterator const_iterator;
	
	/**
	 * Конструктор по-умолчанию, создает новую пустую строку
	 */
    EasyRow();
	
	/**
	 * Конструктор копий, копирует ссылку на строку,
	 * в конечном итоге оба объекта начинают работать с одной строкой
	 */
	EasyRow(const EasyRow &row);
	
	/**
	 * Конструктор на основе map<string, string>
	 *
	 * Создает новый объект и копирует в него данные из map
	 */
	EasyRow(const std::map<std::string, std::string> &row);
	
	/**
	 * Деструктор
	 *
	 * Освобождает строку. Строка удаляется если на неё больше никто
	 * не ссылается. Если есть еще ссылки, то строка остается в памяти
	 */
	~EasyRow();
	
	/**
	 * Претворяемся что мы map<>
	 */
	inline const_iterator begin() const { return ref->begin(); }
	
	/**
	 * Претворяемся что мы map<>
	 */
	inline const_iterator end() const { return ref->end(); }
	
	/**
	 * Оператор копий
	 *
	 * Берет ссылку на новый объект, старый отсоединяется.
	 */
	EasyRow& operator = (const EasyRow &row) { ref = row.ref; }
	
	/**
	 * Претворяемся что мы map<>
	 */
	inline std::string& operator [] (const std::string &key) { return ref->operator [] (key); }
	
	/**
	 * Претворяемся что мы map<>, удалить элемент из строки по ключу
	 */
	void erase(const std::string &key) { ref->erase(key); }
	
	/**
	 * Удалить элемент из строки по ключу,
	 * более очевидный псевдоним для функции erase
	 */
	void remove(const std::string &key) { ref->erase(key); }
	
	/**
	 * Вернуть список ключей
	 */
	EasyList keys() const;
	
	/**
	 * Вернуть список значений
	 */
	EasyList values() const;
	
	/**
	 * Вернуть ссылку на map<>
	 */
	inline std::map<std::string, std::string>& map() { return *(ref.getObject()); }
};

#endif // EASYROW_H
