#include "PropertyCollection.hpp"

PropertyCollection::PropertyCollection(const PropertyCollection& other)
{
	_properties.SetSize(other._properties.GetSize());

	for (size_t i = 0; i < _properties.GetSize(); ++i)
		_properties[i] = other._properties[i]->Clone();
}

PropertyCollection::~PropertyCollection()
{
	for (Property* property : _properties)
		delete property;
}

PropertyCollection& PropertyCollection::operator=(const PropertyCollection& other)
{
	Clear();

	_properties.SetSize(other._properties.GetSize());

	for (size_t i = 0; i < _properties.GetSize(); ++i)
		_properties[i] = other._properties[i]->Clone();

	return *this;
}

PropertyCollection& PropertyCollection::operator=(PropertyCollection&& other) noexcept
{
	_properties = std::move(other._properties);
	return *this;
}

void PropertyCollection::Clear()
{
	for (Property* property : _properties)
		delete property;

	_properties.Clear();
}

String PropertyCollection::HandleCommand(void* obj, const Buffer<String>& tokens, const Context& ctx) const
{
	if (tokens.GetSize() > 0)
	{
		Property* property = Find(tokens[0].ToLower());

		if (property)
		{
			Buffer<String> args(&tokens[1], tokens.GetSize() - 1);
			return property->HandleCommand(obj, args, ctx);
		}
	}

	return "";
}

void PropertyCollection::Transfer(const void* from, void* to, const Context& ctx) const
{
	for (size_t i = 0; i < _properties.GetSize(); ++i)
		if ((_properties[i]->GetFlags() & PropertyFlags::READONLY) == 0)
			_properties[i]->TransferTo(from, to, ctx);
}

Property* PropertyCollection::Find(const String& name) const
{
	for (size_t i = 0; i < _properties.GetSize(); ++i)
		if (_properties[i]->GetName().Equals(name, true))
			return _properties[i];

	return nullptr;
}
