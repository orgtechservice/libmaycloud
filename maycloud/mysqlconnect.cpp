/**
 * MYSQLConnect
 * Revision on 08.05.2015
 * 
 * (c) Michael Solomatin
 * 
 * Простой класс из другого движка.
 * Сейчас оставлен для совсем простых программ
 * и для упрощения будущей интеграции сервисов из другого движка.
 * В дальнейшем может быть выпилен.
 */

#include <maycloud/mysqlconnect.h>

#define ERROR_MYSQL 0
#define ERROR_OTHER 1

#define LOG_FN "/var/log/mysqlconnect_error.log"

/**
 * Конструктор, деструктор
 */

MYSQLConnect::MYSQLConnect()
{
	res = NULL;
	mysqlIsConnected = false;
}

MYSQLConnect::~MYSQLConnect()
{
	
}

/**
 * Запись ошибки в лог
 */
void MYSQLConnect::mysqlLogError(unsigned int error)
{
	fprintf(stderr, "[MYSQLConnect]: %s\n", mysql_error(mysql));
}
	
/**
 * Обработка потери соединения
 */
void MYSQLConnect::mysqlCrash()
{
	mysqlLogError(ERROR_MYSQL);
	mysql_close(mysql);
	mysqlIsConnected = false;
}

bool MYSQLConnect::mysqlConnect(const char* host, const char* user, const char* password, const char* dbname)
{
	mysql = mysql_init(NULL);
	
	
	if(!mysql_real_connect(mysql, host, user, password, NULL, 0, NULL, 0))
	{
		mysqlCrash();
		return false;
	}
	
	if(mysql_select_db(mysql, dbname) != 0)
	{
		mysqlCrash();
		return false;
	}
	
	strcpy(mysqlParams.host, host);
	strcpy(mysqlParams.user, user);
	strcpy(mysqlParams.password, password);
	strcpy(mysqlParams.dbname, dbname);
	
	mysqlIsConnected = true;
	return true;
}

bool MYSQLConnect::mysqlResetConnection()
{
	if(!mysqlIsConnected)
	{
		if(!mysqlConnect(mysqlParams.host, mysqlParams.user, mysqlParams.password, mysqlParams.dbname)) return false;
	}
	
	return true;
}

/**
 * Указать существующее соединение
 */
bool MYSQLConnect::mysqlSetConnection(MYSQL* sql)
{
	mysql = sql;
	if(!mysql)
	{
		mysqlIsConnected = false;
		return false;
	}
	
	mysqlIsConnected = true;
	return true;
}

/**
 * Закрыть соединение с MySQL
 */
void MYSQLConnect::mysqlClose()
{
	mysql_close(mysql);
	mysqlIsConnected = false;
}

/**
 * Выполнить запрос MySQL
 */
bool MYSQLConnect::mysqlQuery(const char* Query)
{
	if(mysql_query(mysql, Query) != 0)
	{
		mysqlCrash();
		return false;
	}
	
	return true;
}

bool MYSQLConnect::mysqlQueryf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	vsnprintf(Query, 512, fmt, args);
	
	va_end(args);
	
	if(mysql_query(mysql, Query) != 0)
	{
		mysqlCrash();
		return false;
	}
	
	return true;
}

bool MYSQLConnect::mysqlBegin()
{
	if(!mysqlQuery("BEGIN")) return false;
	
	return true;
}

bool MYSQLConnect::mysqlCommit()
{
	if(!mysqlQuery("COMMIT")) return false;
	
	return true;
}

/**
 * Загрузить данные из базы
 */
bool MYSQLConnect::mysqlLoadFromDB() { }

bool MYSQLConnect::mysqlStoreResult()
{
	res = mysql_store_result(mysql);
	if(!res) return false;
	return true;
}

MYSQL_ROW MYSQLConnect::mysqlFetchRow()
{
	return mysql_fetch_row(res);
}

void MYSQLConnect::mysqlFreeResult()
{
	mysql_free_result(res);
}