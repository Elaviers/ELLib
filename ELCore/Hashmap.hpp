#pragma once
#include "Allocator.hpp"
#include "KeyDoesNotExist.hpp"
#include "Pair.hpp"
#include "MurmurHash3.hpp"
#include "Utilities.hpp"
#include <utility>

template <typename K, typename V,
	typename HasherType = MurmurHash3_32_FixedSeed,
	typename AllocatorType = DefaultAllocator<Pair<const K, V>>>
	class Hashmap
{
public:
	using size_type = size_t;
	using key_type = K;
	using value_type = V;

private:
	struct VNode
	{
		Pair<const K, V> kv;
		VNode* next;
		VNode* iNext;

		template <typename... Args>
		VNode(const K& key, Args&&... vargs) : kv{ key, V(std::forward<Args>(vargs)...) }, next(nullptr), iNext(nullptr) {}
	};

	using _HashType = HasherType::HashType;

	using _NodePtrAlloc = Utilities::ReplaceParam<AllocatorType, VNode*>::Type;
	using _NodeAlloc = Utilities::ReplaceParam<AllocatorType, VNode>::Type;
	class Proxy1 : public HasherType, public _NodeAlloc
	{
	public:
		VNode* iFirst;

		constexpr Proxy1() noexcept : iFirst(nullptr) {}
		constexpr Proxy1(const AllocatorType& allocator, const HasherType& hasher) : HasherType(hasher), _NodeAlloc(allocator), iFirst(nullptr) {}
		
		constexpr Proxy1(const Proxy1& other) : HasherType((const HasherType&)other), _NodeAlloc((const _NodeAlloc&)other), iFirst(nullptr) {}

		constexpr Proxy1(Proxy1&& other) noexcept : HasherType(std::move((HasherType&)other)), _NodeAlloc(std::move((_NodeAlloc&)other)), iFirst(other.iFirst)
		{
			other.iFirst = nullptr;
		}

		constexpr Proxy1& operator=(const Proxy1& other)
		{
			HasherType::operator=((const HasherType&)other);
			_NodeAlloc::operator=((const _NodeAlloc&)other);
			iFirst = nullptr;

			return *this;
		}

		constexpr Proxy1& operator=(Proxy1&& other) noexcept
		{
			HasherType::operator=(std::move((HasherType&)other));
			_NodeAlloc::operator=(std::move((_NodeAlloc&)other));
			iFirst = other.iFirst;
			other.iFirst = nullptr;

			return *this;
		}
	} _proxy_nodeAlloc_hasher_iFirst;

	class Proxy2 : public _NodePtrAlloc
	{
	public:
		VNode* iLast;

		constexpr Proxy2() : iLast(nullptr) {}
		constexpr Proxy2(const AllocatorType& allocator) : _NodePtrAlloc(allocator), iLast(nullptr) {}

		constexpr Proxy2(const Proxy2& other) : _NodePtrAlloc((const _NodePtrAlloc&)other), iLast(nullptr) {}

		constexpr Proxy2(Proxy2&& other) noexcept : _NodePtrAlloc(std::move((_NodePtrAlloc&)other)), iLast(other.iLast) {}

		constexpr Proxy2& operator=(const Proxy2& other) 
		{
			_NodePtrAlloc::operator=((const _NodePtrAlloc&)other);
			iLast = nullptr;

			return *this;
		}

		constexpr Proxy2& operator=(Proxy2&& other) noexcept
		{
			_NodePtrAlloc::operator=(std::move((_NodePtrAlloc&)other));
			iLast = other.iLast;
			other.iLast = nullptr;

			return *this;
		}
	} _proxy_nodePtrAlloc_iLast;

	VNode*& _iFirst = _proxy_nodeAlloc_hasher_iFirst.iFirst;
	VNode*& _iLast = _proxy_nodePtrAlloc_iLast.iLast;
	VNode** _buckets;
	size_type _bucketCount;
	size_type _iSize;
	float _maxLoad;

	//

	__forceinline constexpr void _Hash2Bucket(_HashType& hash) const noexcept
	{
		hash &= (_bucketCount - 1);
	}

	constexpr void _Rehash()
	{
		_HashType bindex;
		for (VNode* node = _iFirst; node; node = node->iNext)
		{
			Hashing::HashObject(_proxy_nodeAlloc_hasher_iFirst, node->kv.first, bindex);
			_Hash2Bucket(bindex);

			node->next = nullptr;
			VNode* bucket = _buckets[bindex];
			if (bucket)
			{
				while (bucket->next)
					bucket = bucket->next;

				bucket->next = node;
			}
			else
			{
				_buckets[bindex] = node;
			}
		}
	}

	constexpr void _EnforceLoad()
	{
		if (_bucketCount <= 0)
		{
			_bucketCount = 4;
			_buckets = _proxy_nodePtrAlloc_iLast.allocate(_bucketCount);
			_NullBuckets();
		}

		if ((float)_iSize / (float)_bucketCount > _maxLoad)
		{
			_bucketCount *= 2;
			_proxy_nodePtrAlloc_iLast.deallocate(_buckets, _bucketCount);
			_buckets = _proxy_nodePtrAlloc_iLast.allocate(_bucketCount);
			_NullBuckets();

			_Rehash();
		}
	}

