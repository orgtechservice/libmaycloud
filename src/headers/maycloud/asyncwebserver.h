#ifndef ASYNCWEBSERVER_H
#define ASYNCWEBSERVER_H

#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <regex>

#include <maycloud/netdaemon.h>
#include <maycloud/asyncserver.h>
#include <maycloud/logger.h>

class HttpRequest;
class HttpResponse;
class AsyncWebServer;

#include <maycloud/httpconnection.h>

typedef void (*http_request_handler_t)(HttpRequest *request, HttpResponse *response, void *userdata);
//typedef std::pair<http_request_handler_t, void *> http_route_map_item_t;
typedef struct {
	http_request_handler_t handler;
	void *userdata;
	//std::map<std::string, std::string> params;
} http_route_map_item_t;
typedef std::map<std::string, http_route_map_item_t> http_route_map_t;

#include <maycloud/httprequest.h>
#include <maycloud/httpresponse.h>

/**
* Встраиваемый HTTP-сервер
*/
class AsyncWebServer: public AsyncServer
{
protected:
	/**
	 * Принять входящее соединение
	 */
	virtual void onAccept();

	/**
	 * Карта обработчиков GET-запросов
	 */
	http_route_map_t get_routes;

	/**
	 * Карта обработчиков GET-запросов по маске
	 */
	http_route_map_t get_mask_routes;

	/**
	 * Карта обработчиков POST-запросов
	 */
	http_route_map_t post_routes;

	/**
	 * Карта обработчиков POST-запросов по маске
	 */
	http_route_map_t post_mask_routes;

	/**
	 * Строка идентификации сервера
	 */
	std::string _server_id;

	/**
	 * Демон
	 */
	NetDaemon *_daemon;

public:
	/**
	* Конструктор
	*/
	AsyncWebServer(NetDaemon *daemon);
	
	/**
	* Деструктор
	*/
	~AsyncWebServer();

	/**
	 * Добавить обработчик GET-запроса
	 */
	void get(const std::string &path, http_request_handler_t handler, void *userdata);

	/**
	 * Добавить обработчик POST-запроса
	 */
	void post(const std::string &path, http_request_handler_t handler, void *userdata);

	/**
	 * Добавить обработчик GET+POST-запроса
	 */
	void route(const std::string &path, http_request_handler_t handler, void *userdata);

	/**
	 * Добавить HLS-ресурс
	 */
	void hlsRoute(const std::string &path, const std::string &filesystem_path, void *userdata);

	/**
	 * Выбрать зарегистрированный обработчик
	 */
	http_route_map_item_t *selectRoute(http_route_map_t *routes, http_route_map_t *mask_routes, HttpRequest *request);

	/**
	 * Обработчик по умолчанию
	 */
	static void defaultRequestHandler(HttpRequest *request, HttpResponse *response, void *userdata);

	/**
	 * Обработать входящий HTTP-запрос
	 */
	void handleRequest(HttpRequest *request, HttpResponse *response);

	/**
	 * Преобразовать маску маршрута в регулярное выражение
	 */
	std::string maskToRegex(const std::string &path);

	/**
	 * Получить строку идентификации сервера
	 */
	std::string serverIdString();

	NetDaemon *daemon();
};

#endif // ASYNCWEBSERVER_H
