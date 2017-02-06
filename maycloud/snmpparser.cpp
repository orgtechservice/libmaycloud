#include <maycloud/snmpparser.h>

SNMPParser::SNMPParser()
{
}

SNMPParser::~SNMPParser()
{
}

/**
* Обработчик чтения переменной
*/
void SNMPParser::onGetValue(const int *oid, size_t len)
{
	printf("          GET(%d.%d", oid[0], oid[1]);
	for(size_t i = 2; i < len; i++) printf(".%d", oid[i]);
	printf(")\n");
}

/**
* GET-request oid binding
*/
bool SNMPParser::parseGetValue(const unsigned char *&data, const unsigned char *limit)
{
	printf("        GET value\n");
	
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	int oid[ASN1_OID_MAXLEN];
	size_t len;
	if ( ! readOID(oid, len, p, end) ) return false;
	/*printf("          OID: %d.%d", oid[0], oid[1]);
	for(size_t i = 2; i < len ; i++)
	{
		printf(".%d", oid[i]);
	}
	printf("\n");*/
	
	if ( ! readNULL(p, end) ) return false;
	
	onGetValue(oid, len);
	
	data = end;
	
	return true;
}

/**
* GET-request oid bindings
*/
bool SNMPParser::parseGetBindings(const unsigned char *&data, const unsigned char *limit)
{
	//printf("      GET bindings, 0x%02X\n", data[0]);
	
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	while ( p < end )
	{
		if ( ! parseGetValue(p, end) ) return false;
	}
	
	data = end;
	
	return true;
}

/**
* GET-request
*/
bool SNMPParser::parseGetRequest(const unsigned char *data, const unsigned char *limit)
{
	//printf("  parse SNMP GET-request\n");
	
	if ( ! readInt(requestID, data, limit) ) return false;
	//printf("    requestID: %d\n", requestID);
	
	if ( ! readInt(errorStatus, data, limit) ) return false;
	//printf("    errorStatus: %d\n", errorStatus);
	
	if ( ! readInt(errorIndex, data, limit) ) return false;
	//printf("    errorIndex: %d\n", errorIndex);
	
	if ( ! parseGetBindings(data, limit) ) return false;
	
	return true;
}

unsigned int last = 0;
unsigned int min = 0, max = 0, avg = 0, count = 0;
unsigned long long sum = 0; 

/**
* Обработчик чтения переменной
*/
void SNMPParser::onResponseValueInt(const char *host, unsigned int host_ip, const int *oid, size_t len, int value)
{
	//printf("          GET(%d.%d", oid[0], oid[1]);
	//for(size_t i = 2; i < len; i++) printf(".%d", oid[i]);
	struct timeval tv;
	gettimeofday(&tv, 0);
	int tm = (tv.tv_sec % (1024*60)) * 4 + tv.tv_usec / 250000;
	
	unsigned int v = static_cast<unsigned int>(value);
	unsigned int diff = v - last;
	last = v;
	
	if ( (tm % (60*4)) == 0 )
	{
		//unsigned int avg = sum / count;
		float f_min = (((min * 4.0) / 1024.0) / 1024.0) * 8;
		float f_max = (((max * 4.0) / 1024.0) / 1024.0) * 8;
		float f_avg = count > 0 ? ((((sum * 4.0 * 8.0) / 1024.0) / 1024.0) / count) : 0;
		printf("%s min: %.2f, avg: %.2f, max: %.2f, count: %d\n", host, f_min, f_avg, f_max, count);
		//printf("----\n");
		min = diff;
		max = diff;
		sum = 0;
		count = 0;
	}
	
	
	if ( diff < min ) min = diff;
	if ( diff > max ) max = diff;
	sum += diff;
	count++;
	
	printf("%s: %d, %u\n", host, tm, value);
}

void SNMPParser::onResponseValueCounterGauge32(const char *host, unsigned int host_ip, const int *oid, size_t len, unsigned int value)
{
	// ------ TODO ------
}

void SNMPParser::onResponseValueCounter64(const char *host, unsigned int host_ip, const int *oid, size_t len, unsigned long long int value)
{
	// ------ TODO ------
}

