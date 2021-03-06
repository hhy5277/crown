/*
 * Copyright (c) 2012-2017 Daniele Bartolini and individual contributors.
 * License: https://github.com/dbartolini/crown/blob/master/LICENSE
 */

#include "core/containers/vector.h"
#include "core/filesystem/file.h"
#include "core/filesystem/filesystem_disk.h"
#include "core/filesystem/path.h"
#include "core/memory/temp_allocator.h"
#include "core/os.h"
#include "core/strings/dynamic_string.h"

#if CROWN_PLATFORM_POSIX
	#include <stdio.h>
	#include <errno.h>
#elif CROWN_PLATFORM_WINDOWS
	#include <tchar.h>
	#include <windows.h>
#endif

namespace crown
{
class FileDisk : public File
{
#if CROWN_PLATFORM_POSIX
	FILE* _file;
#elif CROWN_PLATFORM_WINDOWS
	HANDLE _file;
	bool _eof;
#endif

public:

	/// Opens the file located at @a path with the given @a mode.
	FileDisk()
#if CROWN_PLATFORM_POSIX
		: _file(NULL)
#elif CROWN_PLATFORM_WINDOWS
		: _file(INVALID_HANDLE_VALUE)
		, _eof(false)
#endif
	{
	}

	virtual ~FileDisk()
	{
		close();
	}

	void open(const char* path, FileOpenMode::Enum mode)
	{
#if CROWN_PLATFORM_POSIX
		_file = fopen(path, (mode == FileOpenMode::READ) ? "rb" : "wb");
		CE_ASSERT(_file != NULL, "fopen: errno = %d, path = '%s'", errno, path);
#elif CROWN_PLATFORM_WINDOWS
		_file = CreateFile(path
			, (mode == FileOpenMode::READ) ? GENERIC_READ : GENERIC_WRITE
			, 0
			, NULL
			, OPEN_ALWAYS
			, FILE_ATTRIBUTE_NORMAL
			, NULL
			);
		CE_ASSERT(_file != INVALID_HANDLE_VALUE
			, "CreateFile: GetLastError = %d, path = '%s'"
			, GetLastError()
			, path
			);
#endif
	}

	void close()
	{
		if (is_open())
		{
#if CROWN_PLATFORM_POSIX
			fclose(_file);
			_file = NULL;
#elif CROWN_PLATFORM_WINDOWS
			CloseHandle(_file);
			_file = INVALID_HANDLE_VALUE;
#endif
		}
	}

	bool is_open() const
	{
#if CROWN_PLATFORM_POSIX
		return _file != NULL;
#elif CROWN_PLATFORM_WINDOWS
		return _file != INVALID_HANDLE_VALUE;
#endif
	}

	u32 size()
	{
#if CROWN_PLATFORM_POSIX
		long pos = ftell(_file);
		CE_ASSERT(pos != -1, "ftell: errno = %d", errno);
		int err = fseek(_file, 0, SEEK_END);
		CE_ASSERT(err == 0, "fseek: errno = %d", errno);
		long size = ftell(_file);
		CE_ASSERT(size != -1, "ftell: errno = %d", errno);
		err = fseek(_file, (long)pos, SEEK_SET);
		CE_ASSERT(err == 0, "fseek: errno = %d", errno);
		CE_UNUSED(err);
		return (u32)size;
#elif CROWN_PLATFORM_WINDOWS
		return GetFileSize(_file, NULL);
#endif
	}

	u32 position()
	{
#if CROWN_PLATFORM_POSIX
		long pos = ftell(_file);
		CE_ASSERT(pos != -1, "ftell: errno = %d", errno);
		return (u32)pos;
#elif CROWN_PLATFORM_WINDOWS
		DWORD pos = SetFilePointer(_file, 0, NULL, FILE_CURRENT);
		CE_ASSERT(pos != INVALID_SET_FILE_POINTER
			, "SetFilePointer: GetLastError = %d"
			, GetLastError()
			);
		return (u32)pos;
#endif
	}

	bool end_of_file()
	{
#if CROWN_PLATFORM_POSIX
		return feof(_file) != 0;
#elif CROWN_PLATFORM_WINDOWS
		return _eof;
#endif
	}

	void seek(u32 position)
	{
#if CROWN_PLATFORM_POSIX
		int err = fseek(_file, (long)position, SEEK_SET);
		CE_ASSERT(err == 0, "fseek: errno = %d", errno);
#elif CROWN_PLATFORM_WINDOWS
		DWORD err = SetFilePointer(_file, position, NULL, FILE_BEGIN);
		CE_ASSERT(err != INVALID_SET_FILE_POINTER
			, "SetFilePointer: GetLastError = %d"
			, GetLastError()
			);
#endif
		CE_UNUSED(err);
	}

