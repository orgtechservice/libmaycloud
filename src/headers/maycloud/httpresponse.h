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
	FILE *file;
	off_t position;
	size_t filesize;
	std::string content_type;
} sendfile_info_t;

class HttpResponse: public HttpMessage
{
protected:
	int _status;
	waiting_info_t *_waiting; // Ждалка
	sendfile_info_t *_sending; // Слалка

public:
	HttpResponse(HttpConnection *connection);
	~HttpResponse();

	inline int status() { return _status; }
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

	bool ready();

	inline bool pending() { return (_waiting != 0) || (_sending != 0); }

	void handlePendingOperation();

	void setReady(bool ready = true);

	void postpone();

	void waitForFile(const std::string &filename, response_handler_t handler, uint8_t timeout, void *userdata);

	static void updateFileWaiting(const timeval &tv, void *hi);

	void waitForFunction(custom_function_t custom_function, response_handler_t handler, uint8_t timeout, void *custom_function_userdata, void *handler_userdata);

	static void updateFunctionWaiting(const timeval &tv, void *hi);
};

#endif
