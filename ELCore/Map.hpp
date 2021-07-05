#pragma once
#include "Allocator.hpp"
#include "KeyDoesNotExist.hpp"
#include "Pair.hpp"
#include "Utilities.hpp"
#include <utility>

template <typename K, typename V, typename AllocatorType = DefaultAllocator<Pair<const K, V>>>
class Map
{
public:
	using size_type = size_t;
	using key_type = K;
	using value_type = V;

private:
	enum class Dir { LEFT = false, RIGHT = true };

	struct Node
	{
		Pair<const K, V> kv;
		Node* parent;
		Node* left;
		Node* right;
		bool isRed;

		template <typename... Args>
		Node(const K& key, Args&&... args) : kv{ key, std::forward<Args>(args)... },
			parent(nullptr), left(nullptr), right(nullptr), isRed(true)
		{}
	};

	using _NodeAlloc = Utilities::ReplaceParam<AllocatorType, Node>::Type;

	class Proxy : public _NodeAlloc
	{
	public:
		Node* root;

		constexpr Proxy() noexcept : root(nullptr) {}
		constexpr Proxy(const Proxy& other) : _NodeAlloc((const _NodeAlloc&)other), root(nullptr) {}

		constexpr Proxy(Proxy&& other) noexcept : _NodeAlloc(std::move((_NodeAlloc&)other)), root(other.root)
		{
			other.root = nullptr;
		}

		constexpr Proxy& operator=(const Proxy& other) 
		{
			_NodeAlloc::operator=((const _NodeAlloc&)other);
			root = nullptr;
			return *this;
		}

		constexpr Proxy& operator=(Proxy&& other) noexcept
		{
			_NodeAlloc::operator=(std::move((_NodeAlloc&)other));
			root = other.root;
			other.root = nullptr;
			return *this;
		}
	} _proxy;

	Node*& _root = _proxy.root;
	Node* _iFirst;
	size_type _iSize;

	constexpr void _RotateLeft(Node* root) noexcept
	{
		Node* pivot = root->right;
		Node* pivotLeft = pivot->left;

		//pivotleft -> rootright
		if (pivotLeft) pivotLeft->parent = root;
		root->right = pivotLeft;

		//pivot -> root
		pivot->parent = root->parent;
		if (pivot->parent == nullptr) _root = pivot; //we're the new root
		else if (pivot->parent->right == root) pivot->parent->right = pivot;
		else pivot->parent->left = pivot;

		//root -> pivotleft
		root->parent = pivot;
		pivot->left = root;
	}

	constexpr void _RotateRight(Node* root) noexcept
	{
		Node* pivot = root->left;
		Node* pivotRight = pivot->right;

		//pivotright -> rootleft
		if (pivotRight) pivotRight->parent = root;
		root->left = pivotRight;

		//pivot -> root
		pivot->parent = root->parent;
		if (pivot->parent == nullptr) _root = pivot; //we're the new root
		else if (pivot->parent->right == root) pivot->parent->right = pivot;
		else pivot->parent->left = pivot;

		//root -> pivotright
		root->parent = pivot;
		pivot->right = root;
	}

	constexpr static Node* _NextNode(Node* node) noexcept
	{
		if (node->right)
		{
			//go to the leftmost branch of right for next value
			node = node->right;
			while (node->left)
				node = node->left;

			return node;
		}

		while (node->parent)
		{
			if (node->parent->left == node)
				return node->parent;

			//keep going up
			node = node->parent;
		}

		//we must be root with no right (end of map)
		return nullptr;
	}

	constexpr static Node* _FindLeaf(Node* node) noexcept
	{
		while (true)
		{
			while (node->left)
				node = node->left;

			if (node->right)
				node = node->right;
			else return node;
		}
	}

	constexpr void _DestroyAllNodes()
	{
		Node* node = _iFirst;
		Node* parent;

		while (node)
		{
			parent = node->parent;
			node->~Node();
			_proxy.deallocate(node, 1);

			if (parent)
			{
				if (node == parent->left)
					parent->left = nullptr;
				else
					parent->right = nullptr;

				node = _FindLeaf(parent);
			}
			else
				return;
		}
	}

