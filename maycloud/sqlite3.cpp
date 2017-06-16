#include <maycloud/sqlite3.h>
#include <maycloud/debug.h>

namespace nanosoft
{
	/**
	 * Конструктор
	 */
	Sqlite3::Sqlite3()
	{

	}

	/**
	 * Деструктор
	 */
	Sqlite3::~Sqlite3()
	{
		close();
	}

	int Sqlite3::callback(void* data, int row_count, char** field_value, char** field_name)
	{
		result* r = static_cast<result*>(data);
		for(int i = 0; i < row_count; i++)
		{
			r->insert(result_pair(field_name[i], field_value[i] ? field_value[i] : ""));
		}
		return 0;
	}

	/**
	 * Открыть базу данных
	 * @param path - путь к базе данных
	 * @return TRUE - база открыта, FALSE - ошибка открытия
	 */
	bool Sqlite3::reopen()
	{
		if(sqlite3_open(path.c_str(), &db) != SQLITE_OK)
		{
			fprintf(stderr, "[Sqlite3] %s\n", sqlite3_errmsg(db));
			return false;  
		}
		return true;
	}

	/**
	 * Открыть базу данных
	 * @param path - путь к базе данных
	 * @return TRUE - база открыта, FALSE - ошибка открытия
	 */
	bool Sqlite3::open(std::string path)
	{
		this->path = path;
		return reopen();
	}

	/**
	 * Закрыть соединение с базой
	 */
	void Sqlite3::close()
	{
		sqlite3_close(db);
	}

	/**
	 * Экранировать строку и заключить её в кавычки
	 */
	std::string Sqlite3::quote(const std::string &text)
	{
		return '"' + text + '"';
	}

	/**
	 * Выполнить произвольный SQL-запрос
	 * @param sql текст одного SQL-запроса
	 * @param len длина запроса
	 * @return набор данных
	 */
	Sqlite3::result Sqlite3::queryRaw(const char *sql, size_t len)
	{
		if ( DEBUG::DUMP_SQL )
		{
			std::string sql_dump(sql, len);
			printf("DUMP SQL[%s]: \033[22;31m%s\033[0m\n", this->path.c_str(), sql_dump.c_str());
		}

		result r;
		char* err = 0;
		int status = sqlite3_exec(db, sql, callback, (void*) &r, &err);
		if(status != SQLITE_OK)
		{
			close();
			if(reopen())
			{
				status = sqlite3_exec(db, sql, callback, (void*) &r, &err);
				if(status != SQLITE_OK) fprintf(stderr, "[Sqlite3] %s\n", err);
			}
		}

		return r;
	}

	/**
	 * Выполнить произвольный SQL-запрос
	 * @param sql текст одного SQL-запроса
	 * @return набор данных
	 */
	Sqlite3::result Sqlite3::query(const char *sql, ...)
	{
		char *buf;
		va_list args;
		va_start(args, sql);
		int len = vasprintf(&buf, sql, args);
		va_end(args);
		
		result r = queryRaw(buf, len);
		
		free(buf);
		
		return r;
	}

	/**
	 * Вставить в таблицу набор данных
	 * @param table_name имя таблицы
	 * @param row <имя столбца> => <значение>
	 */
	void Sqlite3::insert(const char *table_name, row_t &row)
	{
		std::string insert = "INSERT INTO " + std::string(table_name) + "(";
		row_iterator it = row.begin();
		insert += it->first;
		it++;
		while(it != row.end())
		{
			insert += (", " + it->first);
			it++;
		}
		insert += ") VALUES(";
		it = row.begin();
		insert += quote(it->second);
		it++;
		while(it != row.end())
		{
			insert += (", " + quote(it->second));
			it++;
		}
		insert += ");";
		
		query(insert.c_str());
	}
	
	/**
	 * Заменить набор данных в таблице
	 * @param table_name имя таблицы
	 * @param row <имя столбца> => <значение>
	 */
	void Sqlite3::replace(const char *table_name, row_t &row)
	{
		std::string insert = "REPLACE INTO " + std::string(table_name) + "(";
		row_iterator it = row.begin();
		insert += it->first;
		it++;
		while(it != row.end())
		{
			insert += (", " + it->first);
			it++;
		}
		insert += ") VALUES(";
		it = row.begin();
		insert += quote(it->second);
		it++;
		while(it != row.end())
		{
			insert += (", " + quote(it->second));
			it++;
		}
		insert += ");";
		
		query(insert.c_str());
	}
}