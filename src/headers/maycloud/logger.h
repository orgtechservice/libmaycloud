#ifndef NANOSOFT_LOGGER_H
#define NANOSOFT_LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <string>

class Logger
{
	public:
		Logger();
		~Logger();

		void open(const char *filename);

		void information(const char *format, ...);
		void warning(const char *format, ...);
		void error(const char *format, ...);
		void fatalError(const char *format, int exitcode, ...);
		void unexpected(const char *format, ...);
		unsigned long int uptime();

	private:
		FILE *file;
		std::string currentTime();
		unsigned long int start_time;
};

extern Logger logger;

#endif
