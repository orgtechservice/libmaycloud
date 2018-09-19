#ifndef NANOSOFT_XMLPARSER_H
#define NANOSOFT_XMLPARSER_H

#include <maycloud/error.h>
#include <maycloud/xml_types.h>

#include <string.h>

#include <expat.h>

#include <map>
#include <string>

namespace nanosoft
{
	/**
	* XML парсер
	*/
	class XMLParser
	{
	public:
		/**
		* Конструктор
		*/
		XMLParser();
		
		/**
		* Деструктор
		*/
		virtual ~XMLParser();
		
		/**
		* Парсинг XML
		* 
		* Если включена компрессия, то данные сначала распаковываются
		* 
		* @param data буфер с данными
		* @param len длина буфера с данными
		* @param isFinal TRUE - последний кусок, FALSE - будет продолжение
		* @return TRUE - успешно, FALSE - ошибка парсинга
		*/
		bool parseXML(const char *data, size_t len, bool isFinal);
		
		/**
		* Сбросить парсер, начать парсить новый поток
		*/
		void resetParser();
		
	protected:
		/**
		* Обработчик открытия тега
		*/
		virtual void onStartElement(const std::string &name, const attributes_t &attributes) = 0;
		
		/**
		* Обработчик символьных данных
		*/
		virtual void onCharacterData(const std::string &cdata) = 0;
		
		/**
		* Обработчик закрытия тега
		*/
		virtual void onEndElement(const std::string &name) = 0;
		
		/**
		* Обработчик ошибок парсера
		*/
		virtual void onParseError(const char *message) = 0;
		
	private:
		/**
		* Парсер expat
		*/
		XML_Parser parser;
		
		/**
		* Признак парсинга
		* TRUE - парсер в состоянии обработка куска файла
		*/
		bool parsing;
		
		/**
		* Признак необходимости перенинициализации парсера
		* TRUE - парсер должен быть переинициализован перед
		*   обработкой следующего куска файла
		*/
		bool resetNeed;
		
		/**
		* Инициализация парсера
		*/
		bool initParser();
		
		/**
		* Реальная переинициализация парсера
		*/
		bool realResetParser();
		
		/**
		* Парсинг XML
		*
		* @param buf буфер с данными
		* @param len длина буфера с данными
		* @param isFinal TRUE - последний кусок, FALSE - будет продолжение
		* @return TRUE - успешно, FALSE - ошибка парсинга
		*/
		bool realParseXML(const char *buf, size_t len, bool isFinal);
		
		/**
		* Обработчик открытия тега
		*/
		static void startElementCallback(void *user_data, const XML_Char *name, const XML_Char **atts);
		
		/**
		* Отработчик символьных данных
		*/
		static void characterDataCallback(void *user_data, const XML_Char *s, int len);
		
		/**
		* Отбработчик закрытия тега
		*/
		static void endElementCallback(void *user_data, const XML_Char *name);
	};
}

#endif // NANOSOFT_XMLPARSER_H
