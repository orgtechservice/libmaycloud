#ifndef NANOSOFT_XMLWRITER_H
#define NANOSOFT_XMLWRITER_H

#include <maycloud/xml_types.h>

#include <string.h>
#include <string>
#include <vector>
#include <map>


namespace nanosoft
{
	/**
	* Генератор XML-потоков
	*/
	class XMLWriter
	{
	private:
		/**
		* Признак полного XML-файла
		* TRUE - будет формироваться полный XML-файл
		* FALSE - будет формироваться только кусок XML-файла
		*/
		bool full;
		
		/**
		* Признак открытого тега
		*/
		bool opened;
		
		/**
		* Стек имен открытых тегов
		*/
		std::vector<std::string> stack;
		
		/**
		* Размер выходного буфера
		*/
		size_t buflen;
		
		/**
		* Размер буферизированых данных
		*/
		size_t buffered;
		
		/**
		* Выходной буфер
		*/
		char *outbuf;
		
		/**
		* Вытолкнуть данные из буфера
		*/
		void flushBuffer();
		
		/**
		* Записать часть данных
		*/
		void output(const char *buf, size_t len);
		
		/**
		* Записать часть данных
		*/
		void output(const std::string &data);
		
		/**
		* Записать атрибуты тега
		*/
		void writeAttributes();
		
		/**
		* Завершить формирование тега
		*/
		void compliteTag();
		
		/**
		* Занести имя тега в стек
		*/
		void pushTag(const std::string &tag);
		
		/**
		* Извлечь имя тега из стека
		*/
		std::string popTag();
		
	protected:
		/**
		* Атрибуты тега
		*/
		attributes_t attributes;
		
		/**
		* Вывод XML-данных
		*/
		virtual void onWriteXML(const char *buf, size_t len) = 0;
		
	public:
		/**
		* Конструктор
		*/
		XMLWriter(int bufsize = 4096);
		
		/**
		* Деструктор
		*/
		virtual ~XMLWriter();
		
		/**
		* Начать новый XML-файл
		*/
		void resetWriter();
		
		/**
		* Экранировать спец. символы XML
		*/
		static std::string escape(const std::string &text);
		
		/**
		* Инициализация XML-потока
		*/
		void initXML();
		
		/**
		* Открыть тег
		*/
		void startElement(const std::string &tag);
		
		/**
		* Установить атрибут тега
		*/
		void setAttribute(const std::string &name, const std::string &value);
		
		/**
		* Записать символьные данные
		*/
		void characterData(const std::string &cdata);
		
		/**
		* Закрыть тег
		*/
		void endElement(const std::string &tag);
		
		/**
		* Добавить простой тег
		*/
		void addElement(const std::string &tag, const std::string &value);
		
		/**
		* Вытолкнуть данные
		*/
		void flush();
	};
	
}

#endif // NANOSOFT_XMLWRITER_H
