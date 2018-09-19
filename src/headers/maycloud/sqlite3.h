#ifndef NANOSOFT_SQLITE3_H
#define NANOSOFT_SQLITE3_H

#include <maycloud/config.h>
#include <maycloud/error.h>

#include <sqlite3.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <map>
#include <vector>

namespace nanosoft
{
	/**
	 * Класс для работы с базами Sqlite3
	 */
	class Sqlite3
	{
	public:
		typedef std::vector< std::map<std::string, std::string> > result;
		
		typedef std::map<std::string, std::string> row_t;
		typedef std::map<std::string, std::string>::iterator row_iterator;
	protected:
		/**
		 * Путь к базе данных Sqlite3
		 */
		std::string path;

		/**
		 * Соединение с базой Sqlite3
		 */
		sqlite3* db;

		/**
		 * Открыть базу данных

		 * @param path - путь к базе данных
		 * @return TRUE - база открыта, FALSE - ошибка открытия
		 */
		bool reopen();

		static int callback(void* data, int row_count, char** field_value, char** field_name);
	public:
		/**
		 * Конструктор
		 */
		Sqlite3();

		/**
		 * Деструктор
		 */
		~Sqlite3();

		/**
		 * Открыть базу данных

		 * @param path - путь к базе данных
		 * @return TRUE - база открыта, FALSE - ошибка открытия
		 */
		bool open(std::string path);

		/**
		 * Закрыть соединение с базой
		 */
		void close();

		/**
		 * Экранировать строку и заключить её в кавычки
		 */
		std::string quote(const std::string &text);

		/**
		 * Выполнить произвольный SQL-запрос
		 * @param sql текст одного SQL-запроса
		 * @param len длина запроса
		 * @return набор данных
		 */
		result queryRaw(const char *sql, size_t len);
		
		/**
		 * Выполнить произвольный SQL-запрос
		 * @param sql текст одного SQL-запроса
		 * @return набор данных
		 */
		result queryRaw(const std::string &sql) {
			return queryRaw(sql.c_str(), sql.length());
		}
		
		/**
		 * Выполнить произвольный SQL-запрос
		 * @param sql текст одного SQL-запроса
		 * @return набор данных
		 */
		result query(const char *sql, ...);

		/**
		 * Вставить в таблицу набор данных
		 * @param table_name имя таблицы
		 * @param row <имя столбца> => <значение>
		 */
		void insert(const char *table_name, row_t &row);
		
		/**
		 * Заменить набор данных в таблице
		 * @param table_name имя таблицы
		 * @param row <имя столбца> => <значение>
		 */
		void replace(const char *table_name, row_t &row);
	};
}

#endif // NANOSOFT_SQLITE3_H