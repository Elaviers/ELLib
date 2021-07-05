#pragma once
#include "Allocator.hpp"
#include "Buffer.hpp"
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "StringUtils.hpp"
#include "Types.hpp"
#include <iostream>
#include <utility>

class ByteReader;
class ByteWriter;

template <typename CharType, typename AllocatorType>
class TString;

namespace std
{
	template <typename CharType, typename AllocatorType>
	std::istream& getline(std::istream& stream, TString<CharType, AllocatorType>& string);
};

template <typename CharType = char, typename AllocatorType = DefaultAllocator<CharType>>
class TString {
public:
	using size_type = size_t;
	using char_type = CharType;

protected:
	class Proxy : public AllocatorType
	{
	public:
		CharType* data;
		size_type length;

		constexpr Proxy() noexcept : data((CharType*)&length), length(0) {}
		constexpr Proxy(const Proxy& other) : AllocatorType((const AllocatorType&)other), data((CharType*)&length), length(0) {}

		constexpr Proxy(Proxy&& other) noexcept : AllocatorType(std::move((AllocatorType&)other)), data((CharType*)&length), length(0)
		{
			//does not do anything, string has to manage this
		}

		constexpr Proxy& operator=(const Proxy& other)
		{
			((AllocatorType&)*this) = (const AllocatorType&)other;
			return *this;
		}

		constexpr Proxy& operator=(Proxy&& other) noexcept
		{
			((AllocatorType&&)*this) = std::move((AllocatorType&)other);

			//does not do anything, string has to manage this
			return *this;
		}
	} _proxy;

	CharType*& _data = _proxy.data;
	size_t& _length = _proxy.length;

	constexpr void _SetLength(size_t length)
	{
		if (length > 0)
		{
			size_t minLength = length < _length ? length : _length;
			CharType* newData = _proxy.allocate(length + 1);
			newData[length] = '\0';
			
			for (size_t i = 0; i < minLength; ++i)
				newData[i] = _data[i];

			_DeleteData();
			_length = length;
			_data = newData;
		}
		else
			Clear();
	}

	constexpr void _DeleteData() noexcept
	{
		if (_data != (CharType*)&_length)
			_proxy.deallocate(_data, _length + 1);
	}

	constexpr void _EmptyData() noexcept
	{
		_length = 0;
		_data = (CharType*)&_length;
	}

public:
	constexpr ~TString() noexcept
	{
		_DeleteData();
	}

	constexpr TString() noexcept
	{
		_EmptyData();
	}

	//Creates an array of length characters of c
	constexpr TString(CharType c, size_t length)
	{
		if (c == '\0') c = '0'; //Null characters are not allowed

		_SetLength(length);
		for (size_t i = 0; i < _length; ++i)
			_data[i] = c;
	}

	constexpr TString(const TString& other) : _proxy(other._proxy)
	{
		_SetLength(other._length);
		for (size_t i = 0; i < _length; ++i)
			_data[i] = other._data[i];
	}

	constexpr TString(TString&& other) noexcept : _proxy(std::move(other._proxy)) 
	{
		if (other._length > 0)
		{
			_data = other._data;
			_length = other._length;
			other._EmptyData();
		}
		else
			_EmptyData();
	}

	constexpr TString(const CharType* string)
	{
		_SetLength(StringUtils::Length(string));
		for (size_t i = 0; i < _length; ++i)
			_data[i] = string[i];
	}

	constexpr TString(CharType character)
	{
		_SetLength(1);
		_data[0] = character;
	}

	template <typename T>
	constexpr TString(const T* data, size_t length)
	{
		_SetLength(length);
		for (size_t i = 0; i < _length; ++i)
			_data[i] = (CharType)data[i];
	}

	constexpr const CharType* begin() const noexcept { return _data; }
	constexpr const CharType* end() const noexcept { return _data + _length; }
	constexpr size_type GetLength() const noexcept { return _length; }
	constexpr CharType& operator[](size_type position) const noexcept { return _data[position]; }

	constexpr void ShrinkTo(size_type length) { if (length < _length) _SetLength(length); }

	template <typename AllocType = DefaultAllocator<TString>>
	constexpr Buffer<TString, AllocType> Split(const CharType* delimiters) const
	{
		size_t delimiterCount = StringUtils::Length(delimiters);

		Buffer<TString, AllocType> result;

		size_t tokenCount = 0;

		for (size_t i = 0; i < _length;)
		{
			if (StringUtils::IsPartOfToken(_data[i], delimiters, delimiterCount))
			{
				tokenCount++;
				while (StringUtils::IsPartOfToken(_data[++i], delimiters, delimiterCount));
			}
			else i++;
		}

		if (tokenCount)
		{
			result.SetSize(tokenCount);

			size_t token = 0;
			for (size_t i = 0; i < _length;) {
				if (StringUtils::IsPartOfToken(_data[i], delimiters, delimiterCount)) {
					size_t length = 0;
					for (size_t j = i; StringUtils::IsPartOfToken(_data[j], delimiters, delimiterCount); ++j)
						++length;

					result[token]._SetLength(length);

					for (size_t j = 0; j < length; ++j)
						result[token][j] = _data[i + j];

					token++;
					while (StringUtils::IsPartOfToken(_data[++i], delimiters, delimiterCount));
				}
				else i++;
			}
		}

		return result;
	}
	
