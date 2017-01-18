#ifndef OID_H
#define OID_H

#include <maycloud/tempstring.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

class OID
{
protected:
	int oid[256];
	size_t oidlen;
public:
	inline OID() { oidlen = 0; memset(oid, 0, sizeof(oid)); }
	OID(size_t len, ...);
	OID(size_t len, const int* id);
	OID(const OID& id);
	
	int operator [] (size_t i) const;
	
	OID operator = (OID id);
	OID operator + (int i);
	
	inline bool operator == (OID id) const { return memcmp(oid, id.oid, id.oidlen * sizeof(id.oid[0])) == 0; }
	inline bool operator != (OID id) const { return memcmp(oid, id.oid, id.oidlen * sizeof(id.oid[0])) != 0; }
	
	/**
	 * Внимание! Оператор убирает из oid i последних элементов
	 */
	OID operator << (size_t cnt);
	
	inline size_t getLen() { return oidlen; }
	
	/**
	 * Скопировать OID в массив
	 */
	void cpyOIDtoInt(int* id, size_t len);
};

#endif // OID_H
