/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__filesystem__cascadevfs__hpp
#define __libamtrs__filesystem__cascadevfs__hpp
#include <list>
#include "vfs.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

// ============================================================================
//! 複数の vfs を保持します。
// ----------------------------------------------------------------------------
class	cascadevfs
		: public vfs
{
	using	vfsptr		= ref<vfs>;
	using	list_type	= std::list<vfsptr>;
public:
	template<class T>
	void push_front(T&& _fs)
	{
		mVfsList.push_front(std::forward<T>(_fs));
	}

	template<class T>
	void push_back(T&& _fs)
	{
		mVfsList.push_back(std::forward<T>(_fs));
	}

	void erase(vfs* _fs)
	{
		auto	it	= std::remove(mVfsList.begin(), mVfsList.end(), _fs);
		if (it != mVfsList.end())
		{
			mVfsList.erase(it, mVfsList.end());
		}
	}


protected:
	template<class Default, class Func>
	auto each(std::error_code& _ec, Default&& _defalut, Func&& _func) const
	{
		for (auto& vfs : mVfsList)
		{
			auto			retval = _func(vfs.get(), _ec);
			// ファイルを開くことができたのでファイルを返す。
			if (!_ec)
			{
				return	retval;
			}
			// ファイルが存在しない以外のエラーであれば、エラーとして返す。
			if (_ec != std::errc::no_such_file_or_directory)
			{
				return	_defalut();
			}
		}
		return	_defalut();
	}


	virtual ios::iovstream on_open(const path& _path, std::error_code& _ec) override
	{
		return	each
				(
					_ec,
					[&]() -> ios::iovstream { return {}; },
					[&](vfs* _vfs, std::error_code& ec) -> ios::iovstream
					{
						return	_vfs->open(_path, ec);
					}
				);
	}


	virtual filesystem::file_status on_status(const path& _path, std::error_code& _ec) const override
	{
		return	each
				(
					_ec,
					[&]() -> filesystem::file_status { return filesystem::file_status(filesystem::file_type::none, filesystem::perms::none); },
					[&](vfs* _vfs, std::error_code& ec) -> filesystem::file_status
					{
						return	_vfs->status(_path, ec);
					}
				);
	}


	virtual std::uintmax_t on_file_size(const path& _path, std::error_code& _ec) const override
	{
		return	each
				(
					_ec,
					[&]() -> std::uintmax_t { return std::uintmax_t(-1); },
					[&](vfs* _vfs, std::error_code& ec) -> std::uintmax_t
					{
						return	_vfs->file_size(_path, ec);
					}
				);
	}


	virtual bool on_remove(const path& _path, std::error_code& _ec) const override
	{
		return	each
				(
					_ec,
					[&]() -> bool { return false; },
					[&](vfs* _vfs, std::error_code& ec) -> bool
					{
						return	_vfs->remove(_path, ec);
					}
				);
	}

	virtual std::uintmax_t on_remove_all(const path& _path, std::error_code& _ec) const override
	{
		std::uintmax_t	count	= 0;
		for (auto& v : mVfsList)
		{
			std::error_code	ec;
			auto			retval = v->remove_all(_path, ec);
			if (ec)
			{
				// ファイルが存在しない以外のエラーであれば、エラーとして返す。
				if (ec != std::errc::no_such_file_or_directory)
				{
					_ec	= ec;
					return	count;
				}
			}
			count	+= retval;
			_ec	= ec;
		}
		return	count;
	}

	list_type	mVfsList;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
