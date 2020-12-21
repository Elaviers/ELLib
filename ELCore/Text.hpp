#pragma once
#include "String.hpp"

class Text
{
private:
	const String* _source;
	String _alt; //Used if no source

protected:
	friend class TextProvider;
	Text(const String* source) : _source(source) {}

	void _SetSource(const String* source) { _source = source; }

public:
	Text() : _source(nullptr) {}
	explicit Text(const String& string) : _source(nullptr), _alt(string) {}
	explicit Text(String&& string) : _source(nullptr), _alt(string) {}
	virtual ~Text() {}

	static Text FromString(const String& string) { return Text(string); }
	const String& ToString() const { return _source ? *_source : _alt; }

	bool operator==(const Text& other) const 
	{
		if (_source)
			return _source == other._source;

		return _alt == other._alt;
	}
};
