#pragma once
#include "Allocator.hpp"
#include "Utilities.hpp"
#include <initializer_list>
#include <utility>

template <typename T, typename AllocatorType = DefaultAllocator<T>>
class List
{
public:
	using size_type = size_t;
	using value_type = T;

private:
	struct Node
	{
		T value;
		Node* next;
		Node* prev;

		template <typename... Args>
		Node(Args&&... ctorArgs) : value(std::forward<Args>(ctorArgs)...), next(nullptr), prev(nullptr) {}
	};

	using _NodeAlloc = Utilities::ReplaceParam<AllocatorType, Node>::Type;
	class Proxy : public _NodeAlloc
	{
	public:
		size_type size;
		Node* first;
		Node* last;

		constexpr Proxy() : size(0), first(nullptr), last(nullptr) {}
		constexpr Proxy(const AllocatorType& alloc) noexcept : _NodeAlloc(alloc), size(0), first(nullptr), last(nullptr) {}
		
		constexpr Proxy(const Proxy& other) : _NodeAlloc((const _NodeAlloc&)other), size(0), first(nullptr), last(nullptr) {}

		constexpr Proxy(Proxy&& other) noexcept : _NodeAlloc(std::move((_NodeAlloc&)other)), size(other.size), first(other.first), last(other.last) 
		{
			other.size = 0;
			other.first = nullptr;
			other.last = nullptr;
		}

		constexpr Proxy& operator=(const Proxy& other)
		{
			_NodeAlloc::operator=((const _NodeAlloc&)other);
			size = 0;
			first = last = nullptr;
			return *this;
		}

		constexpr Proxy& operator=(Proxy&& other) noexcept
		{
			_NodeAlloc::operator=(std::move((_NodeAlloc&)other));
			size = other.size;
			first = other.first;
			last = other.last;

			other.size = 0;
			other.first = other.last = nullptr;
			return *this;

		}
	} _proxy;

	size_type& _size = _proxy.size;
	Node*& _first = _proxy.first;
	Node*& _last = _proxy.last;

public:
	constexpr List() noexcept {}

	constexpr List(const AllocatorType& allocator) noexcept : _proxy(allocator) {}

	constexpr List(const std::initializer_list<T>& elements, const AllocatorType& allocator = AllocatorType()) : _proxy(allocator)
	{
		for (const T& elem : elements)
			EmplaceBack(elem);
	}

	constexpr List(const List& other) : _proxy(other._proxy)
	{
		for (const T& val : other)
			EmplaceBack(val);
	}

	constexpr List(List&& other) noexcept : _proxy(std::move(other._proxy))
	{
	}

	constexpr ~List()
	{
		Node* next;
		for (Node* node = _first; node;)
		{
			next = node->next;
			node->value.~T();
			_proxy.deallocate(node, 1);
			node = next;
		}
	}

	constexpr List& operator=(const List& other)
	{
		Clear();
		_proxy = other._proxy;

		for (const T& val : other)
			EmplaceBack(val);

		return *this;
	}

	constexpr List& operator=(List&& other) noexcept
	{
		Clear();
		_proxy = std::move(other._proxy);

		return *this;
	}

	constexpr size_type GetSize() const noexcept { return _size; }

	constexpr void Clear()
	{
		Node* next;
		for (Node* node = _first; node;)
		{
			next = node->next;
			node->value.~T();
			_proxy.deallocate(node, 1);
			node = next;
		}

		_size = 0;
		_first = _last = nullptr;
	}

	/*
		ITERATION
	*/

	class Iterator
	{
		Node* _node;

		friend List;
		constexpr Iterator(Node* node) : _node(node) {}

	public:
		constexpr Iterator() noexcept : _node(nullptr) {}

		constexpr Iterator& operator--() noexcept
		{
			_node = _node->prev;
			return *this;
		}

		constexpr Iterator& operator++() noexcept
		{
			_node = _node->next;
			return *this;
		}

		constexpr T& operator*() const { return _node->value; }
		constexpr T* operator->() const { return &_node->value; }

		constexpr operator bool() const noexcept { return _node; }
		constexpr bool operator==(const Iterator& other) const noexcept { return _node == other._node; }
		constexpr bool operator!=(const Iterator& other) const noexcept { return _node != other._node; }
	};

	class ConstIterator
	{
		const Node* _node;

