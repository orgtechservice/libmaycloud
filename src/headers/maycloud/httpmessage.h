#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <map>
#include <string>

class HttpMessage
{
protected:
	std::map<std::string, std::string> headers;

public:
	HttpMessage();
	~HttpMessage();
};

#endif
