#ifndef MYSQL_CONNECT_H
#define MYSQL_CONNECT_H

/**
 * MYSQLConnect
 * Revision on 11.09.2014
 * 
 * (c) Michael Solomatin
 */

#include <maycloud/configurer.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <mysql/mysql.h>

struct mysql_params_t
{
	char host[256];
	char user[256];
	char password[256];
	char dbname[256];
};

class MYSQLConnect: public Configurer
{
protected:
	MYSQL* mysql;
	bool mysqlIsConnected; // Соединение с базой установлено
	mysql_params_t mysqlParams; // Сюда сохраняются параметры для переподключения
	
	char Query[1024]; // Запрос
	
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
	
	/**
	 * Загрузить данные из базы
	 */
	virtual bool mysqlLoadFromDB();
	
	bool mysqlBegin();
	bool mysqlCommit();
	
	/**
	 * Загрузка конфигурации из файла
	 */
	mysql_params_t* loadConfig(const char* fn);
	
	/**
	 * Загрузка конфигурации из аргументов
	 */
	mysql_params_t* setConfig(const char* host, const char* user, const char* password, const char* dbname);
};

#endif // MYSQL_CONNECT_H
