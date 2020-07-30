#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <fstream>

#include <maycloud/httpmessage.h>
#include <maycloud/httpconnection.h>

class HttpMessage;
class HttpConnection;

typedef void (*response_handler_t)(HttpResponse *response, void *userdata);
typedef bool (*custom_function_t)(HttpResponse *response, void *userdata);
typedef struct {
	response_handler_t handler;
	custom_function_t custom_function;
	HttpResponse *response;
	std::string wait_type;
	std::string filename_to_wait;
	time_t expires;
	void *handler_userdata;
	void *custom_function_userdata;
} waiting_info_t;

class HttpResponse: public HttpMessage
{
protected:
	int _status;
	std::map<int, std::string> status_map;
	std::map<std::string, std::string> types;
	waiting_info_t *_waiting; // Ждалка

public:
	HttpResponse(HttpConnection *connection);
	~HttpResponse();

	void setStatus(int code);
	void setStatusPage(int code);
	void setContentType(const std::string &content_type);
	void setBody(const std::string &body);

	std::string toString();

	std::string statusText();
	std::string statusText(int code);

	/**
	 * Сформировать простую служебную веб-страницу
	 */
	std::string simpleHtmlPage(const std::string &title, const std::string &body);

	/**
	 * Сформировать простую веб-страницу и установить её в качестве содержимого ответа
	 */
	void setSimpleHtmlPage(const std::string &title, const std::string &body);

	void requireBasicAuth(const std::string &realm);

	void sendFile(const std::string &filename);

	std::string mimeTypeByExtension(const std::string &extension);

	bool ready();

	void setReady(bool ready = true);

	void postpone();

	void waitForFile(const std::string &filename, response_handler_t handler, uint8_t timeout, void *userdata);

	static void updateFileWaiting(const timeval &tv, void *hi);

	void waitForFunction(custom_function_t custom_function, response_handler_t handler, uint8_t timeout, void *handler_userdata, void *custom_function_userdata);

	static void updateFunctionWaiting(const timeval &tv, void *hi);
};

#endif
