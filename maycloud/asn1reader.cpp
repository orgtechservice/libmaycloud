#include <maycloud/asn1reader.h>

ASN1Reader::ASN1Reader()
{
	
}

ASN1Reader::~ASN1Reader()
{
	
}

/**
 * Прочитать заголовок тега ASN.1
 */
bool ASN1Reader::readHeader(asn1_header_t &h, const unsigned char *&data, const unsigned char *limit)
{
	const unsigned char *p = data;
	
	if ( (p + 1) >= limit ) return false;
	h.type = p[0];
	
	if ( p[1] & 0x80 )
	{
		int c = p[1] & 0x7F;
		if ( c > sizeof(h.length) ) return false;
		h.data = p + 2 + c;
		if ( h.data > limit ) return false;
		unsigned int l = 0;
		for(int i = 0; i < c; i++) l = l * 256 + p[i+2];
		h.length = l;
		h.limit = h.data + l;
	}
	else
	{
		h.length = p[1];
		h.data = p + 2;
		h.limit = h.data + h.length;
	}
	
	if ( h.limit > limit ) return false;
	data = h.data;
	return true;
}

/**
* Прочитать ASN.1 TIMETICK
*/
bool ASN1Reader::readTimeticks(short &result, const unsigned char *&data, const unsigned char *limit)
{
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x43 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	short value = 0;
	while ( p < end ) value = value * 256 + *p++;
	
	result = value;
	data = end;
	
	return true;
}

/**
 * Прочитать ASN.1 INTEGER
 */
bool ASN1Reader::readInt(int &result, const unsigned char *&data, const unsigned char *limit)
{
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x02 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	unsigned char value[4] = { 0, 0, 0, 0 };
	int i = 3;
	while ( p < end ) value[i--] = *p++;

	result = 0;
	if(i == 2)
	{
		signed char* cval = (signed char*) &value[i + 1];
		result = (int) *cval;
	}
	if(i == 1)
	{
		signed short* sval = (signed short*) &value[i + 1];
		result = (int) *sval;
	}
	if(i == 0 || i == -1)
	{
		int* ival = (int*) &value[i + 1];
		result = *ival;
	}		
	
	data = end;
	
	return true;
}

/**
 * Прочитать SNMP COUNTER64
 */
bool ASN1Reader::readCounter64(unsigned long long int &result, const unsigned char *&data, const unsigned char *limit)
{
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x46 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	unsigned long long int value = 0;
	while ( p < end ) value = value * 256 + *p++;
	
	result = value;
	data = end;
	
	return true;
}

/**
 * Прочитать ASN.1 GAUGE32 (0x42) и SNMP COUNTER (0x41)
 * 
 * Эти типы аналогичны, поэтому читать их можно в одном обработчике
 */
bool ASN1Reader::readCounterGauge32(unsigned int &result, const unsigned char *&data, const unsigned char *limit)
{
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x41 && h.type != 0x42 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	unsigned int value = 0;
	while ( p < end ) value = value * 256 + *p++;
	
	result = value;
	data = end;
	
	return true;
}

/**
 * Прочитать ASN.1 OCTET STRING
 * @param result минимальный размер 256 байт (255 символов + 1 символ конца строки)
 */
bool ASN1Reader::readString(char *result, const unsigned char *&data, const unsigned char *limit)
{
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x04 ) return false;
	// TODO
	if ( h.length > 255 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	while ( p < end ) *result++ = *p++;
	*result = 0;
	
	data = end;
	
	return true;
}

/**
 * Прочитать ASN.1 NULL
 */
bool ASN1Reader::readNULL(const unsigned char *&data, const unsigned char *limit)
{
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x05 ) return false;
	
	data = h.limit;
	
	return true;
}

/**
 * Прочитать ASN.1 OID
 */
bool ASN1Reader::readOID(int *result, size_t &len, const unsigned char *&data, const unsigned char *limit)
{
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x06 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	int item = 0;
	while ( p < end )
	{
		unsigned char c = *p++;
		unsigned char d = c & 0x7F;
		item = item * 128 + d;
		if ( (c & 0x80) == 0 ) break;
	}
	result[0] = item / 40;
	result[1] = item % 40;
	len = 2;
	
	while ( p < end )
	{
		if ( len > ASN1_OID_MAXLEN ) return false;
		item = 0;
		while ( p < limit )
		{
			unsigned char c = *p++;
			unsigned char d = c & 0x7F;
			item = item * 128 + d;
			if ( (c & 0x80) == 0 ) break;
		}
		result[len] = item;
		len++;
	}
	
	data = end;
	
	return true;
}

