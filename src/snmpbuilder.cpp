#include <maycloud/snmpbuilder.h>

SNMPBuilder::SNMPBuilder(size_t acapacity)
{
	capacity = acapacity;
	buffer = new unsigned char[capacity];
}

SNMPBuilder::~SNMPBuilder()
{
	delete [] buffer;
}

bool SNMPBuilder::reset(const char *community)
{
	offset = buffer;
	// header
	offset[0] = 0x30;
	offset[1] = 0x82;
	// buffer[2], buffer[3] = packet size
	
	// version
	offset[4] = 0x02;
	offset[5] = 0x01;
	offset[6] = 0x01;
	
	offset[7] = 0x04;
	size_t len = strlen(community);
	if ( len >= 128 ) return false;
	offset[8] = len;
	offset = &offset[9];
	memcpy(offset, community, len);
	offset += len;
	
	cmd_offset = offset;
	// cmd_offset[0] = command type
	offset[1] = 0x82;
	// cmd_offset[2], cmd_offset[3] = cmd size
	
	offset[4] = 0x02;
	offset[5] = 4;
	requestID_offset = &offset[6];
	offset = requestID_offset + 4;
	
	// error status
	offset[0] = 0x02;
	offset[1] = 0x01;
	offset[2] = 0x00;
	
	// error index
	offset[3] = 0x02;
	offset[4] = 0x01;
	offset[5] = 0x00;
	
	// OID bindings
	offset = oid_offset = offset + 6;
	offset[0] = 0x30;
	offset[1] = 0x82;
	// offset[2], offset[3] = OID bindings size
	offset += 4;
	// offset = OID bindings;

	return true;
}

/**
* Установить тип пакета
*/
void SNMPBuilder::setCommand(unsigned char type)
{
	cmd_offset[0] = type;
}

/**
* Установить requestID
*/
void SNMPBuilder::setRequestID(unsigned int value)
{
	unsigned char *p = requestID_offset;
	p[0] = (value >> 24) & 0xFF;
	p[1] = (value >> 16) & 0xFF;
	p[2] = (value >> 8) & 0xFF;
	p[3] = value & 0xFF;
}

static void saveOIDItem(unsigned char *&offset, unsigned int value)
{
	if ( value == 0 )
	{
		*offset++ = 0;
		return;
	}
	
	int len = 0;
	unsigned char buf[8];
	
	while ( value )
	{
		buf[len] = value % 128;
		value = value / 128;
		len ++;
	}
	
	for(int i = len - 1; i > 0; i--)
	{
		*offset++ = buf[i] | 0x80;
	}
	*offset++ = buf[0];
}

static void saveOID(unsigned char *&offset, const int *oid, size_t len)
{
	unsigned char *p = offset + 2;
	saveOIDItem(p, oid[0] * 40 + oid[1]);
	for(size_t i = 2; i < len; i++) saveOIDItem(p, oid[i]);
	offset[0] = 0x06;
	offset[1] = p - offset - 2;
	offset = p;
}

/**
* Добавить OID со значением NULL
*/
bool SNMPBuilder::addNull(const int *oid, size_t len)
{
	offset[0] = 0x30;
	unsigned char *p = offset + 2;
	saveOID(p, oid, len);
	*p++ = 0x05;
	*p++ = 0x00;
	offset[1] = p - offset - 2;
	offset = p;
	
	return true;
}

static void saveLen(unsigned char *offset, size_t len)
{
	offset[0] = (len >> 8) & 0xFF;
	offset[1] = len & 0xFF;
}

/**
* Завершить запись пакета
*/
void SNMPBuilder::commit()
{
	// packet size
	saveLen(buffer+2, offset - buffer - 4);
	
	// cmd size
	saveLen(cmd_offset + 2, offset - cmd_offset - 4);
	
	// oid bindings size
	saveLen(oid_offset + 2, offset - oid_offset - 4);
}

