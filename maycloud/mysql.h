#ifndef NANOSOFT_MYSQL_H
#define NANOSOFT_MYSQL_H

#include <maycloud/config.h>
#include <maycloud/error.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include <mysql/mysql.h>
#include <mysql/errmsg.h>

#include <string>
#include <map>

namespace nanosoft
{
	typedef std::map<std::string, std::string> row_t;
	typedef std::map<std::string, std::string>::iterator row_iterator;
	
	/**
	* Класс для работы с базами MySQL
	*/
	class MySQL
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
		* Набор данных
		*/
		struct ResultSet
		{
			/**
			* набор данных MySQL
			*/
			MYSQL_RES *res;
			
			/**
			* Число полей
			*/
			size_t field_count;
			
			/**
			* Имена полей
			*/
			MYSQL_FIELD *fields;
			
			/**
			* Длины полей
			*/
			unsigned long *lengths;
			
			/**
			* Значения полей
			*/
			MYSQL_ROW values;
		};
		
		/**
		* Соединение сервером
		*
		* В случае неудачи выводит в stderr сообщение об ошибке и возращает FALSE
		* @return TRUE - соединение установлено, FALSE ошибка соединения
		*/
		bool reconnect();
		
	public:
		/**
		* Умный указатель на набор данных
		*/
		class result
		{
		private:
			ResultSet *res;
		public:
			result(ResultSet *r): res(r) { }
			result(const result &r): res(r.res) { }
			
			/**
			* Удалить набор данных
			*/
			void free();
			
			/**
			* Проверка на не NULL
			*/
			operator bool () { return res != 0; }
			
			/**
			* Проверка на NULL
			*/
			bool operator ! () { return ! res; }
			
			/**
			* Проверка достижения конца данных
			*/
			bool eof() { return res == 0 || res->values == 0; }
			
			/**
			* Переход к следующей строке
			*/
			void next();
			
			/**
			* Найти индекс поля
			*
			* Если поле не найдено, то ругается в stderr и возращает 0
			*/
			int indexOf(const char *name);
			
			/**
			* Вернуть значение поля по номеру
			*/
			std::string operator [] (size_t num);
			
			/**
			* Вернуть значение поля по имени
			*/
			std::string operator [] (const char *name);
			
			/**
			* Проверка поля по номеру на NULL
			*/
			bool isNull(size_t num);
			
			/**
			* Проверка поля по имени на NULL
			*/
			bool isNull(const char *name);
		};
		
		/**
		* Конструктор
		*/
		MySQL();
		
		/**
		* Деструктор
		*/
		~MySQL();
		
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
		* Выполнить произвольный SQL-запрос
		* @param sql текст одного SQL-запроса
		* @param len длина запроса
		* @return указатель на набор данных
		*/
		MySQL::result queryRaw(const char *sql, size_t len);
		
		/**
		* Выполнить произвольный SQL-запрос
		* @param sql текст одного SQL-запроса
		* @return указатель на набор данных
		*/
		MySQL::result queryRaw(const std::string &sql) {
			return queryRaw(sql.c_str(), sql.length());
		}
		
		/**
		* Выполнить произвольный SQL-запрос
		* @param sql текст одного SQL-запроса
		* @return указатель на набор данных
		*/
		MySQL::result query(const char *sql, ...);
		
		/**
		 * Вставить в таблицу набор данных
		 * @param table_name имя таблицы
		 * @param row <имя столбца> => <значение>
		 */
		bool insert(const char *table_name, row_t &row);
		
		/**
		 * Заменить набор данных в таблице
		 * @param table_name имя таблицы
		 * @param row <имя столбца> => <значение>
		 */
		bool replace(const char *table_name, row_t &row);
		
		/**
		* Вернуть ID последней вставленой записи
		*/
		int getLastInsertId();
		
		/**
		* Закрыть соединение с сервером
		*/
		void close();
	};
}

#endif // NANOSOFT_MYSQL_H
