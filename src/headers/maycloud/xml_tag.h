
#ifndef MAWAR_XML_TAG_H
#define MAWAR_XML_TAG_H

#include <maycloud/xmlwriter.h>
#include <maycloud/xml_types.h>

#include <string.h>

#include <iostream>
#include <string>

class ATXmlTag: public attributes_t
{
	public:
		ATXmlTag(std::string name, const attributes_t &tag_attributes, ATXmlTag *p, unsigned short int depth);
		ATXmlTag(std::string name);
		~ATXmlTag();
		
		// === потенциально устаревшие методы ===
		// какие-то их них после ревизии перенесены в новые
		// (такие как например getAttribute), другие отмечены
		// как действительно устаревшие
		
		std::string name();
		unsigned short int getDepth();
		void insertCharacterData(std::string cdata);
		std::string getCharacterData();
		void insertChildElement(ATXmlTag *tag);
		tags_list_t getChildren();
		ATXmlTag *getParent();
		void setNameSpace(std::string value);
		void setNameSpace(std::string name, std::string value);
		void setDefaultNameSpaceAttribute(std::string value);
		void setNameSpaceAttribute(std::string name, std::string value);
		std::string getNameSpace();
		std::string asString() const { return toString(); }
		bool hasChild(std::string tag_name);
		ATXmlTag *getChild(std::string tag_name);
		ATXmlTag *getChildByAttribute(std::string tag_name, std::string attribute, std::string attribute_value);
		nodes_list_t getChildNodes();
		attributes_t getAttributes();
		ATXmlTag *clone();
		
		// === новые методы ===
		
		/**
		* Сеаризлизовать тег в строку
		*/
		std::string toString() const;
		
		/**
		* Вернуть значение узла
		* @param tag_name имя тега
		* @param default_value значение по умолчанию если тега нет
		* @return значение узла
		*/
		std::string getChildValue(std::string tag_name, std::string default_value);
		
		/**
		* Вернуть первого потомка
		* @return первый потомок или 0 если потомков нет
		*/
		ATXmlTag* firstChild();
		
		/**
		* Вернуть следующего потомка следующего за тегом from
		* @param from узел с которого нужно начать поиск
		* @return найденый потомок или 0 если больше потомков нет
		*/
		ATXmlTag* nextChild(ATXmlTag *from);
		
		/**
		* Вернуть первого потомка с именем name
		* @param name имя искомого узла
		* @return найденый узел или 0
		*/
		ATXmlTag* firstChild(const char *name);
		ATXmlTag* firstChild(const std::string &name);
		
		/**
		* Вернуть следующего потока с именем name следующего за тегом from
		* @param name имя искомого потомка
		* @param from узел с которого надо начать поиск
		* @return найденый узел или 0 если больше потомков нет
		*/
		ATXmlTag* nextChild(const char *name, ATXmlTag *from);
		ATXmlTag* nextChild(const std::string &name, ATXmlTag *from);
		
		/**
		* Вернуть первый дочерний узел, какого бы типа он ни был
		* @return первый дочерний узел или 0 если дочерних узлов нет
		*/
		ATXmlNode* firstChildNode();
		
		/**
		* Вернуть следующий дочерний узел, какого бы типа он ни был
		* @return найденый узел или 0 если больше дочерних узлов нет
		*/
		ATXmlNode* nextChildNode(ATXmlNode *from);
		
		/**
		* Найти первого потомка по указанному пути
		* @param path путь к узлу
		* @return найденый узел или 0 если узел не найден
		*/
		ATXmlTag* find(const char *path);
		ATXmlTag* find(const std::string &path);
		
		/**
		* Найти следующий узел
		* @param path путь к узлу
		* @return найденый узел или 0 если узлов больше нет
		*/
		ATXmlTag* findNext(const char *path, ATXmlTag *from);
		ATXmlTag* findNext(const std::string &path, ATXmlTag *from);
		
		/**
		* Проверить имеет ли потомка
		* @note требуется для findNext
		*/
		bool hasChild(ATXmlTag *tag);
		
		/**
		* Удалить потомка с указанным именем
		*/
		void removeChild(const char *name);
		void removeChild(const std::string &name);
		
		/**
		* Удалить всех потомков
		*/
		void clear();
		
		/**
		* Вернуть число выделенных (активных) тегов
		*/
		static unsigned getTagsCount() { return tags_created - tags_destroyed; }
		
		/**
		* Вернуть максимальное число тегов
		*/
		static unsigned getTagsMaxCount() { return tags_max_count; }
		
		/**
		* Вернуть число созданных тегов (с момента запуска)
		*/
		static unsigned getTagsCreated() { return tags_created; }
		
		/**
		* Вернуть число удаленных тегов (с момента запуска)
		*/
		static unsigned getTagsDestroyed() { return tags_destroyed; }
		
	private:
		std::string tag_name;
		std::string tag_cdata;
		unsigned short int tag_depth;
		tags_list_t children;
		nodes_list_t childnodes;
		ATXmlTag *parent;
		std::string prefix;
		
		// ------- Статистика ------- //
		
		/**
		* Текущее число выделенных тегов
		*/
		static unsigned tags_created;
		
		/**
		* Текущее число освобожденных тегов
		*/
		static unsigned tags_destroyed;
		
		/**
		* Максимальное число выделенных тегов
		*/
		static unsigned tags_max_count;
		
		/**
		* Вызывается при создании тега
		*/
		static void onTagCreate();
		
		/**
		* Вызывается при удалении тега
		*/
		static void onTagDestroy();
};

#endif
