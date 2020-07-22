#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <map>
#include <string>

class HttpMessage
{
protected:
	std::map<std::string, std::string> headers;
	std::string body;

public:
	HttpMessage();
	~HttpMessage();
};

#endif
