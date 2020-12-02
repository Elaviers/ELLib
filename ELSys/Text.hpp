#pragma once
#include <ELCore/String.hpp>

class Text
{
private:
	const String* _source;

protected:
	friend class TextProvider;
	Text(const String* source) : _source(source) {}

	void _SetSource(const String* source) { _source = source; }

public:
	virtual ~Text() {}

	String ToString() const { return _source ? *_source : ""; }
};
