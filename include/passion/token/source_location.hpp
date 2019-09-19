#ifndef	__passion__source_location__hpp
#define	__passion__source_location__hpp
#include "../psnvm.hpp"
#include "../def.hpp"
#include "../vm/def.hpp"
#include "location.hpp"
PASSION_NAMESPACE_BEGIN
//!	定義されている場所
class	source_location : public location
{
public:
	using	location_type	= location;

	name_type sourcename() const noexcept	{ return location::name(); }
	line_type sourceline() const noexcept { return location::line(); }

	const vmstring& sourcedata() const noexcept	{ return m_sourcedata; }

	source_location(const vmstring& _sourcecode, const name_type& _sourcename, line_type _line)
		: location(_sourcename, _line)
		, m_sourcedata(_sourcecode)
	{}
	source_location() = default;
	source_location(const source_location& ) = default;
	source_location(      source_location&&) = default;
	source_location& operator = (const source_location& ) = default;
	source_location& operator = (      source_location&&) = default;


private:
	vmstring		m_sourcedata;
};
PASSION_NAMESPACE_END
#endif
