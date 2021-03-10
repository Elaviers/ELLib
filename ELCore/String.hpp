#pragma once
#include "Types.hpp"
#include <iostream>

template<typename T>
class Buffer;

class ByteReader;
class ByteWriter;

bool StringsEqual(const char *a, const char *b);
bool StringsEqual_CaseInsensitive(const char *a, const char *b);
bool StringsInequal(const char *a, const char *b);
bool StringContains(const char* string, const char* phrase);

bool StringsEqualN(const char* a, const char* b, size_t n);
bool StringsInequalN(const char* a, const char* b, size_t n);

inline char LowerChar(char c)
{
	if (c <= 'Z' && c >= 'A')
		return c + ('a' - 'A');

	return c;
}

inline size_t StringLength(const char* string) {
	size_t length = 0;
	while (string[length] != '\0') length++;
	return length;
}

class String;

namespace std
{
	std::istream& getline(std::istream& stream, String& string);
};

class String {
private:
	char *_data;
	size_t _length;

	void _SetLength(size_t length);
public:
	~String();
	String();

	//Creates an array of length characters of c
	String(char c, size_t length);

	String(const String &string);
	String(String&& string) noexcept;

	String(const char *string);
	String(char character);

	String(const Buffer<byte>& bytes);


	const char* const GetData() const		{ return _data; }
	size_t GetLength() const				{ return _length; }
	char& operator[](size_t position) const { return _data[position]; }

	Buffer<String> Split(const char *delimiters) const;
	
	//start is inclusive, end is exclusive
	String SubString(size_t start, size_t end) const;
	String SubString(size_t start) const	{ return SubString(start, _length); }

	int IndexOf(char, size_t start = 0) const;
	int LastIndexOf(char, size_t start) const;
	int IndexOf(const char*, size_t start = 0) const;
	int LastIndexOf(const char*, size_t start) const;

	int IndexOfAny(const char* possibleChars, size_t start = 0) const;
	int LastIndexOfAny(const char* possibleChars, size_t start) const;

	int LastIndexOf(char c) const { return LastIndexOf(c, _length - 1); }
	int LastIndexOf(const char* string) const { return LastIndexOf(string, _length - 1); }
	int LastIndexOfAny(const char* possibleChars) const { return LastIndexOfAny(possibleChars, _length - 1); }

	String Replace(const char* src, const char* with) const;

	void Clear()							{ _SetLength(0); }
	void Shrink(size_t amount)				{ _SetLength(_length - amount); }
	void ShrinkTo(size_t length)			{ if (length < _length) _SetLength(length); }

	char& Insert(char c, size_t position);

	String& operator=(const String&);
	String& operator=(String&&) noexcept;
	String& operator=(const char*);
	String& operator+=(const String&);
	String& operator+=(const char*);
	String& operator+=(char);

	String operator+(const String&) const;

	int Compare(const String &other) const;
	bool operator<(const String &other) const	{ return Compare(other) < 0;	}
	bool operator>(const String &other) const	{ return Compare(other) > 0;	}

	bool Equals(const String& other, bool ignoreCase = false) const 
	{ 
		return _length == other._length && 
			(ignoreCase ? StringsEqual_CaseInsensitive(_data, other._data) : StringsEqual(_data, other._data)); 
	}

	bool operator==(const String &other) const { return _length == other._length && StringsEqual(_data, other._data); }
	bool operator==(const char *other) const { return StringsEqual(_data, other); }
	bool operator!=(const String &other) const { return _length != other._length || StringsInequal(_data, other._data); }
	bool operator!=(const char *other) const { return StringsInequal(_data, other); }

	void Lower();
	String ToLower() const;

	bool ToBool() const;
	int ToInt() const;
	float ToFloat() const;

	void Read(ByteReader&);
	void Write(ByteWriter&) const;

	//
	template<typename T>
	static String Concat(T first) { return From(first); }

	//Concatenates each argument interpreted as a string
	template<typename T, typename ...ARGS>
	static String Concat(T first, const ARGS&... args) { return From(first) + Concat(args...); }

	//Concatenates args and returns a char pointer (which is deleted before the next statement)
	#define CSTR(...) (String::Concat(__VA_ARGS__)).GetData()

	static String FromInt(int64, unsigned int minimumDigits = 0, byte base = 10);
	static String FromFloat(double, unsigned int minimumDigits = 0, unsigned int maxDecimalDigits = 3, byte base = 10);
	static String FromWide(const wchar_t*);

	template <typename T>
	static String From(const T& t) { return t.ToString(); }

	template <> static String From(const int64& i) { return FromInt(i); }
	template <> static String From(const double& f) { return FromFloat(f); }
	template <> static String From(const wchar_t* const& string) { return FromWide(string); }

	template <> static String From(const bool& b) { return b ? String("true") : String("false"); }
	template <> static String From(const String& string) { return string; }
	template <> static String From(const char& c) { return String(c); }
	template <> static String From(const unsigned char &c) { return FromInt(c, 0, 16); }

	template <typename T>
	T To() const { return T::FromString((const String&)*this); }

	//todo: have actual functions for this instead of casting up...
	template <> int64 To() const { return (int64)ToInt(); }
	template <> double To() const { return (double)ToFloat(); }
	template <> bool To() const { return ToBool(); }
	template <> char To() const { return (char)ToInt(); }
	template <> unsigned char To() const { return (unsigned char)ToInt(); }
	template <> String To() const { return *this; }

#define LINKFROM(FROMTYPE, TOTYPE) \
	template <> static String From<FROMTYPE>(FROMTYPE const& x) { return From((TOTYPE const&)x); }

#define LINKBOTH(FROMTYPE, TOTYPE) \
	LINKFROM(FROMTYPE, TOTYPE) \
	template <> FROMTYPE To() const { return (FROMTYPE)To<TOTYPE>();}

	LINKFROM(const char*, String);
	LINKFROM(char*, String);
	LINKFROM(wchar_t*, const wchar_t*);
	LINKBOTH(short, int64);
	LINKBOTH(int, int64);
	LINKBOTH(long, int64);
	LINKBOTH(unsigned short, int64);
	LINKBOTH(unsigned int, int64);
	LINKBOTH(unsigned long, int64);
	LINKBOTH(unsigned long long, int64);
	LINKBOTH(float, double);

	friend std::ostream& operator<<(std::ostream& stream, String& string);
	friend std::istream& operator>>(std::istream& stream, String& string);
	friend std::istream& std::getline(std::istream& stream, String& string);
};
