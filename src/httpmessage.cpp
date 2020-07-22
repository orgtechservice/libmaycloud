
#include <maycloud/httpmessage.h>

HttpMessage::HttpMessage(HttpConnection *connection): body("") {
	_connection = connection;
}

HttpMessage::~HttpMessage() {

}

HttpConnection *HttpMessage::connection() {
	return _connection;
}
