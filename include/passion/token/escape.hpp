#ifndef	__passion__token__escape__hpp
#define	__passion__token__escape__hpp
#include "../def.hpp"
PASSION_NAMESPACE_BEGIN




inline std::string decodeEspaceSequence(const std::string& string)
{
	unsigned int	uFirst	= string[0];
	std::size_t		nIndex	= 1;
	std::string		strResult;
	while (nIndex < string.size())
	{
		unsigned int	code = string[nIndex];
		++nIndex;
		if (code == uFirst)
		{
			if (nIndex < string.size())
			{
				strResult += std::string(1, static_cast<char>(code));
				++nIndex;
			}
			else
			{
				break;
			}
		}
		else
		if (code == '\\')
		{
			if (string[nIndex] == 'n')
			{
				strResult += std::string(1, '\n');
			}
			else
			{
				strResult += std::string(1, string[nIndex]);
			}
			++nIndex;
		}
		else
		{
			strResult += std::string(1, static_cast<char>(code));
		}
	}
	return	strResult;
}



inline std::string removeQuotation(const std::string& string)
{
	unsigned int	uFirst	= string[0];
	std::size_t			nIndex	= 1;
	std::string			strResult;
	while (nIndex < string.size())
	{
		unsigned int	code = string[nIndex];
		++nIndex;
		if (code == uFirst)
		{
			if (nIndex < string.size())
			{
				strResult += std::string(1, static_cast<char>(code));
				++nIndex;
			}
			else
			{
				break;
			}
		}
		else
		if (code == '\\')
		{
			strResult += std::string(1, static_cast<char>(code));
			strResult += std::string(1, string[nIndex]);
			++nIndex;
		}
		else
		{
			strResult += std::string(1, static_cast<char>(code));
		}
	}
	return	strResult;
}

PASSION_NAMESPACE_END
#endif
