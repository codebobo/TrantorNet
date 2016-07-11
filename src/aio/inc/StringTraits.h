#ifndef STRING_TRAITS_H_
#define STRING_TRAITS_H_
#include <string>
#include <sstream>

template <class T>
std::string StringTraits(T content)
{
	std::stringstream sstr;
	sstr<<content;
	std::string ret;
	sstr>>ret;
	return ret;
}

template <>
inline std::string StringTraits(std::string content)
{
	return content;
}

template <>
inline std::string StringTraits(const char* content)
{
	return std::string(content);
}

template <>
inline std::string StringTraits(bool content)
{
	if(content)
	{
		return "true";
	}
	else
	{
		return "false";
	}
}

#endif
