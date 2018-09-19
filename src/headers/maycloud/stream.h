#ifndef NANO_STREAM_H
#define NANO_STREAM_H

#include <sys/types.h>

namespace nanosoft
{
	
	class stream
	{
	private:
		stream(const stream &) { }
		stream& operator = (const stream &) { return *this; }
	public:
		stream() { }
		virtual ~stream() { }
		virtual size_t read(void *buffer, size_t size) = 0;
		virtual size_t write(const void *buffer, size_t size) = 0;
	};
	
}

#endif // NANO_STREAM_H