	constexpr void _DestroyVNodes()
	{
		VNode* next;
		while (_iFirst)
		{
			next = _iFirst->next;
			delete _iFirst;
			_iFirst = next;
		}
	}

	__forceinline constexpr void _NullBuckets() noexcept
	{
		for (size_type i = 0; i < _bucketCount; ++i)
			_buckets[i] = nullptr;
	}

	template <typename... Args>
	constexpr VNode* _Set(bool evenIfPresent, const K& key, Args&&... ctorArgs)
	{
		_EnforceLoad();

		_HashType bindex;
		Hashing::HashObject(_proxy_nodeAlloc_hasher_iFirst, key, bindex);
		_Hash2Bucket(bindex);

		VNode* bucket = _buckets[bindex];
		while (bucket)
		{
			if (bucket->kv.first == key)
			{
				if (evenIfPresent)
				{
					bucket->kv.second.~V();
					new (&bucket->kv.second) V(std::forward<Args>(ctorArgs)...);
				}

				return bucket;
			}
			else
			{
				if (bucket->next)
				{
					bucket = bucket->next;
					continue;
				}

				//end
				VNode* node = new (_proxy_nodeAlloc_hasher_iFirst.allocate(1)) VNode(key, std::forward<Args>(ctorArgs)...);
				_iLast->iNext = bucket->next = node;
				_iLast = node;
				++_iSize;
				return node;
			}
		}

		VNode* node = new (_proxy_nodeAlloc_hasher_iFirst.allocate(1)) VNode(key, std::forward<Args>(ctorArgs)...);
		_buckets[bindex] = node;

		if (_iFirst)
		{
			_iLast->iNext = node;
			_iLast = node;
		}
		else
		{
			_iFirst = _iLast = node;
		}

		++_iSize;
		return node;
	}

public:
	constexpr static const float defaultMaxLoad = 1.2f;

	constexpr Hashmap() noexcept : _buckets(nullptr), _bucketCount(0), _iSize(0), _maxLoad(defaultMaxLoad) {}

	constexpr Hashmap(const HasherType& hasher, const AllocatorType& allocator = AllocatorType()) noexcept :
		_proxy_nodeAlloc_hasher_iFirst(allocator, hasher),
		_proxy_nodePtrAlloc_iLast(allocator),
		_buckets(nullptr), _bucketCount(0), _iSize(0), _maxLoad(defaultMaxLoad) {}

	constexpr Hashmap(const AllocatorType& allocator) noexcept : Hashmap(HasherType(), allocator) {}

	template <typename AllocType>
	constexpr Hashmap(const Hashmap<K, V, HasherType, AllocType>& other) :
		_proxy_nodeAlloc_hasher_iFirst(other._proxy_nodeAlloc_hasher_iFirst),
		_proxy_nodePtrAlloc_iLast(other._proxy_nodePtrAlloc_iLast),
		_bucketCount(other._bucketCount), _iSize(0), _maxLoad(other._maxLoad)
	{
		_buckets = _proxy_nodePtrAlloc_iLast.allocate(_bucketCount);
		_NullBuckets();

		//todo: optimise HashTable copy?
		for (const Pair<const K, V>& kv : other)
			GetOrDefault(kv.first, kv.second);
	}

	constexpr Hashmap(Hashmap&& other) noexcept :
		_proxy_nodeAlloc_hasher_iFirst(std::move(other._proxy_nodeAlloc_hasher_iFirst)),
		_proxy_nodePtrAlloc_iLast(std::move(other._proxy_nodePtrAlloc_iLast)),
		_buckets(other._buckets),
		_bucketCount(other._bucketCount),
		_iSize(other._iSize),
		_maxLoad(other._maxLoad)
	{
		other._buckets = nullptr;
		other._bucketCount = other._iSize = 0;
		other._maxLoad = defaultMaxLoad;
	}

	constexpr ~Hashmap()
	{
		_proxy_nodePtrAlloc_iLast.deallocate(_buckets, _bucketCount);
		_DestroyVNodes();
	}

	template <typename AllocType>
	constexpr Hashmap& operator=(const Hashmap<K, V, HasherType, AllocType>& other)
	{
		Clear();

		_proxy_nodeAlloc_hasher_iFirst = other._proxy_nodeAlloc_hasher_iFirst;
		_proxy_nodePtrAlloc_iLast = other._proxy_nodePtrAlloc_iLast;

		for (const Pair<const K, V>& kv : other)
			GetOrDefault(kv.first, kv.second);

		return *this;
	}

