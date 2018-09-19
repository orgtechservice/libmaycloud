#ifndef ASN1_READER_H
#define ASN1_READER_H

#define ASN1_OID_MAXLEN 128
#define ASN1_STRING_MAXLEN 255

#include <stdio.h>

#include <sys/types.h>

/**
* Заголовок тега ASN.1
*/
struct asn1_header_t
{
	unsigned int type;
	unsigned int length;
	const unsigned char *data;
	const unsigned char *limit;
};

/**
* Парсер ASN.1
*/
class ASN1Reader
{
public:
	/**
	* Конструктор
	*/
	ASN1Reader();
	
	/**
	* Деструктор
	*/
	~ASN1Reader();
	
	/**
	* Прочитать заголовок тега ASN.1
	*/
	static bool readHeader(asn1_header_t &h, const unsigned char *&data, const unsigned char *limit);
	
	/**
	* Прочитать ASN.1 TIMETICK
	*/
	static bool readTimeticks(short &result, const unsigned char *&data, const unsigned char *limit);
	
	/**
	* Прочитать ASN.1 INTEGER
	*/
	static bool readInt(int &result, const unsigned char *&data, const unsigned char *limit);
	
	/**
	 * Прочитать SNMP COUNTER64
	 */
	static bool readCounter64(unsigned long long int &result, const unsigned char *&data, const unsigned char *limit);
	
	/**
	 * Прочитать ASN.1 GAUGE32 (0x42) и SNMP COUNTER (0x41)
	 * 
	 * Эти типы аналогичны, поэтому читать их можно в одном обработчике
	 */
	static bool readCounterGauge32(unsigned int &result, const unsigned char *&data, const unsigned char *limit);
	
	/**
	* Прочитать ASN.1 OCTET STRING
	* @param result минимальный размер 256 байт (255 символов + 1 символ конца строки)
	*/
	static bool readString(char *result, const unsigned char *&data, const unsigned char *limit);
	
	/**
	* Прочитать ASN.1 NULL
	*/
	static bool readNULL(const unsigned char *&data, const unsigned char *limit);
	
	/**
	* Прочитать ASN.1 OID
	*/
	static bool readOID(int *result, size_t &len, const unsigned char *&data, const unsigned char *limit);
};

#endif // ASN1_READER_H
