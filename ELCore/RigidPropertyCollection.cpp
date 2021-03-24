#include "RigidPropertyCollection.hpp"

RigidPropertyCollection::~RigidPropertyCollection()
{
	for (RigidProperty* property : _properties)
		delete property;
}

void RigidPropertyCollection::Clear()
{
	for (RigidProperty* property : _properties)
		delete property;

	_properties.Clear();
}

RigidProperty* RigidPropertyCollection::Find(const String& name) const
{
	for (size_t i = 0; i < _properties.GetSize(); ++i)
		if (_properties[i]->GetName().Equals(name, true))
			return _properties[i];

	return nullptr;
}

String RigidPropertyCollection::HandleCommand(const Buffer<String>& tokens, const Context& ctx) const
{
	if (tokens.GetSize() > 0)
	{
		RigidProperty* property = Find(tokens[0].ToLower());

		if (property)
		{
			Buffer<String> args(&tokens[1], tokens.GetSize() - 1);
			return property->HandleCommand(args, ctx);
		}
	}

	return "";
}
