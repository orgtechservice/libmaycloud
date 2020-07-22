#ifndef ASYNCWEBSERVER_H
#define ASYNCWEBSERVER_H

#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#include <maycloud/netdaemon.h>
#include <maycloud/asyncserver.h>
#include <maycloud/logger.h>
#include <maycloud/httprequest.h>
#include <maycloud/httpresponse.h>
#include <maycloud/httpconnection.h>

typedef void (*http_request_handler_t)(HttpRequest *request, HttpResponse *response, void *userdata);
typedef std::pair<http_request_handler_t, void *> http_route_map_item_t;

/**
* Базовый класс для сервера HTTP
*/
class AsyncWebServer: public AsyncServer
{
protected:
	/**
	 * Принять входящее соединение
	 */
	virtual void onAccept();

	/**
	 * Карта обработчиков запросов
	 */
	std::map<std::string, http_route_map_item_t> routes;

public:
	/**
	* Конструктор
	*/
	AsyncWebServer();
	
	/**
	* Деструктор
	*/
	~AsyncWebServer();
	
	/**
	* Подключиться к порту
	*/
	bool bind(int port);
	
	/**
	* Подключиться к IP и порту
	*/
	bool bind(const char *ip, const char *port);

	/**
	* Закрыть сокет
	*/
	void close();

	/**
	 * Добавить обработчик GET-запроса
	 */
	void get(const std::string &path, http_request_handler_t handler, void *userdata);

	/**
	 * Обработчик по умолчанию
	 */
	static void defaultRequestHandler(HttpRequest *request, HttpResponse *response, void *userdata);
};

#endif // ASYNCWEBSERVER_H
