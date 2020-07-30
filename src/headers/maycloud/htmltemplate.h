#ifndef HTML_TEMPLATE_H
#define HTML_TEMPLATE_H

#include <maycloud/inja.hpp>

class HtmlTemplate
{
public:
	HtmlTemplate(const HtmlTemplate&) = delete;
	HtmlTemplate(const std::string &filename);
	HtmlTemplate();
	~HtmlTemplate();
};

#endif
