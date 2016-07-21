#include <maycloud/config.h>
#include <maycloud/controlstream.h>
#include <maycloud/logger.h>
#include <maycloud/debug.h>

using namespace std;
using namespace nanosoft;

/**
 * Конструктор потока
 */
ControlStream::ControlStream(int sock):
	AsyncXMLStream(sock), XMLWriter(1024), depth(0), state(INIT)
{
}

/**
 * Деструктор потока
 */
ControlStream::~ControlStream()
{
}

/**
* Распарсить атрибут "to" на маршрут и имя агента
*/
void ControlStream::parseAttributeTo(const std::string &to, std::string &route, std::string &name)
{
	std::string resource;
	parseAttributeTo(to, route, name, resource);
}

/**
* Распарсить атрибут "to" на маршрут, имя агента и ресурс
*/
void ControlStream::parseAttributeTo(const std::string &to, std::string &route, std::string &name, std::string &resource)
{
	// все могло быть проще... но это же STL...
	// если будет время, надо будет сделать более понятное решение
	
	std::size_t pos = to.find('/');
	std::string part = to.substr(0, pos); // выцепляем часть route:host
	if ( pos == std::string::npos ) resource = "";
	else resource = to.substr(pos+1, std::string::npos);
	
	pos = part.find(':');
	route = part.substr(0, pos);
	if ( pos == std::string::npos ) name = "";
	else name = part.substr(pos+1, std::string::npos);
}

/**
* Сформировать атрибут "to"
*/
std::string ControlStream::makeAttributeTo(const std::string &route, const std::string &name)
{
	return route + ":" + name;
}

/**
* Сформировать атрибут "to"
*/
std::string ControlStream::makeAttributeTo(const std::string &route, const std::string &name, const std::string &resource)
{
	return route + ":" + name + "/" + resource;
}

/**
 * Вернуть маску ожидаемых событий
 */
uint32_t ControlStream::getEventsMask()
{
	uint32_t mask = EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | EPOLLHUP | EPOLLERR | EPOLLPRI;
	return mask;
}

/**
* Событие: выходной буфер пуст (всё отправлено)
*/
void ControlStream::onEmpty()
{
	if ( state == CLOSING )
	{
		onStreamClosed();
	}
}

/**
 * Запись XML
 */
void ControlStream::onWriteXML(const char *data, size_t len)
{
	if ( ! put(data, len) )
	{
		onError("write buffer fault");
	}
}

/**
* Открыть поток
*/
void ControlStream::openStream()
{
	if ( state == INIT )
	{
		initXML();
		startElement("stream");
		state = ACTIVE;
	}
	else
	{
		logger.unexpected("double call of ControlStream::openStream()");
		printf("double call of ControlStream::openStream(), state: %d\n", state);
	}
}

/**
* Закрыть поток
*/
void ControlStream::closeStream()
{
	if ( state == INIT )
	{
		// Если мы даже не успели открыть поток,
		// то видимо лучше сразу свалить из демона
		leaveDaemon();
	}
	
	if ( state == ACTIVE )
	{
		// Если поток был открыт, то закрываем поток, переходим в state=CLOSING
		// и ждем пока отправиться наше последнее слово
		endElement("stream");
		flush();
	}
	
	state = CLOSING;
}

/**
* Сброс потока в начальное состояние
*/
void ControlStream::resetStream()
{
	state = INIT;
}

/**
* Событие закрытия исходящего потока,
* когда мы сказали свое последнее слово
*/
void ControlStream::onStreamClosed()
{
	// по умолчанию сваливаем из демона как только сказали своё последнее
	// слово и поток почти наверняка самоудалиться, если его кто-то ещё
	// не залочил, а если кто-то залочил, то тот пусть и париться...
	leaveDaemon();
}

/**
 * Обработчик открытия тега
 */
void ControlStream::onStartElement(const std::string &name, const attributes_t &attributes)
{
	depth ++;
	switch ( depth )
	{
	case 1:
		onStartStream(name, attributes);
		break;
	case 2: // начало станзы
		builder.startElement(name, attributes, depth);
	break;
	default: // добавить тег в станзу
		builder.startElement(name, attributes, depth);
	}
}

/**
 * Обработчик символьных данных
 */
void ControlStream::onCharacterData(const std::string &cdata)
{
	builder.characterData(cdata);
}

/**
 * Обработчик закрытия тега
 */
void ControlStream::onEndElement(const std::string &name)
{
	switch (depth)
	{
	case 1:
		onEndStream();
		break;
	case 2: {
		builder.endElement(name);
		TagHelper stanza = builder.fetchResult();
		
		if ( DEBUG::DUMP_STANZA )
		{
			fprintf(stdout, "RECV STANZA[%d]: \033[22;32m%s\033[0m\n", getFd(), stanza->toString().c_str());
		}
		
		onStanza(stanza);
		delete stanza; // Внимание — станза удаляется здесь
		break;
	}
	default:
		builder.endElement(name);
	}
	depth --;
}

/**
 * Обработчик ошибок парсера
 */
void ControlStream::onParseError(const char *message)
{
	printf("[ControlStream: %d] parse error: %s\n", getFd(), message);
	// TODO сообщить агенту почему мы закрываем поток
	if ( state == INIT )
	{
		// Если мы ещё не открывали поток, то открыть только ради того
		// чтобы сказать свое матерное слово
		openStream();
		flush();
	}
	
	// Поругаемся
	// TODO сделать метод streamError() для подобных целей...
	TagHelper error = new ATXmlTag("error");
	error = "syntax error";
	sendStanza(error);
	delete error;
	
	// и закроем поток
	closeStream();
}

/**
 * Отправить станзу в поток (thread-safe)
 * @param stanza станза
 * @return TRUE - станза отправлена (или буферизована для отправки), FALSE что-то не получилось
 */
bool ControlStream::sendStanza(TagHelper stanza)
{
	string data = stanza->toString();
	
	if ( DEBUG::DUMP_STANZA )
	{
		fprintf(stdout, "SEND STANZA[%d]: \033[22;34m%s\033[0m\n", getFd(), data.c_str());
	}
	
	if ( state != ACTIVE )
	{
		logger.unexpected("send stanza in (yet|already) inactive stream");
		return false;
	}
	
	if ( ! put(data.c_str(), data.length()) )
	{
		// TODO что лучше? если буфер полон, то может начать очень
		// активно срать в логи, что ещё хуже чем промолчать...
		// logger.warning("can't send stanza: output buffer is full");
		onError("[ControlStream] sendStanza() fault");
		return false;
	}
	
	return true;
}

bool ControlStream::streamError(std::string from, std::string to, std::string description) {
	TagHelper stream_error = new ATXmlTag("stream-error");
	stream_error->setAttribute("from", from);
	stream_error->setAttribute("to", to);
	stream_error->setAttribute("description", description);
	bool status = sendStanza(stream_error);
	delete stream_error;
	return status;
}
