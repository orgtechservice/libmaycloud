#include <maycloud/xmlwriter.h>

namespace
{
	template <class T>
	inline T min(T a, T b)
	{
		return a < b ? a : b;
	}
}

namespace nanosoft
{
	/**
	* Конструктор
	*/
	XMLWriter::XMLWriter(int bufsize):
		full(false),
		opened(false),
		buflen(bufsize),
		buffered(0),
		outbuf(new char[bufsize])
	{
	}
	
	/**
	* Деструктор
	*/
	XMLWriter::~XMLWriter()
	{
		flush();
		delete [] outbuf;
	}
	
	/**
	* Вытолкнуть данные из буфера
	*/
	void XMLWriter::flushBuffer()
	{
		if ( buffered > 0 )
		{
			onWriteXML(outbuf, buffered);
			buffered = 0;
		}
	}
	
	/**
	* Записать часть данных
	*/
	void XMLWriter::output(const char *buf, size_t len)
	{
		while ( len > 0 )
		{
			size_t chunk = min(len, buflen - buffered);
			memcpy(outbuf + buffered, buf, chunk);
			buffered += chunk;
			buf += chunk;
			len -= chunk;
			if ( buffered == buflen )
			{
				flushBuffer();
			}
		}
	}
	
	/**
	* Записать часть данных
	*/
	void XMLWriter::output(const std::string &data)
	{
		output(data.c_str(), data.length());
	}
	
	/**
	* Экранировать спец. символы XML
	*
	* TODO optimize
	*/
	std::string XMLWriter::escape(const std::string &text)
	{
		std::string result;
		for(std::string::const_iterator pos = text.begin(); pos != text.end(); ++pos)
		{
			switch ( *pos )
			{
			case '<': result.append("&lt;"); break;
			case '>': result.append("&gt;"); break;
			case '&': result.append("&amp;"); break;
			case '"': result.append("&quot;"); break;
			default: result.append( &*pos, 1 );
			}
		}
		return result;
	}
	
	/**
	* Записать атрибуты тега
	*/
	void XMLWriter::writeAttributes()
	{
		for(attributes_t::const_iterator pos = attributes.begin(); pos != attributes.end(); ++pos)
		{
			output(" " + pos->first + "=\"" + escape(pos->second) + "\"");
		}
		attributes.clear();
	}
	
	/**
	* Завершить формирование тега
	*/
	void XMLWriter::compliteTag()
	{
		if ( opened )
		{
			writeAttributes();
			output(">", 1);
			opened = false;
		}
	}
	
	/**
	* Занести имя тега в стек
	*/
	void XMLWriter::pushTag(const std::string &tag)
	{
		stack.push_back(tag);
	}

	/**
	* Извлечь имя тега из стека
	*/
	std::string XMLWriter::popTag()
	{
		std::string tag = stack.back();
		stack.pop_back();
		return tag;
	}
	
	/**
	* Инициализация XML-потока
	*/
	void XMLWriter::initXML()
	{
		full = true;
		output("<?xml version=\"1.0\"?>\n");
	}
	
	/**
	* Начать новый XML-файл
	*/
	void XMLWriter::resetWriter()
	{
		flush();
		opened = false;
		stack.clear();
	}
	
	/**
	* Открыть тег
	*/
	void XMLWriter::startElement(const std::string &tag)
	{
		compliteTag();
		output("<" + tag);
		attributes.clear();
		pushTag(tag);
		opened = true;
	}
	
	/**
	* Установить атрибут тега
	*/
	void XMLWriter::setAttribute(const std::string &name, const std::string &value)
	{
		attributes[name] = value;
	}
	
	/**
	* Записать символьные данные
	*/
	void XMLWriter::characterData(const std::string &cdata)
	{
		compliteTag();
		output(escape(cdata));
	}
	
	/**
	* Закрыть тег
	*/
	void XMLWriter::endElement(const std::string &tag)
	{
		if ( opened )
		{
			writeAttributes();
			output(" />");
			opened = false;
		}
		else
		{
			compliteTag();
			output("</" + tag + ">");
		}
	}
	
	/**
	* Добавить простой тег
	*/
	void XMLWriter::addElement(const std::string &tag, const std::string &value)
	{
		startElement(tag);
		characterData(value);
		endElement(tag);
	}
	
	/**
	* Вытолкнуть данные
	*/
	void XMLWriter::flush()
	{
		compliteTag();
		flushBuffer();
	}
}
