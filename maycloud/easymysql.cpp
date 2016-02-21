#include "maycloud/easymysql.h"
#include "maycloud/easylib.h"

#include "maycloud/easyvector.h"

//#include <std::stringList>

#include <cstdio>
#include <cstring>

bool EasyMySQL::reconnect()
{
    if ( connectType == CONNECT_INET )
    {
        printf("[EasyMySQL] connect(host=%s, user=%s, db=%s)\n", cstr(host), cstr(user), cstr(database));

        if ( mysql_real_connect(&conn, cstr(host), cstr(user), cstr(password), cstr(database), port, 0, 0) )
        {
            mysql_set_character_set(&conn, "utf8");
            return true;
        }

        fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
        return false;
    }

    if ( connectType == CONNECT_UNIX )
    {
        printf("[EasyMySQL] connect(socket=%s, user=%s, db=%s)\n", cstr(sock), cstr(user), cstr(database));
        if ( mysql_real_connect(&conn, 0, cstr(user), cstr(password), cstr(database), 0, cstr(sock), 0) )
        {
            mysql_set_character_set(&conn, "utf8");
            return true;
        }
        fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));
        return false;
    }

    return false;
}

int EasyMySQL::real_query(const char *sql, size_t len)
{
    int status = mysql_real_query(&conn, sql, len);
    if ( status == 0 ) return 0;

    fprintf(stderr, "[MySQL] %s\n", mysql_error(&conn));

    unsigned int err = mysql_errno(&conn);
    if ( err == CR_SERVER_GONE_ERROR /* || err == CR_SERVER_LOST */ )
    {
        close();
        if ( reconnect() )
        {
            status = mysql_real_query(&conn, sql, len);
            if ( status == 0 ) return 0;
            fprintf(stderr, "[MySQL] double fault: %s\n", mysql_error(&conn));
            return status;
        }
    }
    return status;
}

int EasyMySQL::real_query(const std::string &sql)
{
	return real_query(sql.c_str(), sql.length());
}

EasyMySQL::EasyMySQL()
{
    if ( ! mysql_init(&conn) )
    {
        fprintf(stderr, "[EasyMySQL] mysql_init failed\n");
    }
}

EasyMySQL::~EasyMySQL()
{
    mysql_close(&conn);
}

bool EasyMySQL::connect(const std::string &host, const std::string &database, const std::string &user, const std::string &password, int port)
{
    this->connectType = CONNECT_INET;
    this->host = host;
    this->port = port;
    this->database = database;
    this->user = user;
    this->password = password;
    return reconnect();
}

bool EasyMySQL::connectUnix(const std::string &sock, const std::string &database, const std::string &user, const std::string &password)
{
    this->connectType = CONNECT_UNIX;
    this->sock = sock;
    this->database = database;
    this->user = user;
    this->password = password;
    return reconnect();
}

std::string EasyMySQL::escape(const std::string &text)
{
    char *buf = new char[text.length() * 2 + 1];
    size_t len = mysql_real_escape_string(&conn, buf, text.c_str(), text.length());
    std::string result(buf, len);
    delete [] buf;
    return result;
}

std::string EasyMySQL::quote(const std::string &text)
{
    return '"' + escape(text) + '"';
}

EasyRow EasyMySQL::quoteRow(const EasyRow &row)
{
    EasyRow result;
    EasyRow::const_iterator it = row.begin();
    while ( it != row.end() )
    {
        result[it->first] = quote(it->second);
    }
    return result;
}

bool EasyMySQL::exec(const std::string &sql)
{
    int status = real_query(sql);
    MYSQL_RES *res = mysql_store_result(&conn);
    if ( res )
    {
        mysql_free_result(res);
    }
    return status == 0;
}


EasyRow EasyMySQL::queryOne(const std::string &sql)
{
    EasyRow row;
    real_query(sql);
    MYSQL_RES *res = mysql_use_result(&conn);
    if ( res )
    {
        int field_count = mysql_field_count(&conn);
        MYSQL_FIELD *fields = mysql_fetch_fields(res);
        MYSQL_ROW values = mysql_fetch_row(res);

        if ( values )
        {
            unsigned long *lengths = mysql_fetch_lengths(res);
            for(int i = 0; i < field_count; i++)
            {
                row[std::string(fields[i].name, fields[i].name_length)] = std::string(values[i], lengths[i]);
            }

            while ( mysql_fetch_row(res) ) ;
        }
        mysql_free_result(res);
    }
    return row;
}

EasyResultSet EasyMySQL::queryAll(const std::string &sql)
{
    EasyRow row;
    EasyResultSet result;
    real_query(sql);
    MYSQL_RES *res = mysql_use_result(&conn);
    if ( res )
    {
        MYSQL_FIELD *fields = mysql_fetch_fields(res);
        int field_count = mysql_num_fields(res);
        EasyVector f;
        for(int i = 0; i < field_count; i ++)
        {
            f.push_back(std::string(fields[i].name, fields[i].name_length));
        }

        MYSQL_ROW values;
        while ( values = mysql_fetch_row(res) )
        {
            unsigned long *lengths = mysql_fetch_lengths(res);

            EasyRow row;
            for(int i = 0; i < field_count; i++)
            {
                row[ f[i] ] = std::string(values[i], lengths[i]);
            }

            result.push_back(row);
        }

        mysql_free_result(res);
    }
    return result;
}

bool EasyMySQL::insert(const std::string &table, const EasyRow &row)
{
    return exec("INSERT INTO " + table + " (" + implode(", ", row.keys()) + ") VALUES (" + implode(", ", row.values()) + ")");
}

bool EasyMySQL::insertQuoted(const std::string &table, const EasyRow &row)
{
    return insert(table, quoteRow(row));
}

bool EasyMySQL::replace(const std::string &table, const EasyRow &row)
{
    return exec("REPLACE INTO " + table + " (" + implode(", ", row.keys()) + ") VALUES (" + implode(", ", row.values()) + ")");
}

bool EasyMySQL::replaceQuoted(const std::string &table, const EasyRow &row)
{
    return replace(table, quoteRow(row));
}

void EasyMySQL::close()
{
    mysql_close(&conn);
    if ( ! mysql_init(&conn) )
    {
        fprintf(stderr, "[EasyMySQL] mysql_init failed\n");
    }
}
