#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <maycloud/httpmessage.h>

class HttpResponse: public HttpMessage
{
public:
	HttpResponse();
	~HttpResponse();
};

#endif
