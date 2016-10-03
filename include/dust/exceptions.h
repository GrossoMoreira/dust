#ifndef _DUST_EXCEPTIONS_H_
#define _DUST_EXCEPTIONS_H_

#include <exception>

namespace dust
{

	struct invalid_argument : public std::exception
	{
		std::string whatstr;

		invalid_argument(std::string what) : whatstr(what)
		{
		}
	
		const char* what() const noexcept
		{
			return whatstr.c_str();
		}

	};

}

#endif // _DUST_EXCEPTIONS_H_
