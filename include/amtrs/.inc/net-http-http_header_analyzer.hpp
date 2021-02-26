/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__http__http_header_analyzer__hpp
#define	__libamtrs__net__http__http_header_analyzer__hpp
AMTRS_NET_HTTP_NAMESPACE_BEGIN


template<class CharT>
struct	http_header_analyzer_traits
{
};

template<class CharT, class Traits, class Ptr>
class	http_header_analyzer_iterator
{
public:
	http_header_analyzer_iterator() = default;
	http_header_analyzer_iterator(http_header_analyzer_iterator const&) = default;
	http_header_analyzer_iterator(http_header_analyzer_iterator&&) = default;
	http_header_analyzer_iterator& operator = (http_header_analyzer_iterator const&) = default;
	http_header_analyzer_iterator& operator = (http_header_analyzer_iterator&&) = default;
	http_header_analyzer_iterator(Ptr _ptr) : mPointer(_ptr) {}

	http_header_analyzer_iterator& operator ++ () noexcept
	{
		mPointer	= mPointer->next;
		return	*this;
	}

	http_header_analyzer_iterator& operator ++ (int) noexcept
	{
		http_header_analyzer_iterator	retval(*this);
		mPointer	= mPointer->next;
		return	retval;
	}

	bool operator == (http_header_analyzer_iterator const& _r) const noexcept { return mPointer == _r.mPointer; }
	bool operator != (http_header_analyzer_iterator const& _r) const noexcept { return mPointer != _r.mPointer; }

	auto* operator -> () const noexcept { return &mPointer->data; }
	auto& operator *  () const noexcept { return  mPointer->data; }

protected:
	Ptr		mPointer	= nullptr;
};

template<class CharT, class Traits = http_header_analyzer_traits<CharT>>
class	basic_http_header_analyzer
{
public:
	using	char_type		= CharT;
	using	traits_type		= Traits;
	using	string_view		= std::basic_string_view<CharT>;
	using	string_type		= std::basic_string<CharT>;
private:
	struct	header
	{
		std::pair<string_type, string_type>	data;
		header*								next	= nullptr;
	};
public:
	using	iterator		= http_header_analyzer_iterator<CharT, Traits, header*>;
	using	const_iterator	= http_header_analyzer_iterator<CharT, Traits, header const*>;

	basic_http_header_analyzer() = default;
	basic_http_header_analyzer(basic_http_header_analyzer const& ) = delete;
	basic_http_header_analyzer(basic_http_header_analyzer&& _s)
		: mResponseCode(_s.mResponseCode)
		, mHeader(_s.mHeader)
	{
		_s.mResponseCode	= -1;
		_s.mHeader			= nullptr;
	}
	~basic_http_header_analyzer()
	{
		clear();
	}

	basic_http_header_analyzer& operator = (basic_http_header_analyzer const& ) = delete;

	basic_http_header_analyzer& operator = (basic_http_header_analyzer&& _s)
	{
		clear();
		mResponseCode		= _s.mResponseCode;
		mHeader				= _s.mHeader;
		_s.mResponseCode	= -1;
		_s.mHeader			= nullptr;
		return	*this;
	}

	void clear()
	{
		while (mHeader)
		{
			auto*	next	= mHeader->next;
			delete	mHeader;
			mHeader	= next;
		}
		mResponseCode	= -1;
	}

	bool add(string_view _line)
	{
		auto	kv		= split_header(_line);
		if (!kv.first.empty())
		{
			string_type		key		= string_type(kv.first);
			string_type		value	= string_type(kv.second);
			header*			h		= new header();
			h->data.first	= std::move(key);
			h->data.second	= std::move(value);
			if (mHeader)
			{
				auto*	f	= mHeader;
				while (f->next)
				{
					f	= f->next;
				}
				f->next	= h;
			}
			else
			{
				mHeader	= h;
			}
			return	true;

		}

		if (starts_with(_line, string_view("HTTP/")))
		{
			mVersionMajor	= _line[5] - '0';
			mVersionMainor	= _line[7] - '0';

			auto	code	= _line.substr(8);
			int		rc		= 0;
			for (auto c : code)
			{
				if (c >= '0' && c <= '9')
				{
					rc = rc * 10 + (c - '0');
				}
			}
			mResponseCode	= rc;

			return	true;
		}

		std::cout << "unkown head: " << _line << std::endl;
		return	false;
	}

	int response_code() const noexcept
	{
		return	mResponseCode;
	}


	int version_major() const noexcept { return mVersionMajor; }
	int version_minor() const noexcept { return mVersionMainor; }


	iterator begin() noexcept { return mHeader; }
	const_iterator begin() const noexcept { return mHeader; }
	const_iterator cbegin() const noexcept { return mHeader; }
	iterator end() noexcept { return nullptr; }
	const_iterator end() const noexcept { return nullptr; }
	const_iterator cend() const noexcept { return nullptr; }

	const_iterator find(string_view _key) const noexcept
	{
		for (auto it = begin(); it != end(); ++it)
		{
			if (it->first == _key)
			{
				return	it;
			}
		}
		return	end();
	}


protected:

	int		mResponseCode	= -1;
	int		mVersionMajor	= 0;
	int		mVersionMainor	= 0;
	header*	mHeader			= nullptr;

	template<class, class, class>
	friend	class	http_header_analyzer_iterator;
};

using	http_header_analyzer	= basic_http_header_analyzer<char>;
AMTRS_NET_HTTP_NAMESPACE_END
#endif
