
#include <maycloud/logger.h>

#include <time.h>

Logger logger;

Logger::Logger() {
	file = 0;
	start_time = (unsigned long int) time(0);
}

Logger::~Logger() {
	if(file) fclose(file);
}

unsigned long int Logger::uptime() {
	return ((unsigned long int) time(0) - start_time);
}

std::string Logger::currentTime() {
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

	return std::string(buffer);
}

void Logger::open(const char *filename) {
	file = fopen(filename, "a");
	if(!file) {
		fprintf(stderr, "failed to open log file: %s\n", filename);
	}
}

/*
* Информационное сообщение о штатной работе
*/
void Logger::information(const char *message, ...) {
	if(!file) return;

	char *buf;
	va_list args;
	va_start(args, message);
	int res __attribute__((unused));
	res = vasprintf(&buf, message, args);
	va_end(args);

	std::string current_time = currentTime();

	fputs("[I] ", file);
	fputs(current_time.c_str(), file);
	fputs(" ", file);
	fputs(buf, file);
	fputs("\n", file);

	fflush(file);

	free(buf);
}

/*
* Сообщение о событии, которое никогда не должно происходить
*/
void Logger::unexpected(const char *message, ...) {
	if(!file) return;

	char *buf;
	va_list args;
	va_start(args, message);
	int res __attribute__((unused));
	res = vasprintf(&buf, message, args);
	va_end(args);

	std::string current_time = currentTime();

	fputs("[U] ", file);
	fputs(current_time.c_str(), file);
	fputs(" ", file);
	fputs(buf, file);
	fputs("\n", file);

	fflush(file);

	free(buf);
}

/*
* Важное информационное сообщение
*/
void Logger::warning(const char *message, ...) {
	if(!file) return;

	char *buf;
	va_list args;
	va_start(args, message);
	int res __attribute__((unused));
	res = vasprintf(&buf, message, args);
	va_end(args);

	std::string current_time = currentTime();

	fputs("[W] ", file);
	fputs(current_time.c_str(), file);
	fputs(" ", file);
	fputs(buf, file);
	fputs("\n", file);

	fflush(file);

	free(buf);
}

/*
* Какая-то ошибка
*/
void Logger::error(const char *message, ...) {
	if(!file) return;

	char *buf;
	va_list args;
	va_start(args, message);
	int res __attribute__((unused));
	res = vasprintf(&buf, message, args);
	va_end(args);

	std::string current_time = currentTime();

	fputs("[E] ", file);
	fputs(current_time.c_str(), file);
	fputs(" ", file);
	fputs(buf, file);
	fputs("\n", file);

	fflush(file);

	free(buf);
}


/*
* Фатальная ошибка, означающая крах узла
*/
void Logger::fatalError(const char *message, int exitcode, ...) {
	if(!file) return;

	char *buf;
	va_list args;
	va_start(args, exitcode);
	int res __attribute__((unused));
	res = vasprintf(&buf, message, args);
	va_end(args);

	std::string current_time = currentTime();

	fputs("[F] ", file);
	fputs(current_time.c_str(), file);
	fputs(" ", file);
	fputs(buf, file);
	fputs("\n", file);

	fflush(file);

	free(buf);
	
	exit(exitcode);
}
