
#include <maycloud/httpmessage.h>

HttpMessage::HttpMessage(HttpConnection *connection) {
	_connection = connection;
}

HttpMessage::~HttpMessage() {

}

HttpConnection *HttpMessage::connection() {
	return _connection;
}
