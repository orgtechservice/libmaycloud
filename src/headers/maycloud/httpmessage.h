#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <map>
#include <string>

class HttpConnection;

class HttpMessage
{
protected:
	std::map<std::string, std::string> headers;
	std::string _body;
	HttpConnection *_connection;

public:
	HttpMessage(HttpConnection *connection);
	~HttpMessage();
	HttpConnection *connection();
};

#endif