	constexpr Hashmap& operator=(Hashmap&& other) noexcept
	{
		Clear();

		_proxy_nodeAlloc_hasher_iFirst = std::move(other._proxy_nodeAlloc_hasher_iFirst);
		_proxy_nodePtrAlloc_iLast = std::move(other._proxy_nodePtrAlloc_iLast);

		_buckets = other._buckets;
		_bucketCount = other._bucketCount;
		_iSize = other._iSize;
		_maxLoad = other._maxLoad;

		other._buckets = nullptr;
		other._bucketCount = 0;
		other._iSize = 0;
		other._maxLoad = defaultMaxLoad;

		return *this;
	}

	constexpr size_type GetSize() const noexcept { return _iSize; }
	constexpr float GetMaxLoad() const noexcept { return _maxLoad; }

	//If the average items per bucket exceeds the max load then table size will be increased
	constexpr void SetMaxLoad(float maxLoad)
	{
		if (_maxLoad != maxLoad)
		{
			_maxLoad = maxLoad;
			_EnforceLoad();
		}
	}

	constexpr void Clear()
	{
		_proxy_nodePtrAlloc_iLast.deallocate(_buckets, _bucketCount);
		_DestroyVNodes();

		_buckets = nullptr;
		_bucketCount = 0;
		_iSize = 0;
		_iFirst = _iLast = nullptr;
	}

	/*
		MEMBER ACCESS
	*/

	constexpr V* TryGet(const K& key) noexcept
	{
		if (_bucketCount)
		{
			_HashType bindex;
			Hashing::HashObject(_proxy_nodeAlloc_hasher_iFirst, key, bindex);
			_Hash2Bucket(bindex);

			VNode* bucket = _buckets[bindex];
			while (bucket)
			{
				if (bucket->kv.first == key)
					return &bucket->kv.second;

				bucket = bucket->next;
			}
		}

		return nullptr;
	}

	constexpr V& Get(const K& key)
	{
		if (_bucketCount)
		{
			_HashType bindex;
			Hashing::HashObject(_proxy_nodeAlloc_hasher_iFirst, key, bindex);
			_Hash2Bucket(bindex);

			VNode* bucket = _buckets[bindex];
			while (bucket)
			{
				if (bucket->kv.first == key)
					return bucket->kv.second;

				bucket = bucket->next;
			}
		}

		throw KeyDoesNotExist();
	}

	constexpr const V* TryGet(const K& key) const noexcept
	{
		return (const_cast<Hashmap*>(this)->TryGet(key));
	}

	constexpr const V& Get(const K& key) const
	{
		return (const_cast<Hashmap*>(this)->Get(key));
	}

	template <typename... Args>
	constexpr V& GetOrDefaultEmplace(const K& key, Args&&... defaultCtorArgs)
	{
		return _Set(false, key, std::forward<Args>(defaultCtorArgs)...)->kv.second;
	}

	constexpr V& GetOrDefault(const K& key, const V& defaultValue)
	{
		return _Set(false, key, defaultValue)->kv.second;
	}

	constexpr V& operator[](const K& key)
	{
		return GetOrDefaultEmplace(key);
	}

	template <typename... Args>
	constexpr V& Emplace(const K& key, Args&&... ctorArgs)
	{
		return _Set(true, key, std::forward<Args>(ctorArgs)...)->kv.second;
	}

	constexpr V& Set(const K& key, const V& value)
	{
		return _Set(true, key, value)->kv.second;
	}

	/*
		ITERATION
	*/

	class Iterator
	{
		VNode* _node;

		friend Hashmap;
		constexpr Iterator(VNode* node) noexcept : _node(node) {}

	public:
		constexpr Iterator() noexcept : _node(nullptr) {}

		constexpr Iterator& operator++() noexcept
		{
			_node = _node->iNext;
			return *this;
		}

		constexpr Pair<const K, V>& operator*() const { return _node->kv; }
		constexpr Pair<const K, V>* operator->() const { return &_node->kv; }

		constexpr operator bool() const noexcept { return _node; }
		constexpr bool operator==(const Iterator& other) const noexcept { return _node == other._node; }
		constexpr bool operator!=(const Iterator& other) const noexcept { return _node != other._node; }
	};

	class ConstIterator
	{
		const VNode* _node;

		friend Hashmap;
		constexpr ConstIterator(const VNode* node) noexcept : _node(node) {}

	public:
		constexpr ConstIterator() noexcept : _node(nullptr) {}

		constexpr ConstIterator(const Iterator& iterator) : _node(iterator.node) {}

		constexpr ConstIterator& operator++() noexcept
		{
			_node = _node->iNext;
			return *this;
		}

		constexpr const Pair<const K, V>& operator*() const { return _node->kv; }
		constexpr const Pair<const K, V>* operator->() const { return &_node->kv; }

		constexpr operator bool() const noexcept { return _node; }
		constexpr bool operator==(const ConstIterator& other) const noexcept { return _node == other._node; }
		constexpr bool operator!=(const ConstIterator& other) const noexcept { return _node != other._node; }
	};

	constexpr Iterator begin() noexcept { return _iFirst; }
	constexpr Iterator end() noexcept { return nullptr; }
	constexpr ConstIterator begin() const noexcept { return _iFirst; }
	constexpr ConstIterator end() const noexcept { return nullptr; }
};
