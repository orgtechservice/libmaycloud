#include <maycloud/asyncxmlstream.h>

using namespace std;

/**
* Конструктор
*/
AsyncXMLStream::AsyncXMLStream(int afd): AsyncStream(afd)
{
}

/**
* Деструктор
*/
AsyncXMLStream::~AsyncXMLStream()
{
}

/**
* Обработчик прочитанных данных
*/
void AsyncXMLStream::onRead(const char *data, size_t len)
{
	parseXML(data, len, false);
}

/**
* Обработчик ошибок парсера
*/
void AsyncXMLStream::onParseError(const char *message)
{
	onError(message);
}

/**
* Пир (peer) закрыл поток.
*
* Мы уже ничего не можем отправить в ответ,
* можем только корректно закрыть соединение с нашей стороны.
*/
void AsyncXMLStream::onPeerDown()
{
	fprintf(stderr, "AsyncXMLStream[%d]: peer down\n", getFd());
	
	// и сообщаем парсеру об EOF
	parseXML(0, 0, true);
}
