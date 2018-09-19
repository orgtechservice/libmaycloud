#include <maycloud/fstream.h>

namespace nanosoft
{
	
	fstream::fstream(int afd): fd(afd)
	{
	}
	
	fstream::fstream(const char *fileName, int flags)
	{
		fd = ::open(fileName, flags);
		if ( fd < 0 ) stderror();
	}
	
	fstream::fstream(const char *fileName, int flags, mode_t mode)
	{
		fd = ::open(fileName, flags, mode);
		if ( fd < 0 ) stderror();
	}
	
	fstream::~fstream()
	{
	}
	
	bool fstream::open(const char *fileName, int flags)
	{
		fd = ::open(fileName, flags);
		return fd >= 0 ? 1 : 0;
	}
	
	bool fstream::open(const char *fileName, int flags, mode_t mode)
	{
		fd = ::open(fileName, flags, mode);
		return fd >= 0 ? 1 : 0;
	}
	
	void fstream::bind(int afd)
	{
		fd = afd;
	}
	
	void fstream::close()
	{
		::close(fd);
		fd = 0;
	}
	
	size_t fstream::read(void *buffer, size_t size)
	{
		return ::read(fd, buffer, size);
	}
	
	size_t fstream::write(const void *buffer, size_t size)
	{
		return ::write(fd, buffer, size);
	}
	
}