	template <typename... Args>
	constexpr V& _Set(bool evenIfPresent, const K& key, Args&&... ctorArgs)
	{
		Node* parent = nullptr;
		Node* next = _root;
		while (next)
		{
			parent = next;
			if (key > parent->kv.first)
				next = parent->right;
			else if (key < parent->kv.first)
				next = parent->left;
			else // key of "parent" must be equal
			{
				if (evenIfPresent)
				{
					parent->kv.second.~V();
					new (&parent->kv.second) V(std::forward<Args>(ctorArgs)...);
				}

				return parent->kv.second;
			}
		}

		Node* kvNode = new (_proxy.allocate(1)) Node(key, std::forward<Args>(ctorArgs)...);
		if (parent == nullptr)
		{
			//Root does not exist
			kvNode->isRed = false; //Root is black
			_root = _iFirst = kvNode;
			_iSize = 1;
		}
		else
		{
			++_iSize;

			//Add to existing tree
			Node* node = kvNode;
			node->parent = parent;

			if (key > parent->kv.first) parent->right = node;
			else
			{
				parent->left = node;

				if (parent == _iFirst)
					_iFirst = node;
			}

			//Verify tree
			//do while false for loopback! little bit strange...
			while (true) {
				if (!parent->isRed)
					break;

				//parent is red

				if (Node* grandparent = parent->parent)
				{
					Dir parentDir = grandparent->right == parent ? Dir::RIGHT : Dir::LEFT;
					Node* uncle = (parentDir == Dir::RIGHT) ? grandparent->left : grandparent->right;

					if (uncle && uncle->isRed)
					{
						parent->isRed = false;
						uncle->isRed = false;
						grandparent->isRed = true;

						node = grandparent;
						parent = node->parent;

						if (parent == nullptr)
						{
							//we are a red root now, just turn black for neatness' sake (not strictly necessary)
							node->isRed = false;
							break;
						}

						continue; //repeat with node being grandparent
					}

					//uncle does not exist / is black vvv
					//we need to rotate parent to the grandparent
					Dir nodeDir = (parent->right == node) ? Dir::RIGHT : Dir::LEFT;
					if (nodeDir != parentDir)
					{
						//We're an inner child
						//Become an outer child via rotation...
						if (nodeDir == Dir::RIGHT) _RotateLeft(parent);
						else _RotateRight(parent);

						//parent is now the outer child that we need
						node = parent;
						parent = node->parent;
					}

					//We are an outer child, safe to do parent rotation
					if (parentDir == Dir::RIGHT) _RotateLeft(grandparent);
					else _RotateRight(grandparent);

					parent->isRed = false;
					grandparent->isRed = true;
				}

				//done
				break;
			} while (false);
		}

		return kvNode->kv.second;
	}

public:
	constexpr Map() noexcept : _iFirst(nullptr), _iSize(0) {}

	constexpr Map(const Map& other) : _proxy(other._proxy), _iFirst(nullptr), _iSize(0)
	{
		for (const Pair<const K, V>& kv : other)
			GetOrDefault(kv.first, kv.second);
	}

	constexpr Map(Map& other) noexcept : _proxy(std::move(other._proxy)), _iFirst(other._iFirst), _iSize()
	{
		other._iFirst = nullptr;
		other._iSize = 0;
	}

	constexpr ~Map()
	{
		_DestroyAllNodes();
	}

	constexpr Map& operator=(const Map& other)
	{
		Clear();
		_proxy = other._proxy;

		for (const Pair<const K, V>& kv : other)
			GetOrDefault(kv.first, kv.second);

		return *this;
	}

	constexpr Map& operator=(Map&& other) noexcept
	{
		_proxy = std::move(other._proxy);

		return *this;
	}

	constexpr void Clear()
	{
		_DestroyAllNodes();
		_root = nullptr;
		_iFirst = nullptr;
		_iSize = 0;
	}

	constexpr size_type GetSize() const
	{
		return _iSize;
	}

	/*
		MEMBER ACCESS
	*/

	constexpr V* TryGet(const K& key) noexcept
	{
		Node* node = _root;
		while (node)
		{
			if (key > node->kv.first)
				node = node->right;
			else if (key < node->kv.first)
				node = node->left;
			else
				return &node->kv.second;
		}

		return nullptr;
	}

	constexpr V& Get(const K& key)
	{
		V* vptr = TryGet(key);
		if (vptr) return *vptr;
		throw KeyDoesNotExist();
	}

	constexpr const V* TryGet(const K& key) const noexcept
	{
		return (const_cast<Map*>(this)->TryGet(key));
	}

	constexpr const V& Get(const K& key) const
	{
		return (const_cast<Map*>(this)->Get(key));
	}

	template <typename... Args>
	constexpr V& GetOrDefaultEmplace(const K& key, Args&&... defaultCtorArgs)
	{
		return _Set(false, key, std::forward<Args>(defaultCtorArgs)...);
	}

	constexpr V& GetOrDefault(const K& key, const V& defaultValue)
	{
		return _Set(false, key, defaultValue);
	}

	constexpr V& operator[](const K& key)
	{
		return GetOrDefaultEmplace(key);
	}

	template <typename... Args>
	constexpr V& Emplace(const K& key, Args&&... ctorArgs)
	{
		return _Set(true, key, std::forward<Args>(ctorArgs)...);
	}

	constexpr V& Set(const K& key, const V& value)
	{
		return _Set(true, key, value);
	}

	/*
		ITERATION
	*/

	class Iterator
	{
		Node* _node;

