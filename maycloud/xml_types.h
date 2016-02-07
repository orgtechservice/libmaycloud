
#ifndef AT_XML_TYPES_H
#define AT_XML_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <list>

class ATXmlTag;

enum ATXmlNodeType { TTag, TCharacterData };

class ATXmlNode {
	public:
	ATXmlNode(ATXmlNodeType nodetype, ATXmlTag *t) {
		type = nodetype;
		tag = t;
	}
	ATXmlNode(ATXmlNodeType nodetype, std::string nodecdata) {
		type = nodetype;
		cdata = nodecdata;
	}
	~ATXmlNode() {}
	ATXmlNodeType type;
	ATXmlTag *tag;
	std::string cdata;
};

typedef std::vector<ATXmlTag*> tags_stack_t;
typedef std::list<ATXmlTag*> tags_list_t;
typedef std::list<ATXmlNode*> nodes_list_t;

class attributes_t: public std::map<std::string, std::string>
{
public:
	/**
	* Проверить существование атрибута
	* @param name имя атрибута
	* @return TRUE - атрибут есть, FALSE - атрибута нет
	*/
	bool hasAttribute(const std::string &name) const;
	
	/**
	* Проверить существование атрибута
	* @param name имя атрибута
	* @return TRUE - атрибут есть, FALSE - атрибута нет
	*/
	bool hasAttribute(const char *name) const;
	
	/**
	* Вернуть значение атрибута
	* @param name имя атрибута
	* @param default_value значение по умолчанию если атрибута нет
	* @return значение атрибута
	*/
	const std::string getAttribute(const std::string &name, const std::string &default_value = "") const;
	
	/**
	* Вернуть значение атрибута
	* @param name имя атрибута
	* @param default_value значение по умолчанию если атрибута нет
	* @return значение атрибута
	*/
	const std::string getAttribute(const char *name, const char *default_value = "") const;
	
	/**
	* Установить значение атрибута
	*
	* Если атрибут уже есть, то сменить его значение
	* Если атрибута нет, то добавить атрибут с указанным значением
	*
	* @param name имя атрибута
	* @param value новое значение атрибута
	*/
	void setAttribute(const std::string &name, const std::string &value);
	
	/**
	* Установить значение атрибута
	*
	* Если атрибут уже есть, то сменить его значение
	* Если атрибута нет, то добавить атрибут с указанным значением
	*
	* @param name имя атрибута
	* @param value новое значение атрибута
	*/
	void setAttribute(const char *name, const std::string &value);
	
	/**
	* Установить значение атрибута
	*
	* Если атрибут уже есть, то сменить его значение
	* Если атрибута нет, то добавить атрибут с указанным значением
	*
	* @param name имя атрибута
	* @param value новое значение атрибута
	*/
	void setAttribute(const char *name, const char *value);
	
	/**
	* Установить значение атрибута с форматированием в стиле printf()
	*
	* Если атрибут уже есть, то сменить его значение
	* Если атрибута нет, то добавить атрибут с указанным значением
	*
	* @param name имя атрибута
	* @param fmt форматирующая строка
	*/
	void setAttributef(const char *name, const char *fmt, ...);
	
	/**
	* Удалить атрибут
	*
	* Если атрибута нет, то ничего не делать - это не ошибка
	*
	* @param name имя удаляемого атрибута
	*/
	void removeAttribute(const std::string &name);
};

#endif
