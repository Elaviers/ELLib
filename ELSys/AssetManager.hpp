#pragma once
#include "IO.hpp"
#include <ELCore/ByteReader.hpp>
#include <ELCore/Context.hpp>
#include <ELCore/Hashmap.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELCore/String.hpp>

class AssetManagerBase
{
protected:
	Buffer<String> _paths;

	const Buffer<String> _extensions;

	bool _fallbackAnywhere; //Allows the asset manager fallback even if _all.txt is not present for a subdir for an asset
protected:
	template <typename T>
	friend class AssetManager;

	AssetManagerBase(const Buffer<String>& extensions) : _extensions(extensions), _fallbackAnywhere(false) { _paths.Add(""); /*Root dir*/ }
public:
	virtual ~AssetManagerBase() {}

	virtual void GetAllPossibleKeys(Buffer<String>& resultsOut, const Context& ctx, const String& path = "", bool recursive = false) const = 0;
};

#include <ELCore/Asset.hpp>

template<typename T>
class AssetManager : protected AssetManagerBase
{
private:
	Hashmap<String, SharedPointerData<T>> _map;
	
protected:
	virtual T* _CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx) = 0;
	virtual void _ResourceRead(T& resource, const Buffer<byte>& data, const String& extension, const Context& ctx)
	{
		Asset* asset = dynamic_cast<Asset*>(&resource);
		if (asset)
		{
			if (extension == ".txt")
			{
				asset->ReadText(String(data), ctx);
			}
			else
			{
				ByteReader reader = data;
				asset->Read(reader);
			}
		}
	}

	//Called when the specified name could not be loaded
	virtual bool _CreateAlternative(T*& resource, const String& name, const Context& ctx) { return false; }

	//Called before deleting a resource
	virtual void _DestroyResource(T& resource) {}

	//Returns a list of managers who's assets can be used to finalise an asset for this manager
	virtual Buffer<const AssetManagerBase*> _GetFallbackManagers(const Context&) const { return {}; }
	
	//Extensions are in priority order (names that specify the extension override this)
	AssetManager(const Buffer<String>& extensions = { ".txt" }) : AssetManagerBase(extensions) { }
	AssetManager(const AssetManager&) = delete;

	virtual ~AssetManager() { _DestroyAll(); }

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

	bool NameIsValid(const String& name) const
	{
		if (name.GetLength() > 0 && name[name.GetLength() - 1] != '/')
		{
			int slash = name.LastIndexOf('/');
			return name[slash >= 0 ? slash + 1 : 0] != '_';
		}

		return false;
	}

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

	struct ResourcePathInfo
	{
		const String* path = nullptr; //Path chosen for the file (w/ trailing slash)
		String extension; //(e.g. ".txt")
		String fullpath; //Path + name + extension
	};

	//info is an output variable and is only valid if function returns true
	bool GetPath(const String& name, ResourcePathInfo &info) const
	{
		info.extension = Utilities::GetExtension(name);
		if (info.extension.GetLength())
		{
			bool validExtension = false;
			for (const String& ext : _extensions)
				if (info.extension == ext)
				{
					validExtension = true;
					break;
				}

			if (validExtension)
			{
				for (const String& path : _paths)
				{
					info.fullpath = path + name;
					if (IO::FileExists(info.fullpath.GetData()))
					{
						info.path = &path;
						return true;
					}
				}
			}
		}
		else
		{
			for (const String& path : _paths)
			{
				for (const String& ext : _extensions)
				{
					info.fullpath = path + name + ext;
					if (IO::FileExists(info.fullpath.GetData()))
					{
						info.extension = ext;
						info.path = &path;
						return true;
					}
				}
			}
		}

		return false;
	}
	
	SharedPointer<T> Get(const String& name, const Context& ctx)
	{
		if (name.GetLength() == 0)
			return SharedPointer<T>();

		String lowerName = name.ToLower();
		SharedPointerData<T>* existing = _map.Get(lowerName);
		if (existing && existing->GetPtr()) 
			return SharedPointer<T>(*existing);

		if (!NameIsValid(name))
			return SharedPointer<T>();

		T* resource = nullptr;
		ResourcePathInfo info;
		if (GetPath(name, info))
		{
			//Run any _all.txt files in preceding dirs & paths
			for (const String& path : _paths)
			{
				int start = 0;
				String searchPath = path;
				do
				{
					String preFile = searchPath + "_all.txt";
					if (IO::FileExists(preFile.GetData()))
					{
						if (resource) _ResourceRead(*resource, IO::ReadFile(preFile.GetData()), ".txt", ctx);
						else resource = _CreateResource(IO::ReadFile(preFile.GetData()), lowerName, ".txt", ctx);
					}

					int stop = name.IndexOf('/', start);
					if (stop == -1) stop = (int)name.GetLength(); //todo: casting size_t to int.. probably always gonna be fine though

					searchPath += name.SubString(start, stop) + '/';
					start = stop + 1;
				} while (start < name.GetLength());

				if (&path == info.path)
					break;
			}
			
			//..Then the resource
			if (resource) _ResourceRead(*resource, IO::ReadFile(info.fullpath.GetData()), info.extension, ctx);
			else resource = _CreateResource(IO::ReadFile(info.fullpath.GetData()), lowerName, info.extension, ctx);

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
		else
		{
			//Okay, so there isn't a file for this specific resource...
			//Let's just run every applicable _all.txt

			for (const String& path : _paths)
			{
				int start = 0;
				String searchPath = path;
				do
				{
					String preFile = searchPath + "_all.txt";
					if (IO::FileExists(preFile.GetData()))
					{
						if (resource) _ResourceRead(*resource, IO::ReadFile(preFile.GetData()), ".txt", ctx);
						else resource = _CreateResource(IO::ReadFile(preFile.GetData()), lowerName, ".txt", ctx);
					}

					int stop = name.IndexOf('/', start);
					if (stop == -1) stop = (int)name.GetLength(); //todo: casting size_t to int.. probably always gonna be fine though

					searchPath += name.SubString(start, stop) + '/';
					start = stop + 1;
				} while (IO::DirectoryExists(searchPath.GetData()) && start < name.GetLength());
			}

			if (_CreateAlternative(resource, name, ctx))
			{
				if (existing)
				{
					existing->SetPtr(resource);
					return SharedPointer<T>(*existing);
				}

				return SharedPointer<T>(_map.Set(lowerName, SharedPointerData<T>(resource, 0)));
			}
			else if (resource)
				delete resource;
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

	Buffer<String> GetAllPossibleKeys(const Context& ctx, const String& path = "", bool recursive = true) 
	{ 
		Buffer<String> b; 
		GetAllPossibleKeys(b, ctx, path, recursive); 
		return b;
	}

	virtual void GetAllPossibleKeys(Buffer<String>& resultsOut, const Context& ctx, const String& path = "", bool recursive = true) const override
	{
		for (const String* key : _map.ToKBuffer())
		{
			if (path.GetLength() > 0)
			{
				int slash = key->IndexOf('/');
				if (slash < 0 || key->SubString(0, (size_t)slash + 1) != path)
					continue;
			}

			resultsOut.OrderedAdd(*key);
		}

		Buffer<const AssetManagerBase*> fallbacks = _GetFallbackManagers(ctx);
		const Buffer<const AssetManagerBase*>* pfallbacks = &fallbacks;

		if (_fallbackAnywhere)
		{
			for (const AssetManagerBase* f : fallbacks)
				f->GetAllPossibleKeys(resultsOut, ctx, path, false);

			pfallbacks = nullptr;
		}
		else
		{
			for (const String& p : _paths)
			{
				if (path.GetLength())
				{
					String check = p;

					for (int i = 0;;)
					{
						if (IO::FileExists(CSTR(check, "_all.txt")))
						{
							for (const AssetManagerBase* f : fallbacks)
								f->GetAllPossibleKeys(resultsOut, ctx, path, false);

							pfallbacks = nullptr;
							break;
						}

						int s = i;
						i = path.IndexOf('/', s);
						if (i < 0 || i >= path.GetLength() - 1)
							break;

						check += path.SubString(s, i) + '/';
					}
				}

				_FindAllKeysForPath(p, path, resultsOut, pfallbacks, ctx, recursive);
			}
		}
	}


private:
	void _FindAllKeysForPath(
		const String& path, const String& dir, 
		Buffer<String>& results, 
		const Buffer<const AssetManagerBase*>* fallbacks,
		const Context& ctx, 
		bool recursive) const
	{
		String d = path + dir;

		if (!IO::DirectoryExists(d.GetData()))
			return;

		if (fallbacks)
		{
			if (IO::FileExists(CSTR(d, "_all.txt")))
			{
				for (const AssetManagerBase* f : *fallbacks)
					f->GetAllPossibleKeys(results, ctx, dir, true);

				fallbacks = nullptr;
			}
		}

		for (const String& filename : IO::FindFilesInDirectory(CSTR(d, "*.*")))
		{
			String f = dir + filename;
			String full = path + f;

			if (IO::IsDirectory(full.GetData()))
			{
				if (recursive)
					_FindAllKeysForPath(path, f + '/', results, fallbacks, ctx, true);
			}
			else
			{
				int extIndex = f.IndexOf('.');
				if (extIndex >= 0)
				{
					String fpath = f.ToLower();
					String name = fpath.SubString(0, extIndex);
					String ext = fpath.SubString(extIndex, fpath.GetLength());

					for (const String& extension : _extensions)
						if (ext == extension)
						{
							if (NameIsValid(name) && results.IndexOfFirst(name) == -1)
								results.OrderedAdd(name);

							break;
						}
				}
			}
		}
	}
};