	//start is inclusive, end is exclusive
	constexpr TString SubString(size_type start, size_type end) const
	{
		if (start > _length || start >= end)
			return "";

		if (end > _length)
			end = _length;

		TString string;
		string._SetLength(end - start);
		for (size_t i = start; i < end; ++i)
			string._data[i - start] = _data[i];

		return string;
	}

	constexpr TString SubString(size_type start) const	{ return SubString(start, _length); }

	constexpr int IndexOf(const CharType* string, size_type start = 0) const
	{
		if (start < _length && string[0] != '\0')
		{
			const size_t len = StringUtils::Length(string);
			const size_t stop = (_length - len) + 1;
			for (size_t i = start; i < stop; ++i)
				if (_data[i] == string[0])
					if (StringUtils::Equal(_data + i, string, len))
						return (int)i;
		}

		return -1;
	}

	constexpr int LastIndexOf(const CharType* string, size_type start) const
	{
		if (start < _length)
		{
			const size_t len = StringUtils::Length(string);
			const size_t last = len - 1;
			if (len <= _length)
				for (size_t i = (start - len) + 2; i > 0; --i)
					if (_data[i - 1] == string[0])
						if (StringUtils::Equal(_data + i - 1, string, len))
							return (int)(i - 1);
		}

		return -1;
	}

	constexpr int IndexOfAny(const CharType* possibleChars, size_type start = 0) const
	{
		for (size_t i = start; i < _length; ++i)
			for (const char* c = possibleChars; *c != '\0'; ++c)
				if (_data[i] == *c)
					return (int)i;

		return -1;
	}

	constexpr int LastIndexOfAny(const CharType* possibleChars, size_type start) const
	{
		for (size_t i = start + 1; i > 0; --i)
			for (const char* c = possibleChars; *c != '\0'; ++c)
				if (_data[i - 1] == *c)
					return (int)(i - 1);

		return -1;
	}

	constexpr int LastIndexOf(const CharType* string) const { return LastIndexOf(string, _length - 1); }
	constexpr int LastIndexOfAny(const CharType* possibleChars) const { return LastIndexOfAny(possibleChars, _length - 1); }

	constexpr TString Replace(const CharType* src, const CharType* with) const
	{
		int end = IndexOf(src);
		if (end < 0)
			return TString(*this);

		size_t skip = StringUtils::Length(src);
		TString result = SubString(0, end) + with;

		size_t start = end + skip;
		while ((end = IndexOf(src, start)) > 0)
		{
			result += SubString(start, end) + with;
			start = end + skip;
		}

		result += SubString(start);
		return result;
	}

	constexpr void Clear() { _DeleteData(); _EmptyData(); }
	
	constexpr CharType& Insert(CharType c, size_t position)
	{
		if (position > _length)
			position = _length;

		CharType* newData = _proxy.allocate(_length + 2);

		for (size_t i = 0; i < position; ++i)
			newData[i] = _data[i];

		newData[position] = c;

		for (size_t i = position; i < _length; ++i)
			newData[i + 1] = _data[i];

		_DeleteData();
		_data = newData;
		_length++;
		_data[_length] = '\0';

		return _data[position];
	}

	constexpr TString& operator=(const TString& other)
	{
		_proxy = other._proxy; //doesnt touch data/length
		_SetLength(other._length);
		for (size_t i = 0; i < _length; ++i)
			_data[i] = other._data[i];

		return *this;
	}

	constexpr TString& operator=(TString&& other) noexcept
	{
		_DeleteData();
		_proxy = std::move(other._proxy);

		if (other._length > 0)
		{
			_data = other._data;
			_length = other._length;
			other._EmptyData();
		}
		else
			_EmptyData();

		return *this;
	}

	constexpr TString& operator+=(const TString& other)
	{
		const size_type originalLength = _length;
		_SetLength(_length + other._length);
		for (size_type i = 0; i < other._length; ++i)
			_data[originalLength + i] = other._data[i];

		return *this;
	}

	constexpr TString& operator+=(const CharType* string)
	{
		const size_type originalLength = _length;
		const size_t length = StringUtils::Length(string);
		_SetLength(_length + length);
		for (size_t i = 0; i < length; ++i)
			_data[originalLength + i] = string[i];

		return *this;
	}

