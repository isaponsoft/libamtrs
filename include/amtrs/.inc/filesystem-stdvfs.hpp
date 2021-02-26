/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__stdvfs__hpp
#define __libamtrs__filesystem__stdvfs__hpp
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


// ============================================================================
//! 標準ファイルシステムに対するインターフェース
// ----------------------------------------------------------------------------
//! std::filesystem を仮想化します。
// ----------------------------------------------------------------------------
class	stdvfs : public vfs
{
public:
	static ref<stdvfs> get_instance()
	{
		if (instance() == nullptr)
		{
			instance() = new stdvfs();
		}
		return	instance();
	}

	virtual ~stdvfs()
	{
		instance() = nullptr;
	}

private:

	static stdvfs*& instance()
	{
		static	stdvfs*	thiz	= nullptr;
		return	thiz;
	}

protected:
	stdvfs() {}

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

	
	virtual io::iovstream on_open(path_type _path, std::error_code& _ec) override
	{
		struct	vfsstreambuf
				: public io::iovstream::vstreambuf::streamif
		{
			using	seekdir		= std::ios_base::seekdir;
			using	openmode	= std::ios_base::openmode;
			using	off_type	= std::streambuf::off_type;
			using	pos_type	= std::streambuf::pos_type;
			using	char_type	= std::istream::char_type;
			using	int_type	= std::istream::int_type;

			vfsstreambuf(std::ifstream&& _in)
				: fs(std::move(_in))
			{}

			virtual pos_type seekoff(off_type _off, seekdir _dir, openmode _which) override
			{
				return	fs.rdbuf()->pubseekoff(_off, _dir, _which);
			}

			virtual int_type underflow() override
			{
				return	fs.rdbuf()->sgetc();
			}

			virtual int_type uflow() override
			{
				return	fs.rdbuf()->sbumpc();
			}

			virtual std::streamsize xsgetn(char_type* _s, std::streamsize _count) override
			{
				return	fs.rdbuf()->sgetn(_s, _count);
			}

			std::ifstream	fs;
		};

		io::iovstream	retval;
		std::ifstream	fin((std::string)_path, std::ios::binary);
		if (fin.is_open())
		{
			retval	= io::iovstream(io::iovstream::vstreambuf(new vfsstreambuf(std::move(fin))));
		}
		else
		{
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
		}
		return	retval;
	}
};





AMTRS_FILESYSTEM_NAMESPACE_END
#endif
