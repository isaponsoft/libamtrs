/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__language__xml__element___hpp
#define	__libamtrs__language__xml__element___hpp
AMTRS_LANGUAGE_XML_NAMESPACE_BEGIN

enum class	elmtype
{
	emptytype		= 0,
	contentstype	= 1,
	tagtype			= 2
};


template<class CharT>
class	basic_element
		: public std::basic_string_view<CharT>
{
public:
	using	view_type	= std::basic_string_view<CharT>;

	static constexpr elmtype	emptytype		= elmtype::emptytype;
	static constexpr elmtype	contentstype	= elmtype::contentstype;
	static constexpr elmtype	tagtype			= elmtype::tagtype;

	basic_element() = default;
	basic_element(const basic_element&) = default;
	basic_element& operator = (const basic_element&) = default;

	basic_element(view_type const& _view, elmtype _type)
		: view_type(_view)
		, mType(_type)
	{}

	bool contents() const noexcept { return mType == elmtype::contentstype; }
	bool tag() const noexcept { return mType == elmtype::tagtype; }

	view_type const& view() const noexcept { return static_cast<view_type const&>(*this); }

	template<class R> bool operator == (R&& _r) const noexcept { return view() == std::forward<R>(_r); }
	template<class R> bool operator != (R&& _r) const noexcept { return view() != std::forward<R>(_r); }

private:
	elmtype	mType	= elmtype::emptytype;
};


using	element	= basic_element<char>;



// ============================================================================
//! 指定する string_view から要素をひとつだけ抜き出します。
// ----------------------------------------------------------------------------
template<class CharT, class CharTraits>
basic_element<CharT> parse_once_element(std::basic_string_view<CharT, CharTraits> _in)
{
	using	elm	= basic_element<CharT>;

	if (!_in.empty())
	{
		if (auto t = tag_starts_with(_in); !t.empty())
		{
			// タグとして扱う
			auto	cursor	= find_view_if(_in.substr(t.size()), [](auto _view)
							{
								if (_view[0] == '>')
								{
									return	_view.substr(0, 1);
								}
								return	_view.substr(0, 0);
							});
			return	cursor.empty()
					? elm(_in, elm::tagtype)			// > が見つからない場合は最後までをタグとして扱う
					: elm(_in.substr(0, cursor.data() + cursor.size() - _in.data()), elm::tagtype);
		}
		else
		{
			// コンテンツとして扱う。次のタグが見つかるまで抜き出す
			auto	cursor	= amtrs::find_view_if(_in, [](auto _view)
							{
								return	tag_starts_with(_view);
							});
			return	cursor.empty()
					? elm(_in, elm::contentstype)			// タグが見つからない場合は最後までをコンテンツとして扱う
					: elm(_in.substr(0, cursor.data() - _in.data()), elm::contentstype);
		}
	}
	return	elm(_in.substr(0, 0), elm::emptytype);
}


template<class CharT>
class	basic_element_iterator
{
public:
	using	view_type	= std::basic_string_view<CharT>;
	using	value_type	= basic_element<CharT>;

	class	iterator
	{
	public:
		iterator() = default;
		iterator(const iterator&) = default;
		iterator& operator = (const iterator&) = default;

		value_type const* operator -> () const noexcept
		{
			return	&mCurrent;
		}

		value_type const& operator * () const noexcept
		{
			return	mCurrent;
		}

		iterator& operator ++ () noexcept
		{
			mRange		= mRange.substr(mCurrent.size());
			mCurrent	= parse_once_element(mRange);
			return	*this;
		}

		iterator operator ++ (int) noexcept
		{
			iterator	retval(*this);
			++(*this);
			return	retval;
		}

		bool operator == (const iterator& _r) const noexcept
		{
			return	mRange.data() == _r.mRange.data();
		}

		bool operator != (const iterator& _r) const noexcept
		{
			return	!(*this == _r);
		}


	private:
		iterator(view_type _range)
			: mRange(_range)
			, mCurrent(parse_once_element(mRange))
		{
		}

		view_type	mRange;
		value_type	mCurrent;

		friend	basic_element_iterator;
	};

	basic_element_iterator() = default;
	basic_element_iterator(const basic_element_iterator&) = default;
	basic_element_iterator& operator = (const basic_element_iterator&) = default;

	template<class... Args>
	basic_element_iterator(Args&&... _args)
		: mXML(std::forward<Args>(_args)...)
	{}

	auto begin() const noexcept { return iterator(mXML); }
	auto end() const noexcept { return iterator(view_type(mXML.data() + mXML.size(), 0)); }


protected:
	view_type	mXML;
};

using	element_iterator	= basic_element_iterator<char>;




AMTRS_LANGUAGE_XML_NAMESPACE_END
#endif