	constexpr TString& operator+=(CharType character)
	{
		_SetLength(_length + 1);
		_data[_length - 1] = character;
		return *this;
	}

	constexpr TString operator+(const TString& other) const
	{
		TString product;
		product._SetLength(_length + other._length);

		for (size_t i = 0; i < _length; ++i)
			product[i] = _data[i];

		for (size_t i = 0; i < other._length; ++i)
			product[_length + i] = other._data[i];

		return product;
	}

	constexpr int Compare(const TString&other) const noexcept
	{
		const size_t minLength = _length < other._length ? _length : other._length;

		for (size_t i = 0; i < minLength; ++i)
			if (_data[i] != other[i])
				return _data[i] < other[i] ? -1 : 1;

		if (_length != other._length)
			return _length < other._length ? -1 : 1;

		return 0;
	}

	constexpr bool operator<(const TString& other) const noexcept { return Compare(other) < 0; }
	constexpr bool operator>(const TString& other) const noexcept { return Compare(other) > 0; }

	constexpr bool Equals(const TString& other, bool ignoreCase = false) const noexcept
	{ 
		return _length == other._length && 
			(ignoreCase ? StringUtils::Equal_CaseInsensitive(_data, other._data) : StringUtils::Equal(_data, other._data));
	}

	constexpr bool operator==(const TString& other) const noexcept { return _length == other._length && StringUtils::Equal(_data, other._data); }
	constexpr bool operator==(const CharType* other) const noexcept { return StringUtils::Equal(_data, other); }
	constexpr bool operator!=(const TString& other) const noexcept { return _length != other._length || StringUtils::NotEqual(_data, other._data); }
	constexpr bool operator!=(const CharType* other) const noexcept { return StringUtils::NotEqual(_data, other); }

	constexpr void Lower() noexcept
	{
		for (int i = 0; i < _length; ++i)
			_data[i] = StringUtils::LowerChar(_data[i]);
	}

	constexpr TString ToLower() const
	{
		TString string(*this);
		string.Lower();
		return string;
	}

	constexpr bool ToBool() const noexcept
	{
		TString lower = ToLower();

		if (lower == "t" || lower == "true" || ToInt() != 0)
			return true;

		return false;
	}

	constexpr int ToInt() const
	{
		return atoi(_data);
	}

	constexpr float ToFloat() const
	{
		return (float)atof(_data);
	}

	constexpr void Read(ByteReader& reader)
	{
		size_t len = 0;
		size_t readStart = reader.GetIndex();
		while (reader.Read_byte())
			++len;

		reader.SetIndex(readStart);
		size_t i = _length;
		_SetLength(_length + len);

		while (i < _length)
			_data[i++] = reader.Read_byte();

		//skip null byte
		reader.IncrementIndex(1);
	}

	constexpr void Write(ByteWriter& writer) const
	{
		writer.EnsureSpace(_length + 1);

		byte* const dest = writer.Ptr();
		dest[_length] = '\0';

		for (unsigned int i = 0; i < _length; ++i)
			dest[i] = _data[i];

		writer.IncrementIndex(_length + 1);
	}

	//
	template<typename T>
	static constexpr TString Concat(T first) { return From(first); }

	//Concatenates each argument interpreted as a string
	template<typename T, typename ...ARGS>
	static constexpr TString Concat(T first, const ARGS&... args) { return From(first) + Concat(args...); }

	//Concatenates args and returns a char pointer (which is deleted before the next statement)
	
	static constexpr TString FromInt(int64 number, unsigned int minimumDigits = 0, byte base = 10)
	{
		size_t digit_count = 1;

		for (int64 n = number / base; n; n /= base)
			++digit_count;

		if (digit_count < minimumDigits)
			digit_count = minimumDigits;

		bool neg = false;
		if (number < 0)
		{
			number *= -1;
			neg = true;
		}

		TString string;
		string._SetLength(digit_count + (neg ? 1 : 0));

		for (size_t i = 0; i < digit_count; ++i) {
			string[digit_count - 1 - i + (neg ? 1 : 0)] = StringUtils::ALPHA[number % base];
			number /= base;
		}

		if (neg)
			string[0] = '-';

		return string;
	}

