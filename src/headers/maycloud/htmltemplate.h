#ifndef HTML_TEMPLATE_H
#define HTML_TEMPLATE_H

#include <maycloud/json.hpp>
#include <maycloud/inja.hpp>

class HtmlTemplate
{
protected:
	std::string _code;
	nlohmann::json _context;

public:
	HtmlTemplate(const HtmlTemplate&) = delete;
	HtmlTemplate(const std::string &filename);
	HtmlTemplate();
	~HtmlTemplate();

	void setCode(const std::string &code);
	std::string toString();
	std::string render();
	void set(const std::string &tag, const std::string &value);
};

#endif