void SNMPParser::onResponseValueTimeticks(const char *host, unsigned int host_ip, const int *oid, size_t len, short value)
{
	// ------ TODO ------
}

void SNMPParser::onResponseValueOID(const char *host, unsigned int host_ip, const int *oid, size_t len, int* value, size_t vlen)
{
	// ------ TODO ------
}

void SNMPParser::onResponseValueOctetString(const char *host, unsigned int host_ip, const int *oid, size_t len, char* value)
{
	// ------ TODO ------
}

/**
* GET-response oid binding
*/
bool SNMPParser::parseResponseValue(const char *host, unsigned int host_ip, const unsigned char *&data, const unsigned char *limit)
{
	//printf("        Response value\n");
	
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x30 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	int oid[ASN1_OID_MAXLEN];
	size_t len;
	if ( ! readOID(oid, len, p, end) ) return false;
	/*printf("          OID: %d.%d", oid[0], oid[1]);
	for(size_t i = 2; i < len ; i++)
	{
		printf(".%d", oid[i]);
	}
	printf("\n");*/
	
	const unsigned char *p1 = p;
	const unsigned char *end1 = end;
	int value;
	if ( readInt(value, p1, end1) )
	{
		onResponseValueInt(host, host_ip, oid, len, value);
		
		data = end1;
		
		return true;
	}
	
	p1 = p;
	end1 = end;
	unsigned int uValue;
	if ( readCounterGauge32(uValue, p1, end1) )
	{
		onResponseValueCounterGauge32(host, host_ip, oid, len, uValue);
		
		data = end1;
		
		return true;
	}
	
	p1 = p;
	end1 = end;
	unsigned long long int ulValue;
	if ( readCounter64(ulValue, p1, end1) )
	{
		onResponseValueCounter64(host, host_ip, oid, len, ulValue);
		
		data = end1;
		
		return true;
	}
	
	p1 = p;
	end1 = end;
	char strValue[256];
	if ( readString(strValue, p1, end1) )
	{
		onResponseValueOctetString(host, host_ip, oid, len, strValue);
		
		data = end1;
		
		return true;
	}
	
	return false;
}

/**
* GET-response oid bindings
*/
bool SNMPParser::parseResponseBindings(const char *host, unsigned int host_ip, const unsigned char *&data, const unsigned char *limit)
{
	//printf("      Response bindings, 0x%02X\n", data[0]);
	
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x30 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	while ( p < end )
	{
		if ( ! parseResponseValue(host, host_ip, p, end) ) return false;
	}
	
	data = end;
	
	return true;
}

/**
* GET-response
*/
bool SNMPParser::parseResponse(const char *host, unsigned int host_ip, const unsigned char *data, const unsigned char *limit)
{
	//printf("  parse SNMP GET-response\n");
	
	if ( ! readInt(requestID, data, limit) ) return false;
	//printf("    requestID: %d\n", requestID);
	
	if ( ! readInt(errorStatus, data, limit) ) return false;
	//printf("    errorStatus: %d\n", errorStatus);
	
	if ( ! readInt(errorIndex, data, limit) ) return false;
	//printf("    errorIndex: %d\n", errorIndex);
	
	if ( ! parseResponseBindings(host, host_ip, data, limit) ) return false;
	
	return true;
}

/**
 * TRAP oid binding
 */