	static constexpr TString FromFloat(double number, unsigned int minimumDigits = 0, unsigned int maxDecimalDigits = 3, byte base = 10)
	{
		if (number == INFINITY)
			return "infinity";

#pragma warning(push)
#pragma warning(disable : 26451)
		if (number == NAN)
#pragma warning(pop)
			return "NAN";

		int64 whole_number = (int64)number;
		double fraction = number - whole_number;

		TString whole_string;

		//This is here because negative numbers with a whole part of zero would return as if positive otherwise
		if (number < 0.0)
			whole_string = TString('-') + FromInt(-whole_number, minimumDigits, base);
		else
			whole_string = FromInt(whole_number, minimumDigits, base);

		size_t fraction_digit_count = 0;
		for (double d = fraction; d != 0.f && fraction_digit_count < maxDecimalDigits;) {
			d *= base;
			d -= (int)d;
			++fraction_digit_count;
		}

		TString fraction_string;
		fraction_string._SetLength(fraction_digit_count);
		byte digit;

		if (fraction < 0.0) fraction *= -1.0;

		for (size_t i = 0; i < fraction_digit_count; ++i) {
			fraction *= base;

			digit = (byte)fraction;
			fraction_string[i] = StringUtils::ALPHA[digit];
			fraction -= digit;
		}

		if (fraction_string.GetLength() > 0)
			return whole_string + '.' + fraction_string;

		return whole_string;
	}

	template <typename T>
	static constexpr TString From(const T& t) { return t.ToString(); }

	template <> static constexpr TString From(const int64& i) { return FromInt(i); }
	template <> static constexpr TString From(const double& f) { return FromFloat(f); }
	
	template <> static constexpr TString From(const bool& b) { return b ? TString("true") : TString("false"); }
	template <> static constexpr TString From(const TString& string) { return string; }
	template <> static constexpr TString From(const CharType& c) { return TString(c); }
	template <> static constexpr TString From(const unsigned char &c) { return FromInt(c, 0, 16); }

	template <typename T>
	constexpr T To() const { return T::FromString((const TString&)*this); }

	//todo: have actual functions for this instead of casting up...
	template <> constexpr int64 To() const { return (int64)ToInt(); }
	template <> constexpr double To() const { return (double)ToFloat(); }
	template <> constexpr bool To() const { return ToBool(); }
	template <> constexpr char To() const { return (char)ToInt(); }
	template <> constexpr unsigned char To() const { return (unsigned char)ToInt(); }
	template <> constexpr TString To() const { return *this; }

#define LINKFROM(FROMTYPE, TOTYPE) \
	template <> static constexpr TString From<FROMTYPE>(FROMTYPE const& x) { return From((TOTYPE const&)x); }

#define LINKBOTH(FROMTYPE, TOTYPE) \
	LINKFROM(FROMTYPE, TOTYPE) \
	template <> constexpr FROMTYPE To() const { return (FROMTYPE)To<TOTYPE>();}

	LINKFROM(const CharType*, TString);
	LINKFROM(CharType*, TString);
	LINKBOTH(short, int64);
	LINKBOTH(int, int64);
	LINKBOTH(long, int64);
	LINKBOTH(unsigned short, int64);
	LINKBOTH(unsigned int, int64);
	LINKBOTH(unsigned long, int64);
	LINKBOTH(unsigned long long, int64);
	LINKBOTH(float, double);

	friend std::ostream& operator<<(std::ostream& stream, TString& string);
	friend std::istream& operator>>(std::istream& stream, TString& string);
	friend std::istream& std::getline(std::istream& stream, TString& string);
};

using String = TString<char, DefaultAllocator<char>>;

namespace StringUtils
{
	inline String FromWide(const wchar_t* string)
	{
		String::size_type i = 0;

		while (string[++i] != 0x0000);
		String result('?', i);
		
		i = 0;
		do
		{
			if (string[i] <= 0xFF) result[i] = (char)string[i];
		} while (string[++i] != '\0');

		return result;
	}
}

#define CSTR(...) (String::Concat(__VA_ARGS__)).begin()

template <typename CharType, typename AllocType>
std::ostream& operator<<(std::ostream& stream, TString<CharType, AllocType>& string)
{
	stream.write(string._data, string._length);
	return stream;
}

template <typename CharType, typename AllocType>
std::istream& operator>>(std::istream& stream, TString< CharType, AllocType>& string)
{
	bool skip = true;
	while (true)
	{
		char c = stream.peek();
		if (c == ' ' || c == '\n' || c == '\r')
		{
			if (!skip) break;

			stream.get();
			continue;
		}

		skip = false;
		string += stream.get();
	}

	return stream;
}

namespace std
{
	template <typename CharType, typename AllocType>
	istream& getline(istream& stream, TString<CharType, AllocType>& string)
	{
		bool skip = true;
		while (true)
		{
			int c = stream.peek();
			if (c == '\n' || c == '\r')
			{
				if (!skip) break;

				stream.get();
				continue;
			}

			skip = false;
			string += stream.get();
		}

		return stream;
	}
};

#include "Hasher.hpp"

namespace Hashing
{
	template <typename HasherType, typename HASH_T>
	constexpr void HashObject(const HasherType& hasher, const String& object, HASH_T& out)
	{
		hasher.Hash(object.begin(), object.GetLength(), out);
	}
}