		friend Map;
		constexpr Iterator(Node* node) noexcept : _node(node) {}

	public:
		constexpr Iterator() noexcept : _node(nullptr) {}

		constexpr Iterator& operator++() noexcept
		{
			_node = _NextNode(_node);
			return *this;
		}

		constexpr operator bool() const noexcept { return _node; }
		constexpr bool operator==(const Iterator& other) const noexcept { return _node == other._node; }
		constexpr bool operator!=(const Iterator& other) const noexcept { return _node != other._node; }
		constexpr Pair<const K, V>& operator*() const { return _node->kv; }
		constexpr Pair<const K, V>* operator->() const { return &_node->kv; }

		constexpr Iterator Left() const { return _node->left; }
		constexpr Iterator Right() const { return _node->right; }
		constexpr Iterator Parent() const { return _node->parent; }
		constexpr bool IsRed() const { return _node->isRed; }
	};

	class ConstIterator
	{
		const Node* _node;

		friend Map;
		constexpr ConstIterator(Node* node) noexcept : _node(node) {}

	public:
		constexpr ConstIterator() noexcept : _node(nullptr) {}
		
		constexpr ConstIterator(const Iterator& iterator) noexcept : _node(iterator._node) {}

		constexpr ConstIterator& operator++() noexcept
		{
			_node = _NextNode(_node);
			return *this;
		}

		constexpr operator bool() const noexcept { return _node; }
		constexpr bool operator==(const ConstIterator& other) const noexcept { return _node == other._node; }
		constexpr bool operator!=(const ConstIterator& other) const noexcept { return _node != other._node; }
		constexpr const Pair<const K, V>& operator*() const { return _node->kv; }
		constexpr const Pair<const K, V>* operator->() const { return &_node->kv; }

		constexpr ConstIterator Left() const { return _node->left; }
		constexpr ConstIterator Right() const { return _node->right; }
		constexpr ConstIterator Parent() const { return _node->parent; }
		constexpr bool IsRed() const { return _node->isRed; }
	};

	constexpr Iterator begin() noexcept { return _iFirst; }
	constexpr Iterator end() noexcept { return nullptr; }
	constexpr Iterator Root() noexcept { return _root; }

	constexpr ConstIterator begin() const noexcept { return _iFirst; }
	constexpr ConstIterator end() const noexcept { return nullptr; }

	/*
		TEMP
	*/

	/*
	void _doimnodes(Node* node, int& id)
	{
		if (node)
		{
			imnodes::PushColorStyle(imnodes::ColorStyle_TitleBar, node->isRed ? IM_COL32(200, 20, 20, 255) : IM_COL32(0, 0, 0, 255));

			imnodes::BeginNode(id);
			imnodes::BeginNodeTitleBar();
			std::string kstring = std::to_string(node->kv.first);
			std::string vstring = std::to_string(node->kv.second);

			ImGui::Text("%s : %s", kstring.c_str(), vstring.c_str());
			imnodes::EndNodeTitleBar();

			imnodes::BeginInputAttribute(id + 1);
			ImGui::Text("parent");
			imnodes::EndInputAttribute();

			imnodes::BeginOutputAttribute(id + 2);
			ImGui::Text("left");
			imnodes::EndOutputAttribute();

			imnodes::BeginOutputAttribute(id + 3);
			ImGui::Text("right");
			imnodes::EndOutputAttribute();

			imnodes::EndNode();

			imnodes::PopColorStyle();

			int thisId = id;

			if (node->left)
			{
				int leftId = id + 10;
				id = leftId;
				_doimnodes(node->left, id);
				imnodes::Link(leftId + 4, thisId + 2, leftId + 1);
			}

			if (node->right)
			{
				int rightId = id + 10;
				id = rightId;
				_doimnodes(node->right, id);
				imnodes::Link(rightId + 5, thisId + 3, rightId + 1);
			}
		}
	}

	void _resetnodepos(Node* node, int& id, bool dir, int y = 0, int depth = 0)
	{
		int h = _totalheight(node);
		int offset = h + 50;
		if (!dir) offset *= -1;

		ImVec2 pos(depth * 150, y + offset);
		imnodes::SetNodeGridSpacePos(id, pos);

		if (node->left)
		{
			id += 10;
			_resetnodepos(node->left, id, false, pos.y, depth + 1);
		}

		if (node->right)
		{
			id += 10;
			_resetnodepos(node->right, id, true, pos.y, depth + 1);
		}
	}

	int _totalheight(Node* node)
	{
		int h = 0;
		if (node->left) h += 50 + _totalheight(node->left);
		if (node->right) h += 50 + _totalheight(node->right);

		return h;
	}

	void DoIMNodes()
	{
		int idtracker = 10;
		_doimnodes(_root, idtracker);


	}

	void ResetIMNodes()
	{
		int idtracker = 10;
		_resetnodepos(_root, idtracker, true);
	}
	*/
};
