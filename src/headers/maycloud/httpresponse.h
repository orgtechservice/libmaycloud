#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <fstream>
#include <sys/sendfile.h>

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
	long timer_id;
} waiting_info_t;
typedef struct {
	HttpResponse *response;
	FILE *file;
	off_t position;
	size_t filesize;
	long timer_id;
} sendfile_info_t;

class HttpResponse: public HttpMessage
{
protected:
	int _status;
	std::map<int, std::string> status_map;
	std::map<std::string, std::string> types;
	waiting_info_t *_waiting; // Ждалка
	sendfile_info_t *_sending; // Слалка

public:
	HttpResponse(HttpConnection *connection);
	~HttpResponse();

	void setStatus(int code);
	void setStatusPage(int code);
	void setContentType(const std::string &content_type);
	void setBody(const std::string &body);

	std::string toString();

	std::string headersString(unsigned long long content_length);

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

	/**
	 * На время отладки пусть пока называется так, потом объединим
	 */
	void sendBigFile(const std::string &filename);

	std::string mimeTypeByExtension(const std::string &extension);

	bool ready();

	inline bool pending() { return (_waiting != 0) || (_sending != 0); }

	void setReady(bool ready = true);

	void postpone();

	void waitForFile(const std::string &filename, response_handler_t handler, uint8_t timeout, void *userdata);

	static void updateFileWaiting(const timeval &tv, void *hi);
	
	static void updateFileSending(const timeval &tv, void *sending);

	void waitForFunction(custom_function_t custom_function, response_handler_t handler, uint8_t timeout, void *custom_function_userdata, void *handler_userdata);

	static void updateFunctionWaiting(const timeval &tv, void *hi);
};

#endif
