#include <maycloud/oid.h>

OID::OID(size_t len, ...)
{
	memset(oid, 0, sizeof(oid));
	oidlen = len;
	
	va_list ap;
	va_start(ap, len);
	size_t i = 0;
	while( len-- > 0 )
	{
		oid[i] = va_arg(ap, int);
		i++;
	}
	va_end(ap);
}

OID::OID(size_t len, const int* id)
{
	memset(oid, 0, sizeof(oid));
	oidlen = len;
	memcpy(oid, id, len * sizeof(oid[0]));
}

OID::OID(const OID& id)
{
	memset(oid, 0, sizeof(oid));
	oidlen = id.oidlen;
	memcpy(oid, id.oid, sizeof(oid));
}

int OID::operator [] (size_t i) const
{
	if(i < oidlen) return oid[i];
	else return 0;
}

OID OID::operator = (OID id)
{
	memset(oid, 0, sizeof(oid));
	oidlen = id.oidlen;
	memcpy(oid, id.oid, sizeof(oid));
	return *this;
}

OID OID::operator + (int i)
{
	if(oidlen < 256)
	{
		oid[oidlen] = i;
		oidlen++;
	}
	return *this;
}

/**
 * Внимание! Оператор "убирает" из oid i последних элементов
 */
OID OID::operator << (size_t cnt)
{
	for(size_t i = 0; i < cnt; i++)
	{
		if(oidlen) oid[--oidlen] = 0;
	}
	
	return *this;
}

/**
 * Скопировать OID в массив
 */
void OID::cpyOIDtoInt(int* id, size_t len)
{
	if(len > oidlen)
	{
		printf("OID::cpyOID() Incorrect len!\n");
		return;
	}
	for(size_t i = 0; i < len; i++) id[i] = oid[i];
}
