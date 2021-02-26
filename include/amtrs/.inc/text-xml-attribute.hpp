/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__language__xml__attribute___hpp
#define	__libamtrs__language__xml__attribute___hpp
AMTRS_LANGUAGE_XML_NAMESPACE_BEGIN





// ============================================================================
//! 属性のフィールド名で始まっている場合は抜き出します。
// ----------------------------------------------------------------------------
//! _trim_head_space
//!		true の時は空白で始まっている場合に空白をスキップします。
// ----------------------------------------------------------------------------
template<class CharT, class CharTraits>
std::basic_string_view<CharT> attribute_field_starts_with(std::basic_string_view<CharT, CharTraits> _in, bool _trim_head_space = true)
{
	auto	view	= _in;
	if (_trim_head_space)
	{
		// 頭のスペースを取り除いてから調べる
		view	= skipspace(view);
	}

	// アルファベットから始まっていない場合は一致しない扱い
	if (view.empty() || !isattributefirst(view[0]))
	{
		return	_in.substr(0, 0);
	}

	auto	top	= view.data();
	view	= view.substr(1);
	for (;;)
	{
		// 何も残っていないので全部フィールド名として扱う
		if (!view.empty())
		{
			auto	c	= view[0];
			if (isattributesecond(c))
			{
				view	= view.substr(1);
				continue;
			}
		}
		return	_in.substr(top - _in.data(), view.data() - top);
	}
}



// ****************************************************************************
//! アトリビュートのフィールドと値を分離します。
// ----------------------------------------------------------------------------
template<class CharT>
class	basic_attribute
{
public:
	using	view_type	= std::basic_string_view<CharT>;
	using	value_type	= view_type;

	basic_attribute() = default;
	basic_attribute(const basic_attribute&) = default;

	basic_attribute(view_type _attr, bool _trim_head_space = true)
	{
		if (_attr.empty())
		{
			return;
		}

		auto	xml	= _attr;

		mField	= attribute_field_starts_with(xml, _trim_head_space);
		if (mField.empty())
		{
			mValue	= mField;
			return;
		}

		// = でなければそこで終了
		xml	= skipspace(xml.substr(mField.data() + mField.size() - xml.data()));
		if (xml.empty() || (xml[0] != '='))
		{
			mValue	= mField;
			mField	= _attr.substr(0, 0);
			return;
		}

		// 値を調べる
		xml	= skipspace(xml.substr(1));
		if (xml.empty() || (xml[0] != '\'' && xml[0] != '\"'))
		{
			mField	= _attr.substr(0, 0);
			mValue	= _attr.substr(0, 0);
			return;
		}

		auto	val	= xml.data();
		auto	f	= xml[0];
		auto	v	= find_view_if(xml, [f](auto _view)
					{
						if (_view[0] == f)
						{
							return	_view.substr(0, 1);
						}
						return	_view.substr(0, 0);
					});
		if (v.empty())
		{
			mField	= _attr.substr(0, 0);
			mValue	= _attr.substr(0, 0);
			return;
		}
		mValue	= _attr.substr(val - _attr.data(), v.data() + v.size() - val);
	}


	view_type const& field() const noexcept { return mField; }
	view_type const& value() const noexcept { return mValue; }
	bool empty() const noexcept { return mField.empty() && mValue.empty(); }

	auto data() const noexcept { return mField.data(); }
	auto size() const noexcept { return mValue.data() + mValue.size() - mField.data(); }

protected:
	view_type	mField;
	view_type	mValue;
};



// ============================================================================
//! 属性から始まっている場合、属性をひとつだけ抜き出します。
// ----------------------------------------------------------------------------
//! _trim_head_space
//!		true の時は空白で始まっている場合に空白をスキップします。
// ----------------------------------------------------------------------------
template<class CharT, class CharTraits>
std::basic_string_view<CharT> attribute_starts_with(std::basic_string_view<CharT, CharTraits> _in, bool _trim_head_space = true)
{
	basic_attribute<CharT>	attr(_in, _trim_head_space);
	if (attr.empty())
	{
		return	_in.substr(0, 0);
	}
	return	_in.substr(attr.field().data() - _in.data(), attr.value().data() + attr.value().size() - attr.field().data());
}



template<class CharT>
class	basic_attribute_iterator
{
public:
	using	view_type	= std::basic_string_view<CharT>;
	using	value_type	= attribute;

	class	iterator
	{
	public:
		iterator() = default;
		iterator(const iterator&) = default;
		iterator& operator = (const iterator&) = default;

		attribute const* operator -> () const noexcept
		{
			return	&mCurrent;
		}

		attribute const& operator * () const noexcept
		{
			return	mCurrent;
		}

		iterator& operator ++ () noexcept
		{
			mRange		= mRange.substr(mCurrent.data() + mCurrent.size() - mRange.data());
			mCurrent	= attribute_starts_with(mRange);
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
			return	mRange.data() == _r.mRange.data() || (mCurrent.empty() && _r.mCurrent.empty());
		}

		bool operator != (const iterator& _r) const noexcept
		{
			return	!(*this == _r);
		}


	private:
		iterator(view_type _range)
			: mRange(_range)
			, mCurrent(attribute_starts_with(mRange))
		{}

		view_type	mRange;
		attribute	mCurrent;

		friend	basic_attribute_iterator;
	};

	basic_attribute_iterator() = default;
	basic_attribute_iterator(const basic_attribute_iterator&) = default;
	basic_attribute_iterator& operator = (const basic_attribute_iterator&) = default;

	template<class... Args>
	basic_attribute_iterator(Args&&... _args)
		: mXML(std::forward<Args>(_args)...)
	{}

	auto begin() const noexcept { return iterator(mXML); }
	auto end() const noexcept { return iterator(view_type(mXML.data() + mXML.size(), 0)); }


protected:
	view_type	mXML;
};


AMTRS_LANGUAGE_XML_NAMESPACE_END
#endif
