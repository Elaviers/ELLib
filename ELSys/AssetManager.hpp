#pragma once
#include "IO.hpp"
#include <ELCore/ByteReader.hpp>
#include <ELCore/Context.hpp>
#include <ELCore/Hashmap.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELCore/String.hpp>

class Asset;

template<typename T>
class AssetManager
{
private:
	Buffer<String> _paths;
	Hashmap<String, SharedPointerData<T>> _map;

	const Buffer<String> _extensions;

protected:
	virtual T* _CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context&) = 0;
	virtual void _ResourceRead(T& resource, const Buffer<byte>& data, const String& extension, const Context&)
	{
		ByteReader reader = data;
		((Asset&)resource).Read(reader);
	}

	//Called before deleting a resource
	virtual void _DestroyResource(T& resource) {}

	//Extensions are in priority order (names that specify the extension override this)
	AssetManager(const Buffer<String>& extensions = { ".txt" }) :
		_extensions(extensions)
	{ 
		_paths.Add(""); //Root dir
	}

	AssetManager(const AssetManager&) = delete;

	virtual ~AssetManager() 
	{
		_DestroyAll();
	}

	void _DestroyAll()
	{
		Buffer<Pair<const String, SharedPointerData<T>>*> buffer = _map.ToKVBuffer();

		for (size_t i = 0; i < buffer.GetSize(); ++i)
		{
			_DestroyResource(*buffer[i]->second.GetPtr());
			delete buffer[i]->second.GetPtr();
			buffer[i]->second.SetPtr(nullptr);
		}
	}

	SharedPointerData<T>& _MapValue(const String& name)
	{
		SharedPointerData<T>* found = _map.Get(name);

		if (found)
			return *found;

		return _map.Set(name, SharedPointerData<T>(nullptr, 0)); 
	}

public:
	virtual void Initialise() {}

	void Refresh(const Context& ctx)
	{
		_DestroyAll();
		Initialise();

		//Reload all assets that are still in use
		Buffer<Pair<const String, SharedPointerData<T>>*> buffer = _map.ToKVBuffer();
		for (size_t i = 0; i < buffer.GetSize(); ++i)
		{
			if (buffer[i]->second.GetReferenceCount())
				auto loadNewAsset = Get(buffer[i]->first, ctx);
		}
	}

	const Hashmap<String, SharedPointerData<T>>& GetMap() const { return _map; }

	//Only adds if key does not exist, otherwise returns false
	SharedPointer<T> Add(const String& name, T* value) 
	{
		if (_map.Get(name))
			return SharedPointer<T>();

		return _map.Set(name, SharedPointerData<T>(value, 0));
	}

	String GetFilename(const String& name) const
	{
		String extension = Utilities::GetExtension(name);

		if (extension.GetLength())
		{
			bool validExtension = false;
			for (const String& ext : _extensions)
				if (extension == ext)
				{
					validExtension = true;
					break;
				}

			if (!validExtension)
				return "";

			for (const String& path : _paths)
			{
				String filepath = path + '/' + name;
				if (IO::FileExists(filepath.GetData()))
					return filepath;
			}

			return "";
		}

		for (const String& path : _paths)
		{
			for (const String& ext : _extensions)
			{
				String filepath = path + '/' + name + ext;
				if (IO::FileExists(filepath.GetData()))
					return filepath;
			}
		}

		return "";
	}
	
	SharedPointer<T> Get(const String& name, const Context& ctx)
	{
		if (name.GetLength() == 0)
			return SharedPointer<T>();

		String lowerName = name.ToLower();
		SharedPointerData<T>* existing = _map.Get(lowerName);
		if (existing && existing->GetPtr()) 
			return SharedPointer<T>(*existing);

		String filename = GetFilename(name);
		if (filename.GetLength())
		{
			String extension = Utilities::GetExtension(filename);
			T* resource;
			
			String preFile = filename.SubString(0, filename.LastIndexOf('/') + 1) + "_all.txt";
			if (IO::FileExists(preFile.GetData()))
			{
				resource = _CreateResource(IO::ReadFile(preFile.GetData()), lowerName, ".txt", ctx);
				_ResourceRead(*resource, IO::ReadFile(filename.GetData()), extension, ctx);
			}
			else
				resource = _CreateResource(IO::ReadFile(filename.GetData()), lowerName, extension, ctx);

			if (resource)
			{
				if (existing)
				{
					existing->SetPtr(resource);
					return SharedPointer<T>(*existing);
				}

				return SharedPointer<T>(_map.Set(lowerName, SharedPointerData<T>(resource, 0)));
			}
		}

		return SharedPointer<T>();
	}

	SharedPointer<T> Find(const String& name) const
	{
		SharedPointerData<T>* found = _map.Get(name.ToLower());

		return found ? SharedPointer<T>(*found) : SharedPointer<T>();
	}

	SharedPointer<T> Find(const T* ptr)
	{
		Buffer<Pair<const String, SharedPointerData<T>>*> buffer = _map.ToKVBuffer();

		for (Pair<const String, SharedPointerData<T>>* kv : buffer)
		{
			if (kv->second.GetPtr() == ptr)
				return SharedPointer<T>(kv->second);
		}

		return SharedPointer<T>();
	}

	String FindNameOf(const T* resource) const
	{
		Buffer<Pair<const String, const SharedPointerData<T>>*> buffer = _map.ToKVBuffer();

		for (size_t i = 0; i < buffer.GetSize(); ++i)
			if (buffer[i]->second.GetPtr() == resource)
				return buffer[i]->first;

		return "None";
	}

	void SetRootPath(const String &root) { _paths[0] = root; }
	const String &GetRootPath() { return _paths[0]; }

	void AddPath(const String &root) { _paths.Add(root); }

	const Buffer<String>& GetPaths() { return _paths; }

	Buffer<String> GetAllPossibleKeys()
	{
		Buffer<String> results;

		Buffer<const String*> keys = _map.ToKBuffer();

		for (size_t i = 0; i < keys.GetSize(); ++i)
			results.OrderedAdd(*keys[i]);

		for (const String& path : _paths)
		{
			Buffer<String> filenames = IO::FindFilesInDirectoryRecursive(path.GetData(), "*.*");

			for (const String& filename : filenames)
			{
				int extIndex = filename.IndexOf('.');
				if (extIndex >= 0)
				{
					String filenamelower = filename.ToLower();
					String name = filenamelower.SubString(0, extIndex);
					String ext = filenamelower.SubString(extIndex, filenamelower.GetLength());

					for (const String& extension : _extensions)
						if (ext == extension)
						{
							if (name[0] != '_' && results.IndexOf(name) == -1)
								results.OrderedAdd(name);

							break;
						}
				}
			}
		}

		return results;
	}
};