#ifndef EASYMYSQL_H
#define EASYMYSQL_H

#include <maycloud/easyrow.h>
#include <maycloud/easyresultset.h>

#include <mysql/mysql.h>
#include <mysql/errmsg.h>

#include <string>

/**
 * Класс для доступа к базам данных MySQL
 *
 * Легковесный класс который работает непосредственно
 * с libmysqlclient без посредников
 */
class EasyMySQL
{
protected:
	/**
	* Тип соединения
	*/
	enum {
		/**
		* Подключение через сеть
		*/
		CONNECT_INET,
		
		/**
		* Подключение черел UNIX-сокет
		*/
		CONNECT_UNIX
	} connectType;
	
	/**
	* Путь к UNIX-сокету
	*/
	std::string sock;
	
	/**
	* Хост MySQL-сервера
	*/
	std::string host;
	
	/**
	* Порт MySQL-сервера
	*/
	int port;
	
	/**
	* Имя базы данных
	*/
	std::string database;
	
	/**
	* Пользователь
	*/
	std::string user;
	
	/**
	* Пароль пользователя
	*/
	std::string password;
	
	/**
	* Соединение с MySQL сервером
	*/
	MYSQL conn;
	
	/**
	* Соединение сервером
	*
	* В случае неудачи выводит в stderr сообщение об ошибке и возращает FALSE
	* @return TRUE - соединение установлено, FALSE ошибка соединения
	*/
	bool reconnect();
	
	/**
	* Обертка вокруг mysql_real_query()
	*
	* имеет дополнительную обработку - в случае если сервер отключился,
	* то делает повторную попытку подключиться и выполнить запрос
	*/
	int real_query(const char *sql, size_t len);
	
	/**
	* Обертка вокруг mysql_real_query()
	*
	* имеет дополнительную обработку - в случае если сервер отключился,
	* то делает повторную попытку подключиться и выполнить запрос
	*/
	int real_query(const std::string &sql);
public:
	/**
	 * конструктор по умолчанию
	 */
	EasyMySQL();
	
	/**
	 * Деструктор
	 */
	~EasyMySQL();

	/**
	* Соединение сервером
	*
	* В случае неудачи выводит в stderr сообщение об ошибке и возращает FALSE
	*
	* @param host хост
	* @param database имя БД к которой подключаемся
	* @param user пользователь
	* @param password пароль
	* @param port порт
	* @return TRUE - соединение установлено, FALSE ошибка соединения
	*/
	bool connect(const std::string &host, const std::string &database, const std::string &user, const std::string &password, int port = 3306);
	
	/**
	* Соединение сервером
	*
	* В случае неудачи выводит в stderr сообщение об ошибке и возращает FALSE
	*
	* @param sock путь к Unix-сокету
	* @param database имя БД к которой подключаемся
	* @param user пользователь
	* @param password пароль
	* @return TRUE - соединение установлено, FALSE ошибка соединения
	*/
	bool connectUnix(const std::string &sock, const std::string &database, const std::string &user, const std::string &password);
	
	/**
	* Экранировать строку
	*/
	std::string escape(const std::string &text);
	
	/**
	* Экранировать строку и заключить её в кавычки
	*/
	std::string quote(const std::string &text);
	
	/**
	* Экранировать строки
	*/
	EasyRow quoteRow(const EasyRow &row);
	
	/**
	* Выполнить запрос не возвращающий набор данных
	*/
	bool exec(const std::string &sql);
	
	/**
	* Извлечь одну строку
	*
	* Функция выполняет SQL-запрос и возвращает первую строку.
	* Если запрос возвращает больше строк, то они игнорируются.
	*/
	EasyRow queryOne(const std::string &sql);
	
	/**
	* Выполнить запрос
	*
	* Функция выполняет SQL-запрос и возвращает набор данных
	* в виде списка строк.
	*/
	EasyResultSet queryAll(const std::string &sql);
	
	/**
	* Вставить строку в таблицу
	*
	* Значения строк нужно экранировать самостоятельно
	*/
	bool insert(const std::string &table, const EasyRow &row);
	
	/**
	* Вставить строку в таблицу
	*
	* Все значения автоматически экранируются
	*/
	bool insertQuoted(const std::string &table, const EasyRow &row);
	
	/**
	* Вставить-заменить строку
	*
	* Используется расширение MySQL REPLACE INTO
	*/
	bool replace(const std::string &table, const EasyRow &row);
	
	/**
	* Вставить-заменить строку
	*
	* Используется расширение MySQL REPLACE INTO
	* Все значения автоматически экранируются
	*/
	bool replaceQuoted(const std::string &table, const EasyRow &row);
	
	/**
	* Закрыть соединение с сервером
	*/
	void close();
};

#endif // EASYMYSQL_H
