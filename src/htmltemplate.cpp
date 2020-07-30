
#include <maycloud/htmltemplate.h>

HtmlTemplate::HtmlTemplate(const std::string &filename) {

}

HtmlTemplate::HtmlTemplate() {
	_code = "";
}

HtmlTemplate::~HtmlTemplate() {

}

void HtmlTemplate::setCode(const std::string &code) {
	_code = code;
}

std::string HtmlTemplate::toString() {
	return inja::render(_code, _context);
}

std::string HtmlTemplate::render() {
	return toString();
}

void HtmlTemplate::set(const std::string &tag, const std::string &value) {
	_context[tag] = value;
}
