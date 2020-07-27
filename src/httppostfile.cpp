
#include <maycloud/httppostfile.h>

HttpPostFile::HttpPostFile(HttpRequest *request) {
    _request = request;
}

HttpPostFile::~HttpPostFile() {

}

void HttpPostFile::setData(const std::string &data) {

}
