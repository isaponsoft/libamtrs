#ifndef	__passion__token__location__hpp
#define	__passion__token__location__hpp
#include "../psnvm.hpp"
#include "../def.hpp"
#include "../vm/types.hpp"
PASSION_NAMESPACE_BEGIN


class	location
{
public:
	using	name_type	= vmstring;
	using	line_type	= int;

	location(const name_type& _sourcename, line_type _line)
		: mName(_sourcename)
		, mLine(_line)
	{}

	location() = default;
	location(const location& ) = default;
	location(      location&&) = default;
	location& operator = (const location& ) = default;
	location& operator = (      location&&) = default;

	bool operator == (const location& _b) const noexcept { return name() == _b.name() && line() == _b.line(); }
	bool operator != (const location& _b) const noexcept { return !(*this == _b); }

	void swap(location& _b) noexcept { std::swap(mName, _b.mName); std::swap(mLine, _b.mLine); }

	name_type name() const noexcept	{ return mName; }
	line_type line() const noexcept { return mLine; }

	void set_name(name_type _name) noexcept { mName = _name; }
	void set_line(line_type _line) noexcept { mLine = _line; }

	void set_location(const location&  _loc) { *this = _loc; }
	void set_location(      location&& _loc) { *this = std::move(_loc); }

private:
	name_type		mName;
	line_type		mLine;
};
PASSION_NAMESPACE_END
#endif
