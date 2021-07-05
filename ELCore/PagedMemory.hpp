#pragma once
#include "Allocator.hpp"
#include "Buffer.hpp"
#include "Types.hpp"
#include "List.hpp"
#include "Utilities.hpp"

template <typename AllocatorType = DefaultAllocator<byte>>
class PagedMemory
{
private:
	using _PageArrayType = Buffer<byte, AllocatorType>;

public:
	using size_type = _PageArrayType::size_type;

private:
	struct _Page
	{
		_PageArrayType bytes;
		size_type allocated = 0;
		size_type freed = 0;
	};

	using _PageAllocator = Utilities::ReplaceParam<AllocatorType, _Page>::Type;
	using _PageListType = List<_Page, _PageAllocator>;

	_PageListType _pages;
	_PageListType::Iterator _firstUsablePage;
	size_type _newPageSize;

public:
	constexpr const static size_type DefaultPageSize = 512;

	constexpr PagedMemory(size_type newPageSize = DefaultPageSize) noexcept : _newPageSize(newPageSize) {}

	constexpr PagedMemory(const PagedMemory& other) : _pages(other._pages), _newPageSize(other._newPageSize)
	{
		for (auto it = _pages.begin(); it; ++it)
		{
			if (it->elements.GetSize() < it->bytes.allocated)
			{
				_firstUsablePage = it;
				break;
			}
		}
	}

	constexpr PagedMemory(PagedMemory&& other) noexcept : _pages(std::move(other._pages)), _firstUsablePage(other._firstUsablePage), _newPageSize(other._newPageSize)
	{
		other._firstUsablePage = _PageListType::Iterator();
		other._newPageSize = DefaultPageSize;
	}

	constexpr ~PagedMemory() {}

	constexpr void Clear()
	{
		_pages.Clear();
		_firstUsablePage = _PageListType::Iterator();
	}

	constexpr byte* Allocate(size_t size)
	{
		if (_firstUsablePage)
		{
			auto page = _firstUsablePage;
			while (page && page->bytes.GetSize() - page->allocated < size)
				++page;

			if (page)
			{
				byte* addr = &page->bytes[page->allocated];
				page->allocated += size;

				if (page->allocated >= page->bytes.GetSize())
					++_firstUsablePage;

				return addr;
			}
		}

		//new page
		_Page& newPage = _pages.EmplaceBack();
		newPage.bytes.Grow(_newPageSize);
		newPage.allocated = size;
		return &newPage.bytes[0];
	}

	constexpr void Deallocate(byte* data, size_t size) noexcept
	{
		for (auto page = _pages.begin(); page; ++page)
		{
			if (data >= page->bytes.begin() && data < page->bytes.end())
			{
				page->freed += size;

				if (page->freed == page->bytes.GetSize())
					_pages.Remove(page);

				return;
			}
		}
	}

	template <typename T>
	class Allocator
	{
		PagedMemory* _provider;

		friend PagedMemory;
		constexpr Allocator(PagedMemory* provider) noexcept : _provider(provider) {}

	public:
		using value_type = T;

		template <typename O>
		constexpr Allocator(const Allocator<O>& other) noexcept : _provider(other._provider) {}

		template <typename O>
		constexpr Allocator& operator=(const Allocator<O>& other) noexcept
		{
			_provider = other._provider;
			return *this;
		}

		template <typename O>
		constexpr Allocator(Allocator<O>&& other) noexcept : _provider(other._provider)
		{
			other._provider = nullptr;
		}

		template <typename O>
		constexpr Allocator& operator=(Allocator<O>&& other) noexcept
		{
			_provider = other._provider;
			other._provider = nullptr;
			return *this;
		}

		constexpr bool operator==(const Allocator& other) const noexcept { return _provider == other._provider; }

		constexpr value_type* allocate(size_t size)
		{
			return (value_type*)_provider->Allocate(sizeof(value_type) * size);
		}

		constexpr void deallocate(value_type* data, size_t size) noexcept
		{
			_provider->Deallocate((byte*)data, size);
		}
	};

	template <typename T>
	constexpr Allocator<T> GetAllocator() noexcept { return Allocator<T>(this); }
};
