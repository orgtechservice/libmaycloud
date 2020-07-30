#include "maycloud/easylib.h"

#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>

EasyVector explode(const std::string &delimiter, const std::string &str) {
	EasyVector result;

	int length = str.length();
	int delimiter_length = delimiter.length();
	if(delimiter_length==0) return result;

	int i = 0;
	int k = 0;
	while(i < length) {
		int j = 0;
		while((i + j < length) && (j < delimiter_length) && (str[i + j] == delimiter[j])) j++;
		if(j == delimiter_length) {
			result.push_back(str.substr(k, i - k));
			i += delimiter_length;
			k = i;
		} else {
			i ++;
		}
	}
	result.push_back(str.substr(k, i - k));
	return result;
}

std::string implode(const std::string &sep, const std::list<std::string> &list)
{
	std::string result;
	std::list<std::string>::const_iterator it = list.begin();
	if ( it == list.end() ) return result;
	
	result = *it;
	++it;
	
	while ( it != list.end() )
	{
		result += sep + *it;
		++it;
	}
	
	return result;
}

/**
 * Вернуть переменные окружения в виде ассоциативного массива
 */
EasyRow easyEnviron(char **envp)
{
	EasyRow env;
	
	for(; *envp; envp++)
	{
		char *line = *envp;
		char *x = strchr(line, '=');
		if ( x )
		{
			env[ std::string(line, x) ] = x + 1;
		}
		else
		{
			env[line] = "";
		}
	}
	
	return env;
}

/**
 * Вернуть переменные окружения в виде ассоциативного массива
 */
EasyRow easyEnviron()
{
	return easyEnviron(environ);
}

/**
 * Распарсить команду, представленную в виде строки, в
 * набор аргументов EasyVector.
 * 
 * TODO штука недотестирована, но для работы виртуалок должно хватить.
 * 
 * @param args выходной набор аргументов  
 */
void parseCmdString(const std::string &cmd, EasyVector& args)
{
	std::size_t pos = 0;
	bool open = false;
	char opening = 0;
	std::string arg;
	while(cmd[pos] != 0)
	{
		if(cmd[pos] == '\'' or cmd[pos] == '\"')
		{
			if(pos == 0 or cmd[pos - 1] != '\\')
			{
				if(!open)
				{
					opening = cmd[pos];
					open = true;
					
					pos++;
					continue;
				}
				else
				{
					if(cmd[pos] == opening)
					{
						opening = 0;
						open = false;
						 
						pos++;
						continue;
					}
				}
			}
		}
		
		if(cmd[pos] == ' ')
		{
			if(!open)
			{
				args.append(arg);
				arg = "";
			}
			else arg += cmd[pos];
		}
		else arg += cmd[pos];
		
		pos++;
	}
	args.append(arg);
}

int easyExec(const std::string &filename, EasyVector args, char **envp)
{
	typedef char * const *argv_t;
	
	const char **argv;
	
	int argc = args.size();
	argv = new const char*[argc+1];
	for(int i = 0; i < argc; i++)
	{
		argv[i] = args[i].c_str();
	}
	argv[argc] = 0;
	
	return execve(filename.c_str(), (argv_t)argv, (argv_t)envp);
}

/**
* Вызов exec()
*
* переменные окружения наследуются из текущегое процесса, из переменной environ
*/
int easyExec(const std::string &filename, EasyVector args)
{
	return easyExec(filename, args, environ);
}

/**
* Вызов exec()
*
* Переменные окружения задаются в виде списка строк вида "KEY=value"
*/
int easyExec(const std::string &filename, EasyVector args, EasyVector env)
{
	char **envp;
	
	int envc = env.size();
	envp = new char*[envc+1];
	for(int i = 0; i < envc; i++)
	{
		envp[i] = const_cast<char*>(env[i].c_str());
	}
	envp[envc] = 0;
	
	return easyExec(filename, args, envp);
}


/**
* Вызов exec()
*
* Переменные окружения задаются в виде ассоциативного массива
*/
int easyExec(const std::string &filename, EasyVector args, EasyRow env)
{
	EasyVector e;
	for(EasyRow::const_iterator it = env.begin(); it != env.end(); ++it)
	{
		e.append(it->first + "=" + it->second);
	}
	return easyExec(filename, args, e);
}

