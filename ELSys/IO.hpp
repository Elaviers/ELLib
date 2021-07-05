#pragma once
#include <ELCore/Buffer.hpp>
#include <ELCore/Pair.hpp>
#include <ELCore/String.hpp>

namespace IO {
	bool DirectoryExists(const char* path);
	bool FileExists(const char* filename);

	bool IsDirectory(const char* path);

	Buffer<byte> ReadFile(const char *filename, bool silent = false);
	bool WriteFile(const char *filename, const byte *data, size_t dataLength);
	inline bool WriteFile(const char* filename, const Array<byte>& data) { return WriteFile(filename, data.begin(), data.GetSize()); }

	inline String ReadFileString(const char* filename, bool silent = false)
	{
		Buffer<byte> filedata = ReadFile(filename, silent);
		filedata.Add('\0');
		return String(filedata.begin(), filedata.GetSize());
	}

	//searchPath must have a wildcard in it, like C:/Example/*.poo
	Buffer<String> FindFilesInDirectory(const char *searchPath);
	Buffer<String> FindFilesInDirectoryRecursive(const char *searchPath, const char *wildcard = "*.*");

	String OpenFileDialog(const wchar_t* dir, const Buffer<Pair<const wchar_t*>>& ext);
	String SaveFileDialog(const wchar_t* dir, const Buffer<Pair<const wchar_t*>>& ext);
}