		friend List;
		constexpr ConstIterator(Node* node) noexcept : _node(node) {}

	public:
		constexpr ConstIterator() noexcept : _node(nullptr) {}
		constexpr ConstIterator(const Iterator& it) noexcept : _node(it._node) {}

		constexpr ConstIterator& operator--() noexcept
		{
			_node = _node->prev;
			return *this;
		}

		constexpr ConstIterator& operator++() noexcept
		{
			_node = _node->next;
			return *this;
		}

		constexpr const T& operator*() const { return _node->value; }
		constexpr const T* operator->() const { return &_node->value; }

		constexpr operator bool() const noexcept { return _node; }
		constexpr bool operator==(const ConstIterator& other) const noexcept { return _node == other._node; }
		constexpr bool operator!=(const ConstIterator& other) const noexcept { return _node != other._node; }
	};

	constexpr Iterator begin() noexcept { return _first; }
	constexpr Iterator end() noexcept { return nullptr; }
	constexpr Iterator Back() noexcept { return _last; }
	constexpr ConstIterator begin() const noexcept { return _first; }
	constexpr ConstIterator end() const noexcept { return nullptr; }
	constexpr ConstIterator Back() const noexcept { return _last; }

	/*
		INSERTION
	*/

	template <typename... Args>
	constexpr T& EmplaceFront(Args&&... ctorArgs)
	{
		Node* node = new (_proxy.allocate(1)) Node(std::forward<Args>(ctorArgs)...);

		if (_first)
		{
			node->next = _first;
			_first->prev = node;
			_first = node;
		}
		else
		{
			_first = _last = node;
		}

		++_size;
		return node->value;
	}

	constexpr T& AddFront(const T& element) { return EmplaceFront(element); }
	constexpr T& AddFront(T&& element) { return EmplaceFront(std::move(element)); }

	template <typename... Args>
	constexpr T& EmplaceBack(Args&&... ctorArgs)
	{
		Node* node = new (_proxy.allocate(1)) Node(std::forward<Args>(ctorArgs)...);

		if (_last)
		{
			node->prev = _last;
			_last->next = node;
			_last = node;
		}
		else
		{
			_first = _last = node;
		}

		++_size;
		return node->value;
	}

	constexpr T& AddBack(const T& element) { return EmplaceFront(element); }
	constexpr T& AddBack(T&& element) { return EmplaceFront(std::move(element)); }

	template <typename... Args>
	constexpr T& Emplace(const ConstIterator& before, Args&&... ctorArgs)
	{
		if (!before) return EmplaceBack(std::forward<Args>(ctorArgs)...);

		//todo: assert iterator belongs to this list
		Node* nextNode = const_cast<Node*>(before._node);
		Node* node = new (_proxy.allocate(1)) Node(std::forward<Args>(ctorArgs)...);

		node->next = nextNode;
		node->prev = nextNode->prev;
		nextNode->prev = node;

		if (node->prev)
			node->prev->next = node;
		else
			_first = node;

		++_size;
		return node->value;
	}

	constexpr T& Add(const ConstIterator& before, const T& element) { return Emplace(before, element); }
	constexpr T& Add(const ConstIterator& before, T&& element) { return Emplace(before, std::move(element)); }

	/*
		REMOVAL
	*/

	constexpr Iterator RemoveFront()
	{
		if (_first)
		{
			Node* next = _first->next;
			_first->value.~T();
			_proxy.deallocate(_first, 1);

			_first = next;
			if (!_first)
				_last = nullptr;

			--_size;
		}

		return _first;
	}

	constexpr Iterator RemoveBack()
	{
		if (_last)
		{
			Node* prev = _last->prev;
			_last->value.~T();
			_proxy.deallocate(_last, 1);

			_last = prev;
			if (!_last)
				_first = nullptr;

			--_size;
		}

		return end();
	}

	constexpr Iterator Remove(const ConstIterator& position)
	{
		//todo: verify iterator belongs to this list

		if (Node* node = const_cast<Node*>(position._node)) //const_cast is ok here
		{
			if (node->prev)
				node->prev->next = node->next;
			else
				_first = _first->next;

			Node* next = node->next;
			if (next)
				node->next->prev = node->prev;
			else
				_last = _last->prev;

			node->value.~T();
			_proxy.deallocate(node, 1);

			--_size;
			return next;
		}

		return end();
	}
};
