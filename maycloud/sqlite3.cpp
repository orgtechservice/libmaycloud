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
}