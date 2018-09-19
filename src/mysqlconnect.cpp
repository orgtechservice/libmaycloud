/**
 * MYSQLConnect
 * Revision on 11.09.2014
 * 
 * (c) Michael Solomatin
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
	time_t ts = time(0);
	struct tm* tmt = localtime(&ts);
	char ct[40];
	strcpy(ct, asctime(tmt));
	
	FILE* f = fopen(LOG_FN, "a");
	if(!f)
	{
		fprintf(stderr, "MYSQLConnect can not open log file: %s!\n", LOG_FN);
		return;
	}
	switch(error)
	{
		case ERROR_MYSQL:
			fprintf(f, "%s: MYSQLConnect %s\n", ct, mysql_error(mysql));
			break;
		case ERROR_OTHER:
			fprintf(f, "%s: MYSQLConnect can not open configuration file\n", ct);
			break;
	}
	fclose(f);
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
	
	mysql_set_character_set(mysql, "utf8");
	
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
bool MYSQLConnect::mysqlLoadFromDB() { return true; }

/**
 * Загрузка конфигурации из файла
 */
mysql_params_t* MYSQLConnect::loadConfig(const char* fn)
{
	memset(&mysqlParams, 0, sizeof(mysql_params_t));
	
	if( !loadFile(fn) )
	{
		mysqlLogError(ERROR_OTHER);
		return &mysqlParams;
	}
	
	sscanf(searchParam("MYSQL_HOST").c_str(), "%s", mysqlParams.host);
	sscanf(searchParam("MYSQL_USER").c_str(), "%s", mysqlParams.user);
	sscanf(searchParam("MYSQL_PASSWORD").c_str(), "%s", mysqlParams.password);
	sscanf(searchParam("MYSQL_DBNAME").c_str(), "%s", mysqlParams.dbname);
	
	return &mysqlParams;
}

/**
 * Загрузка конфигурации из аргументов
 */
mysql_params_t* MYSQLConnect::setConfig(const char* host, const char* user, const char* password, const char* dbname)
{
	strcpy(mysqlParams.host, host);
	strcpy(mysqlParams.user, user);
	strcpy(mysqlParams.password, password);
	strcpy(mysqlParams.dbname, dbname);
	
	return &mysqlParams;
}
