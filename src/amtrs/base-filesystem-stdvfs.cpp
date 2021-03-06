/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
AMTRS_FILESYSTEM_NAMESPACE_BEGIN



// ============================================================================
//! 標準ファイルシステムに対するインターフェース
// ----------------------------------------------------------------------------
//! std::filesystem を仮想化します。
// ----------------------------------------------------------------------------
class	stdvfs_impl : public stdvfs
{
public:
	static ref<stdvfs_impl> get_instance()
	{
		if (instance() == nullptr)
		{
			instance() = new stdvfs_impl();
		}
		return	instance();
	}

	virtual ~stdvfs_impl()
	{
		instance() = nullptr;
	}

private:

	static stdvfs_impl*& instance()
	{
		static	stdvfs_impl*	thiz	= nullptr;
		return	thiz;
	}

protected:
	stdvfs_impl() {}

	virtual file_status on_status(path_type _path, std::error_code& _ec) const override
	{
		struct stat	st;
		std::string	p(_path);
		if (::stat(p.c_str(), &st))
		{
			// ファイルが見つからなかった
			if (errno == ENOENT)
			{
				AMTRS_DEBUG_LOG("No such file \"%s\".", p.c_str());
				_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
			}
			return	file_status(file_type::not_found);
		}

		// file type.
		file_status		retval;
		retval.type
		(
			(st.st_mode & S_IFMT) == S_IFREG  ? file_type::regular   :
			(st.st_mode & S_IFMT) == S_IFDIR  ? file_type::directory :
#ifdef S_IFLNK
			(st.st_mode & S_IFMT) == S_IFLNK  ? file_type::symlink   :
#endif
#ifdef S_IFBLK
			(st.st_mode & S_IFMT) == S_IFBLK  ? file_type::block     :
#endif
			(st.st_mode & S_IFMT) == S_IFCHR  ? file_type::character :
#ifdef S_IFFIFO
			(st.st_mode & S_IFMT) == S_IFFIFO ? file_type::fifo      :
#endif
#ifdef S_IFSOCK
			(st.st_mode & S_IFMT) == S_IFSOCK ? file_type::socket    :
#endif
						   file_type::unknown
		);
		return	retval;
	}


	virtual std::uintmax_t on_file_size (path_type _path, std::error_code& _ec) const override
	{
		struct stat	st;
		std::string	p(_path);
		if (::stat(p.c_str(), &st))
		{
			if (errno == ENOENT)
			{
				_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
			}
			return	0;
		}
		return	st.st_size;
	}


	virtual bool on_remove(path_type _path, std::error_code& _ec) const override
	{
		return false;
	}

	virtual std::uintmax_t on_remove_all(path_type _path, std::error_code& _ec) const override
	{
		return 0;
	}

	
	virtual io::vstreamif on_open(path_type _path, std::error_code& _ec) override
	{

		io::vstreamif	retval;
		std::ifstream	fin((std::string)_path, std::ios::binary);
		if (fin.is_open())
		{
			auto	s	= io::make_streamif(std::move(fin));
			retval	= io::make_vstreamif(std::move(s));
		}
		else
		{
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
		}
		return	retval;
	}
};



ref<stdvfs> stdvfs::get_instance()
{
	return	stdvfs_impl::get_instance();
}


AMTRS_FILESYSTEM_NAMESPACE_END