	void seek_to_end()
	{
#if CROWN_PLATFORM_POSIX
		int err = fseek(_file, 0, SEEK_END);
		CE_ASSERT(err == 0, "fseek: errno = %d", errno);
#elif CROWN_PLATFORM_WINDOWS
		DWORD err = SetFilePointer(_file, 0, NULL, FILE_END);
		CE_ASSERT(err != INVALID_SET_FILE_POINTER
			, "SetFilePointer: GetLastError = %d"
			, GetLastError()
			);
#endif
		CE_UNUSED(err);
	}

	void skip(u32 bytes)
	{
#if CROWN_PLATFORM_POSIX
		int err = fseek(_file, bytes, SEEK_CUR);
		CE_ASSERT(err == 0, "fseek: errno = %d", errno);
#elif CROWN_PLATFORM_WINDOWS
		DWORD err = SetFilePointer(_file, bytes, NULL, FILE_CURRENT);
		CE_ASSERT(err != INVALID_SET_FILE_POINTER
			, "SetFilePointer: GetLastError = %d"
			, GetLastError()
			);
#endif
		CE_UNUSED(err);
	}

	u32 read(void* data, u32 size)
	{
		CE_ASSERT(data != NULL, "Data must be != NULL");
#if CROWN_PLATFORM_POSIX
		size_t bytes_read = fread(data, 1, size, _file);
		CE_ASSERT(ferror(_file) == 0, "fread error");
		return (u32)bytes_read;
#elif CROWN_PLATFORM_WINDOWS
		DWORD bytes_read;
		BOOL err = ReadFile(_file, data, size, &bytes_read, NULL);
		CE_ASSERT(err == TRUE, "ReadFile: GetLastError = %d", GetLastError());
		_eof = err && bytes_read == 0;
		return bytes_read;
#endif
	}

	u32 write(const void* data, u32 size)
	{
		CE_ASSERT(data != NULL, "Data must be != NULL");
#if CROWN_PLATFORM_POSIX
		size_t bytes_written = fwrite(data, 1, size, _file);
		CE_ASSERT(ferror(_file) == 0, "fwrite error");
		return (u32)bytes_written;
#elif CROWN_PLATFORM_WINDOWS
		DWORD bytes_written;
		WriteFile(_file, data, size, &bytes_written, NULL);
		CE_ASSERT(size == bytes_written
			, "WriteFile: GetLastError = %d"
			, GetLastError()
			);
		return bytes_written;
#endif
	}

	void flush()
	{
#if CROWN_PLATFORM_POSIX
		int err = fflush(_file);
		CE_ASSERT(err == 0, "fflush: errno = %d", errno);
#elif CROWN_PLATFORM_WINDOWS
		BOOL err = FlushFileBuffers(_file);
		CE_ASSERT(err != 0
			, "FlushFileBuffers: GetLastError = %d"
			, GetLastError()
			);
#endif
		CE_UNUSED(err);
	}
};

FilesystemDisk::FilesystemDisk(Allocator& a)
	: _allocator(&a)
	, _prefix(a)
{
}

void FilesystemDisk::set_prefix(const char* prefix)
{
	_prefix.set(prefix, strlen32(prefix));
}

File* FilesystemDisk::open(const char* path, FileOpenMode::Enum mode)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	FileDisk* file = CE_NEW(*_allocator, FileDisk)();
	file->open(abs_path.c_str(), mode);
	return file;
}

void FilesystemDisk::close(File& file)
{
	CE_DELETE(*_allocator, &file);
}

bool FilesystemDisk::exists(const char* path)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	return os::exists(abs_path.c_str());
}

bool FilesystemDisk::is_directory(const char* path)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	return os::is_directory(abs_path.c_str());
}

bool FilesystemDisk::is_file(const char* path)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	return os::is_file(abs_path.c_str());
}

u64 FilesystemDisk::last_modified_time(const char* path)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	return os::mtime(abs_path.c_str());
}

void FilesystemDisk::create_directory(const char* path)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	if (!os::exists(abs_path.c_str()))
		os::create_directory(abs_path.c_str());
}

void FilesystemDisk::delete_directory(const char* path)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	os::delete_directory(abs_path.c_str());
}

void FilesystemDisk::create_file(const char* path)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	os::create_file(abs_path.c_str());
}

void FilesystemDisk::delete_file(const char* path)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	os::delete_file(abs_path.c_str());
}

void FilesystemDisk::list_files(const char* path, Vector<DynamicString>& files)
{
	CE_ENSURE(NULL != path);

	TempAllocator256 ta;
	DynamicString abs_path(ta);
	get_absolute_path(path, abs_path);

	os::list_files(abs_path.c_str(), files);
}

void FilesystemDisk::get_absolute_path(const char* path, DynamicString& os_path)
{
	if (path::is_absolute(path))
	{
		os_path = path;
		return;
	}

	path::join(os_path, _prefix.c_str(), path);
}

} // namespace crown
