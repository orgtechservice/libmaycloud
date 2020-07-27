#ifndef HTTP_POST_FILE_H
#define HTTP_POST_FILE_H

#include <maycloud/httprequest.h>

class HttpPostFile
{
protected:
	HttpRequest *_request;

public:
	HttpPostFile(HttpRequest *request);
	setData(const std::string &data);
	~HttpPostFile();
};

#endif
