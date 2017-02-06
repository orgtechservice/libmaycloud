#ifndef SNMP_BUILDER_H
#define SNMP_BUILDER_H

#include <string.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>

#include <pcre.h>


/**
* Класс пакета SNMP
*/
class SNMPBuilder
{
protected:
	size_t capacity;
	unsigned char *buffer;
	unsigned char *offset;
	unsigned char *cmd_offset;
	unsigned char *requestID_offset;
	unsigned char *oid_offset;
public:
	SNMPBuilder(size_t acapacity = 4096);
	~SNMPBuilder();
	
	/**
	* Иницировать новый пакет
	*/
	bool reset(const char *community);
	
	/**
	* Установить тип пакета
	*/
	bool setCommand(unsigned char type);
	
	/**
	* Установить requestID
	*/
	bool setRequestID(unsigned int value);
	
	/**
	* Добавить OID со значением NULL
	*/
	bool addNull(const int *oid, size_t len);
	
	/**
	* Завершить запись пакета
	*/
	bool commit();
	
	/**
	* Вернуть указатель на данные
	*/
	const unsigned char *getData() const { return buffer; }
	
	/**
	* Вернуть размер пакета
	*/
	size_t getDataSize() const { return offset - buffer; }
};

#endif // SNMP_BUILDER_H
