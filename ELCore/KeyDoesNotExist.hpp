#pragma once
#include <exception>

class KeyDoesNotExist : public std::exception
{
public:
	virtual char const* what() const override
	{
		return "Key does not exist";
	}
};

