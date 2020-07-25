#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <map>
#include <string>

class HttpConnection;

class HttpMessage
{
protected:
	std::map<std::string, std::string> _headers;
	std::string _body;
	std::string _content_type;
	unsigned long _content_length;
	HttpConnection *_connection;

public:
	HttpMessage(HttpConnection *connection);
	~HttpMessage();
	HttpMessage(const HttpMessage&) = delete;
	HttpConnection *connection();
	std::map<std::string, std::string> headers();
	std::string header(const std::string &header_name, const std::string &default_value);
	void setHeader(const std::string &header_name, const std::string &value);
	bool hasHeader(const std::string &header_name);
	bool hasLength();
	std::string contentType();
};

#endif
