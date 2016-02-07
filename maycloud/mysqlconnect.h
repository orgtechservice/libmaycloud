#ifndef MYSQL_CONNECT_H
#define MYSQL_CONNECT_H

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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <mysql/mysql.h>

struct mysql_params_t
{
	char host[256];
	char user[256];
	char password[256];
	char dbname[256];
};

class MYSQLConnect
{
protected:
	MYSQL* mysql;
	MYSQL_RES* res;
	
	bool mysqlIsConnected; // Соединение с базой установлено
	mysql_params_t mysqlParams; // Сюда сохраняются параметры для переподключения
	
	char Query[512]; // Запрос
	
	/**
	 * Обработка потери соединения
	 */
	void mysqlCrash();
	
	/**
	 * Запись ошибки в лог
	 */
	void mysqlLogError(unsigned int error);
public:
	/**
	 * Конструктор, деструктор
	 */
	MYSQLConnect();
	virtual ~MYSQLConnect();
	
	/**
	 * Подключиться к базе данных MySQL
	 */
	bool mysqlConnect(const char* host, const char* user, const char* password, const char* dbname);
	
	/**
	 * Переподключиться к базе данных MySQL
	 */
	bool mysqlResetConnection();
	
	/**
	 * Указать существующее соединение
	 */
	bool mysqlSetConnection(MYSQL* sql);
	
	/**
	 * Закрыть соединение с MySQL
	 */
	void mysqlClose();
	
	/**
	 * Выполнить запрос MySQL
	 */
	bool mysqlQuery(const char* Query);
	bool mysqlQueryf(const char* fmt, ...);
	bool mysqlStoreResult();
	MYSQL_ROW mysqlFetchRow();
	void mysqlFreeResult();
	
	/**
	 * Загрузить данные из базы
	 */
	virtual bool mysqlLoadFromDB();
	
	bool mysqlBegin();
	bool mysqlCommit();
};

#endif // MYSQL_CONNECT_H