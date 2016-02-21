#ifndef EASYRESULTSET_H
#define EASYRESULTSET_H

#include <vector>
#include <maycloud/easyrow.h>

extern template class std::vector<EasyRow>;

/**
 * Класс представляющий набор данных,
 * просто как список записей EasyRow
 *
 * По сути данный класс является списком элементов EasyRow (vector<EasyRow>),
 * который в разных частях библиотеки может выполнять разные функции.
 * Первичное назначение - представлять набор данных (строк) которые возвращает
 * SQL-запрос. Но это также может быть просто набор данных для классов
 * QListView, QTreeView, QTableView или что-то еще.
 *
 * @NOTE Класс использует семантику умных указателей. По логике реализации
 * он является указателем на автоматически создаваемый объект.
 * Когда один объект копирует другого, оба объекта начинают ссылаться
 * на одни и те же реальные данные и оба могут модифицировать данные.
 */
class EasyResultSet
{
private:
	class ResultSet: public Object, public std::vector<EasyRow>
	{
	};
	
	ptr<ResultSet> ref;
public:
	
	/**
	 * Претворяемся что мы vector<>
	 */
	typedef ResultSet::const_iterator const_iterator;
	
	/**
	 * Конструктор по умолчанию, создает пустой набор данных
	 */
    EasyResultSet();
	
	/**
	 * Конструктор копий, копирует ссылку на реальный объект,
	 * в результате оба объекта ссылаются реальный набор данных
	 */
	EasyResultSet(const EasyResultSet &r);
	
	/**
	 * Деструктор, совобождает ссылку. Если на объект никто не ссылается
	 * то он удаляется. Если ссылки еще есть, то продолжает жить.
	 */
	~EasyResultSet();
	
	/**
	 * Претворяемся что мы vector<>
	 */
	inline const_iterator begin() const { return ref->begin(); }
	
	/**
	 * Претворяемся что мы vector<>
	 */
	inline const_iterator end() const { return ref->end(); }
	
	/**
	 * Оператор копий
	 * 
	 * Старая строка высвобождается, копирует ссылку объект
	 */
	EasyResultSet& operator = (const EasyResultSet &row);
	
	/**
	 * 
	 */
	int count() { return ref->size(); }
	
	/**
	 * Претворяемся что мы vector<>
	 */
	inline EasyRow& operator [] (int idx) { return ref->operator [] (idx); }
	
	void push_back(const EasyRow &row) { ref->push_back(row); }
};

#endif // EASYRESULTSET_H
