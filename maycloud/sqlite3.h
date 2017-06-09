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

namespace nanosoft
{
	/**
	 * Класс для работы с базами Sqlite3
	 */
	class Sqlite3
	{
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
	};
}

#endif // NANOSOFT_SQLITE3_H