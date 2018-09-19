#include <maycloud/mysql.h>
#include <maycloud/debug.h>

namespace nanosoft
{
	/**
	* Конструктор
	*/
	MySQL::MySQL()
	{
	}
	
	/**
	* Деструктор
	*/
	MySQL::~MySQL()
	{
		close();
	}
	
	/**
	* Соединение сервером
	*
	* В случае неудачи выводит в stderr сообщение об ошибке и возращает FALSE
	* @return TRUE - соединение установлено, FALSE ошибка соединения
	*/
	bool MySQL::reconnect()
	{
		switch ( connectType )
		{
		case CONNECT_INET:
			if ( mysql_real_connect(mysql_init(&conn), host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, 0, 0) ) break;
			fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
			return false;
		case CONNECT_UNIX:
			if ( mysql_real_connect(mysql_init(&conn), 0, user.c_str(), password.c_str(), database.c_str(), 0, sock.c_str(), 0) ) break;
			fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
			return false;
		default:
			return false;
		}
		
		// кодировка только UTF-8
		mysql_real_query(&conn, "SET NAMES UTF8", sizeof("SET NAMES UTF8"));
		return true;
	}
	
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
	bool MySQL::connect(const std::string &host, const std::string &database, const std::string &user, const std::string &password, int port)
	{
		this->connectType = CONNECT_INET;
		this->host = host.c_str();
		this->port = port;
		this->database = database.c_str();
		this->user = user.c_str();
		this->password = password.c_str();
		return reconnect();
	}
	
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
	bool MySQL::connectUnix(const std::string &sock, const std::string &database, const std::string &user, const std::string &password)
	{
		this->connectType = CONNECT_UNIX;
		this->sock = sock.c_str();
		this->database = database.c_str();
		this->user = user.c_str();
		this->password = password.c_str();
		return reconnect();
	}
	
	/**
	* Экранировать строку
	*/
	std::string MySQL::escape(const std::string &text)
	{
		char *buf = new char[text.length() * 2 + 1];
		size_t len = mysql_real_escape_string(&conn, buf, text.c_str(), text.length());
		std::string result(buf, len);
		delete [] buf;
		return result;
	}
	
	/**
	* Экранировать строку и заключить её в кавычки
	*/
	std::string MySQL::quote(const std::string &text)
	{
		return '"' + escape(text) + '"';
	}
	
	/**
	* Выполнить произвольный SQL-запрос
	* @param sql текст одного SQL-запроса
	* @param len длина запроса
	* @return указатель на набор данных
	*/
	MySQL::result MySQL::queryRaw(const char *sql, size_t len)
	{
		if ( DEBUG::DUMP_SQL )
		{
			std::string sql_dump(sql, len);
			printf("DUMP SQL[%s]: \033[22;31m%s\033[0m\n", this->database.c_str(), sql_dump.c_str());
		}
		
		int status = mysql_real_query(&conn, sql, len);
		if ( status )
		{
			fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
			
			unsigned int err = mysql_errno(&conn);
			if ( err == CR_SERVER_GONE_ERROR || err == CR_SERVER_LOST )
			{
				close();
				if ( reconnect() )
				{
					status = mysql_real_query(&conn, sql, len);
					if ( status )
					{
							fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
							return 0;
					}
				}
			}
			else
			{
				return 0;
			}
		}
		
		MYSQL_RES *res = mysql_store_result(&conn);
		if ( res || mysql_field_count(&conn) > 0 ) {
			ResultSet *r = new ResultSet;
			r->res = res;
			r->field_count = mysql_field_count(&conn);
			r->fields = mysql_fetch_fields(res);
			r->values = mysql_fetch_row(res);
			if ( r->values ) r->lengths = mysql_fetch_lengths(res);
			
			return r;
		}
		
		if ( mysql_errno(&conn) ) {
			fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
		}
		
		return 0;
	}
	
	/**
	* Выполнить произвольный SQL-запрос
	*/
	MySQL::result MySQL::query(const char *sql, ...)
	{
		char *buf;
		va_list args;
		va_start(args, sql);
		int len = vasprintf(&buf, sql, args);
		va_end(args);
		
		MySQL::result r = queryRaw(buf, len);
		
		free(buf);
		
		return r;
	}
	
	/**
	* Вернуть ID последней вставленой записи
	*/
	int MySQL::getLastInsertId()
	{
		return mysql_insert_id(&conn);
	}
	
	/**
	* Закрыть соединение с сервером
	*/
	void MySQL::close()
	{
		mysql_close(&conn);
	}
	
	void MySQL::result::next()
	{
		if ( res && res->res ) {
			res->values = mysql_fetch_row(res->res);
			if ( res->values ) res->lengths = mysql_fetch_lengths(res->res);
		}
	}
	
	int MySQL::result::indexOf(const char *name)
	{
		MYSQL_FIELD *p = res->fields;
		for(size_t i = 0; i < res->field_count; i++, p++)
		{
			if ( strcmp(name, p->name) == 0 ) return i;
		}
		fprintf(stderr, "[MySQL] field not found: %s\n", name);
		return 0;
	}
	
	std::string MySQL::result::operator [] (size_t num)
	{
		return std::string(res->values[num], res->lengths[num]);
	}
	
	std::string MySQL::result::operator [] (const char *name)
	{
		int num = indexOf(name);
		return std::string(res->values[num], res->lengths[num]);
	}
	
	bool MySQL::result::isNull(size_t num)
	{
		return res->values[num] == 0;
	}
	
	bool MySQL::result::isNull(const char *name)
	{
		int num = indexOf(name);
		return res->values[num] == 0;
	}
	
	/**
	 * Вставить в таблицу набор данных
	 * @param table_name имя таблицы
	 * @param row <имя столбца> => <значение>
	 */
	bool MySQL::insert(const char *table_name, row_t &row)
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
		
		return true;
	}
	
	/**
	 * Заменить набор данных в таблице
	 * @param table_name имя таблицы
	 * @param row <имя столбца> => <значение>
	 */
	bool MySQL::replace(const char *table_name, row_t &row)
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
		
		return true;
	}
	
	/**
	* Удалить набор данных
	*/
	void MySQL::result::free()
	{
		mysql_free_result(res->res);
		delete res;
	}
}
