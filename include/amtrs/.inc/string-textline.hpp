/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__textline__hpp
#define	__libamtrs__string__textline__hpp
AMTRS_NAMESPACE_BEGIN

/*
 * 行単位でテキストを読みとる時に便利なクラス。
 */
template<class CharT, class Traits = std::char_traits<CharT>>
class	basic_textline
{
public:
	using	view_type	= std::basic_string_view<CharT, Traits>;

	basic_textline() = default;
	basic_textline(basic_textline const&) = default;
	basic_textline& operator = (basic_textline const&) = default;

	basic_textline(view_type _view)
		: mView(_view)
	{}

	bool empty() const noexcept
	{
		return	mView.empty();
	}

	view_type fetchline(bool _trimreturn = true) noexcept
	{
		auto	line	= AMTRS_NAMESPACE::readline(mView, true);
		return	_trimreturn ? trim(line, "\r\n") : line;
	}

	view_type readline(bool _trimreturn = true) noexcept
	{
		auto	line	= AMTRS_NAMESPACE::readline(mView, true);
		mView	= mView.substr(line.size());
		return	_trimreturn ? trim(line, "\r\n") : line;
	}

private:
	view_type	mView;
};
using	textline	= basic_textline<char>;
using	wtextline	= basic_textline<wchar_t>;


AMTRS_NAMESPACE_END
#endif