/**
* Получить вывод команды командной строки
*/
std::string getCmdOutput(const std::string& cmd) {
	char buffer[512];
	std::string result;
	FILE *pipe = popen(cmd.c_str(), "r");
	if (!pipe) {
		std::cerr << "getCmdOutput(" + cmd + "): popen() failed\n";
		return "";
	}

	while (fgets(buffer, sizeof buffer, pipe) != NULL) {
		result += buffer;
	}

	pclose(pipe);
	return result;
}

bool sendCmd(const std::string &cmd) {
	return !system(cmd.c_str());
}

bool sendCmd(const std::string &cmd, const std::string &error_message) {
	if (!system(cmd.c_str())) {
		return true;
	} else {
		std::cerr << error_message << '\n';
		return false;
	}
}

bool sendCmdLog(const std::string &cmd) {
	if (!system(cmd.c_str())) {
		logger.information(cmd.c_str());
		return true;
	} else {
		return false;
	}
}

bool sendCmdLog(const std::string &cmd, const std::string &success_log_message) {
	if (!system(cmd.c_str())) {
		logger.information(success_log_message.c_str());
		return true;
	} else {
		return false;
	}
}

bool sendCmdLog(const std::string &cmd, const std::string &error_message, const std::string &success_log_message) {
	if (!system(cmd.c_str())) {
		logger.information(success_log_message.c_str());
		return true;
	} else {
		std::cerr << error_message << '\n';
		return false;
	}
}

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * Раскодировать из base64
 * © https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
 */
std::string base64_decode(const std::string &encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while(in_len -- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i ++] = encoded_string[in_]; in_ ++;
		if(i == 4) {
			for(i = 0; i < 4; i ++) char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for(i = 0; (i < 3); i ++) ret += char_array_3[i];
			i = 0;
		}
	}

	if(i) {
		for(j = i; j < 4; j ++) char_array_4[j] = 0;
		for(j = 0; j < 4; j ++) char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

/**
 * Locale-independent conversion of ASCII characters to lowercase.
 */
int av_tolower(int c) {
	if (c >= 'A' && c <= 'Z') c ^= 0x20;
	return c;
}

/**
 * Decodes an URL from its percent-encoded form back into normal
 * representation. This function returns the decoded URL in a string.
 * The URL to be decoded does not necessarily have to be encoded but
 * in that case the original string is duplicated.
 *
 * @param url a string to be decoded.
 * @return new string with the URL decoded or NULL if decoding failed.
 * Note that the returned string should be explicitly freed when not
 * used anymore.
 */
char *urldecode(const char *url)
{
	int s = 0, d = 0, url_len = 0;
	char c;
	char *dest = NULL;

	if(!url) return NULL;

	url_len = strlen(url) + 1;
	dest = (char *) malloc(url_len);

	if(!dest) return NULL;

	while(s < url_len) {
		c = url[s++];

		if(c == '%' && s + 2 < url_len) {
			char c2 = url[s ++];
			char c3 = url[s ++];
			if(isxdigit(c2) && isxdigit(c3)) {
				c2 = tolower(c2);
				c3 = tolower(c3);

				if(c2 <= '9')
					c2 = c2 - '0';
				else
					c2 = c2 - 'a' + 10;

				if(c3 <= '9')
					c3 = c3 - '0';
				else
					c3 = c3 - 'a' + 10;

				dest[d ++] = 16 * c2 + c3;

			} else { /* %zz or something other invalid */
				dest[d ++] = c;
				dest[d ++] = c2;
				dest[d ++] = c3;
			}
		} else if(c == '+') {
			dest[d ++] = ' ';
		} else {
			dest[d ++] = c;
		}

	}

	return dest;
}

std::string urldecode(const std::string &url) {
	char *buf = urldecode(url.c_str());
	std::string result(buf);
	free(buf);
	return result;
}

std::string trim(const std::string &s) {
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

std::string regexEscape(const std::string &s) {
	std::regex specialChars { R"([-[\]{}()*+?.,\^$|#\s])" };
	return std::regex_replace(s, specialChars, R"(\$&)");
}

bool setPidFile(const char* fn) {
	FILE* f = fopen(fn, "w+");
	if(!f) {
		perror("Can't create PID-file");
		return false;
	}

	fprintf(f, "%u", getpid());
	fclose(f);
	
	return true;
}

bool fileExists(const std::string& path) {
    return std::ifstream(path).good();
}
