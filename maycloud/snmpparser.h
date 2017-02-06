#ifndef SNMP_PARSER_H
#define SNMP_PARSER_H

#include <maycloud/asn1reader.h>

#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <time.h>
#include <sys/time.h>

/**
* Парсер SNMP пакетов
*/
class SNMPParser: public ASN1Reader
{
protected:
	/**
	* Обработчик чтения переменной
	*/
	virtual void onGetValue(const int *oid, size_t len);
	
	/**
	* Обработчик чтения переменной
	*/
	virtual void onResponseValueTimeticks(const char *host, unsigned int host_ip, const int *oid, size_t len, short value);
	virtual void onResponseValueInt(const char *host, unsigned int host_ip, const int *oid, size_t len, int value);
	virtual void onResponseValueCounterGauge32(const char *host, unsigned int host_ip, const int *oid, size_t len, unsigned int value);
	virtual void onResponseValueCounter64(const char *host, unsigned int host_ip, const int *oid, size_t len, unsigned long long int value);
	virtual void onResponseValueOID(const char *host, unsigned int host_ip, const int *oid, size_t len, int* value, size_t vlen);
	virtual void onResponseValueOctetString(const char *host, unsigned int host_ip, const int *oid, size_t len, char* value);
	
	/**
	* GET-request oid binding
	*/
	bool parseGetValue(const unsigned char *&data, const unsigned char *limit);
	
	/**
	* GET-request oid bindings
	*/
	bool parseGetBindings(const unsigned char *&data, const unsigned char *limit);
	
	/**
	* GET-request
	*/
	bool parseGetRequest(const unsigned char *data, const unsigned char *limit);
	
	/**
	* GET-response oid binding
	*/
	bool parseResponseValue(const char *host, unsigned int host_ip, const unsigned char *&data, const unsigned char *limit);
	
	/**
	* GET-response oid bindings
	*/
	bool parseResponseBindings(const char *host, unsigned int host_ip, const unsigned char *&data, const unsigned char *limit);
	
	/**
	* GET-response
	*/
	bool parseResponse(const char *host, unsigned int host_ip, const unsigned char *data, const unsigned char *limit);
	
	/**
	 * TRAP oid binding
	 */
	bool parseTrapValue(const char *host, unsigned int host_ip, const unsigned char *&data, const unsigned char *limit);
	
	/**
	 * TRAP oid bindings
	 */
	bool parseTrapBindings(const char *host, unsigned int host_ip, const unsigned char *&data, const unsigned char *limit);
	
	/**
	 * TRAP
	 */
	bool parseTrap(const char *host, unsigned int host_ip, const unsigned char *data, const unsigned char *limit);
public:
	/**
	* Версия пакета
	*/
	int version;
	
	/**
	* Community string
	*/
	char community[ASN1_STRING_MAXLEN+1];
	
	/**
	* ID запроса
	*/
	int requestID;
	
	int errorStatus;
	
	int errorIndex;
	
	/**
	* Конструктор
	*/
	SNMPParser();
	
	/**
	* Деструктор
	*/
	virtual ~SNMPParser();
	
	/**
	* Парсинг пакета
	*/
	bool parse(const char *host, unsigned int host_ip, const unsigned char *data, const unsigned char *limit);
	
	/**
	* Парсинг пакета
	*/
	bool parse(const char *host, unsigned int host_ip, const unsigned char *data, size_t size);
};

#endif // SNMP_PARSER_H
