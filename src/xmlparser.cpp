#include <maycloud/xmlparser.h>

using namespace std;

namespace nanosoft
{
	/**
	* Конструктор
	*/
	XMLParser::XMLParser(): parser(0), parsing(false), resetNeed(false)
	{
		initParser();
	}
	
	/**
	* Деструктор
	*/
	XMLParser::~XMLParser()
	{
		if ( parser ) XML_ParserFree(parser);
	}
	
	/**
	* Инициализация парсера
	*/
	bool XMLParser::initParser()
	{
		parser = XML_ParserCreate((XML_Char *) "UTF-8");
		if ( parser == 0 ) return false;
		XML_SetUserData(parser, (void*) this);
		XML_SetElementHandler(parser, startElementCallback, endElementCallback);
		XML_SetCharacterDataHandler(parser, characterDataCallback);
		XML_SetDefaultHandler(parser, NULL);
		return true;
	}
	
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
	bool XMLParser::parseXML(const char *data, size_t len, bool isFinal)
	{
		return realParseXML(data, len, isFinal);
	}
	
	/**
	* Реальная переинициализация парсера
	*/
	bool XMLParser::realResetParser()
	{
		if ( parser )
		{
			XML_ParserFree(parser);
			parser = 0;
		}
		if ( initParser() )
		{
			resetNeed = false;
			return true;
		}
		return false;
	}
	
	/**
	* Парсинг XML
	*
	* @param buf буфер с данными
	* @param len длина буфера с данными
	* @param isFinal TRUE - последний кусок, FALSE - будет продолжение
	* @return TRUE - успешно, FALSE - ошибка парсинга
	*/
	bool XMLParser::realParseXML(const char *buf, size_t len, bool isFinal)
	{
		parsing = true;
		int r = XML_Parse(parser, buf, len, isFinal);
		parsing = false;
		if ( resetNeed ) return realResetParser();
		if ( ! r )
		{
			onParseError(XML_ErrorString(XML_GetErrorCode(parser)));
			return false;
		}
		return true;
	}
	
	/**
	* Сбросить парсер, начать парсить новый поток
	*/
	void XMLParser::resetParser()
	{
		// TODO что-то надо делать с обработкой ошибок
		if ( parsing ) resetNeed = true;
		else realResetParser();
	}
	
	/**
	* Обработчик открытия тега
	*/
	void XMLParser::startElementCallback(void *user_data, const XML_Char *name, const XML_Char **atts)
	{
		attributes_t attributes;
		for(int i = 0; atts[i]; i += 2) {
			attributes[ atts[i] ] = atts[i + 1];
		}
		static_cast<XMLParser *>(user_data)->onStartElement(name, attributes);
	}
	
	/**
	* Отработчик символьных данных
	*/
	void XMLParser::characterDataCallback(void *user_data, const XML_Char *s, int len)
	{
		static_cast<XMLParser *>(user_data)->onCharacterData(string(s, len));
	}
	
	/**
	* Отбработчик закрытия тега
	*/
	void XMLParser::endElementCallback(void *user_data, const XML_Char *name)
	{
		static_cast<XMLParser *>(user_data)->onEndElement(name);
	}
}