bool SNMPParser::parseTrapValue(const char *host, unsigned int host_ip, const unsigned char *&data, const unsigned char *limit)
{
	//printf("        Trap value\n");
	
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x30 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	int oid[ASN1_OID_MAXLEN];
	size_t len;
	if ( ! readOID(oid, len, p, end) ) return false;
	/*printf("          OID: %d.%d", oid[0], oid[1]);
	for(size_t i = 2; i < len ; i++)
	{
		printf(".%d", oid[i]);
	}
	printf("\n");*/
	
	const unsigned char *p1 = p;
	const unsigned char *end1 = end;
	int value;
	if ( readInt(value, p1, end1) )
	{
		onResponseValueInt(host, host_ip, oid, len, value);
		
		data = end1;
		
		return true;
	}
	
	p1 = p;
	end1 = end;
	unsigned int uValue;
	if ( readCounterGauge32(uValue, p1, end1) )
	{
		onResponseValueCounterGauge32(host, host_ip, oid, len, uValue);
		
		data = end1;
		
		return true;
	}

	p1 = p;
	end1 = end;
	unsigned long long int ulValue;
	if ( readCounter64(ulValue, p1, end1) )
	{
		onResponseValueCounter64(host, host_ip, oid, len, ulValue);
		
		data = end1;
		
		return true;
	}
	
	p1 = p;
	end1 = end;
	short sValue;
	if ( readTimeticks(sValue, p1, end1) )
	{
		onResponseValueTimeticks(host, host_ip, oid, len, sValue);
		
		data = end1;
		
		return true;
	}
	
	p1 = p;
	end1 = end;
	int oidValue[ASN1_OID_MAXLEN];
	size_t vlen;
	if ( readOID(oidValue, vlen, p1, end1) )
	{
		onResponseValueOID(host, host_ip, oid, len, oidValue, vlen);
		
		data = end1;
		
		return true;
	}
	
	p1 = p;
	end1 = end;
	char strValue[256];
	if ( readString(strValue, p1, end1) )
	{
		onResponseValueOctetString(host, host_ip, oid, len, strValue);
		
		data = end1;
		
		return true;
	}
	
	return false;
}
	
/**
 * TRAP oid bindings
 */
bool SNMPParser::parseTrapBindings(const char *host, unsigned int host_ip, const unsigned char *&data, const unsigned char *limit)
{
	//printf("      TRAP bindings, 0x%02X\n", data[0]);
	
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	if ( h.type != 0x30 ) return false;
	
	const unsigned char *p = h.data;
	const unsigned char *end = h.limit;
	
	while ( p < end )
	{
		if ( ! parseTrapValue(host, host_ip, p, end) ) return false;
	}
	
	data = end;
	
	return true;
}
	
/**
 * TRAP
 */
bool SNMPParser::parseTrap(const char *host, unsigned int host_ip, const unsigned char *data, const unsigned char *limit)
{
	//printf("  parse SNMP TRAP\n");
	
	if ( ! readInt(requestID, data, limit) ) return false;
	//printf("    requestID: %d\n", requestID);
	
	if ( ! readInt(errorStatus, data, limit) ) return false;
	//printf("    errorStatus: %d\n", errorStatus);
	
	if ( ! readInt(errorIndex, data, limit) ) return false;
	//printf("    errorIndex: %d\n", errorIndex);
	
	if ( ! parseTrapBindings(host, host_ip, data, limit) ) return false;
	
	return true;
}

/**
* Парсинг пакета
*/
bool SNMPParser::parse(const char *host, unsigned int host_ip, const unsigned char *data, const unsigned char *limit)
{
	//printf("parse SNMP packet\n");
	asn1_header_t h;
	if ( ! readHeader(h, data, limit) ) return false;
	//printf("SNMP packet, type: 0x%02X\n", h.type);
	
	if ( ! readInt(version, data, limit) ) return false;
	//printf("SNMP version: %d\n", version);
	if ( version != 1 ) return false;
	
	if ( ! readString(community, data, limit) ) return false;
	//printf("SNMP community: '%s'\n", community);
	
	if ( ! readHeader(h, data, limit) ) return false;
	//printf("SNMP sub-packet, type: 0x%02X\n", h.type);
	switch ( h.type )
	{
	case 0xA0:
		if ( ! parseGetRequest(data, limit) ) return false;
		break;
	case 0xA2:
		if ( ! parseResponse(host, host_ip, data, limit) ) return false;
		break;
	case 0xA7:
		if ( ! parseTrap(host, host_ip, data, limit) ) return false;
		break;
	default:
		printf("unknown SNMP command: 0x%02X\n", h.type);
		return false;
	}
	
	return true;
}

/**
* Парсинг пакета
*/
bool SNMPParser::parse(const char *host, unsigned int host_ip, const unsigned char *data, size_t size)
{
	bool status = parse(host, host_ip, data, data + size);
	//printf("%s", status ? "parse ok\n" : "parse fail\n");
	return status;
}
