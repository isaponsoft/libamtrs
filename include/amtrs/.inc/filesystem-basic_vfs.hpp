/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__basic_vfs__hpp
#define __libamtrs__filesystem__basic_vfs__hpp
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

// ============================================================================
//! 仮想ファイルシステムのインターフェース
// ----------------------------------------------------------------------------
template<class StreamT, class PathT>
class	basic_vfs
		: public ref_object
{
public:
	using	stream_type		= StreamT;
	using	path_type		= PathT;
	using	filesize_type	= uintmax_t;
	using	size_type		= size_t;
	using	error_type		= std::error_code;

	stream_type open(const path_type& _path, error_type& _ec) {                                                     return on_open(_path, _ec);     }
	stream_type open(const path_type& _path                 ) { return throwmode(_path, [&](error_type& _ec) { return on_open(_path, _ec); }); }


	file_status		status		(const path_type& _path, error_type& _ec) const {                                          		    return on_status(_path, _ec);     }
	file_status		status		(const path_type& _path                 ) const { return throwmode(_path, [&](error_type& _ec) {	return on_status(_path, _ec); }); }

	filesize_type	file_size	(const path_type& _path, error_type& _ec) const {                                             			return on_file_size(_path, _ec);     }
	filesize_type	file_size	(const path_type& _path                 ) const { return throwmode(_path, [&](error_type& _ec) {	return on_file_size(_path, _ec); }); }


	bool			remove		(const path_type& _path, error_type& _ec) const {                                              		return on_remove(_path, _ec);     }
	bool			remove		(const path_type& _path                 ) const { return throwmode(_path, [&](error_type& _ec) {	return on_remove(_path, _ec); }); }
	size_type		remove_all	(const path_type& _path, error_type& _ec) const {                                              		return on_remove_all(_path, _ec);     }
	size_type		remove_all	(const path_type& _path                 ) const { return throwmode(_path, [&](error_type& _ec) {	return on_remove_all(_path, _ec); }); }

	// status(path_type) に対するユーティリティメソッド群
	#define	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(_name)	\
	bool _name(const path_type& _path, error_type& _ec) const {                 		                             return stdfs::_name(status(_path, _ec));     }\
	bool _name(const path_type& _path                      ) const { return throwmode(_path, [&](error_type& _ec) { return stdfs::_name(status(_path, _ec)); }); }
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(exists)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_block_file)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_character_file)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_directory)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_fifo)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_regular_file)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_socket)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_symlink)
	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD(is_other)
	#undef	__AMTRS_FILESYSTEM_VFS_STATUS_METHOD

protected:
	virtual stream_type		on_open      (const path_type& _path, error_type& _ec)       = 0;
	virtual file_status		on_status    (const path_type& _path, error_type& _ec) const = 0;
	virtual filesize_type	on_file_size (const path_type& _path, error_type& _ec) const = 0;
	virtual bool			on_remove    (const path_type& _path, error_type& _ec) const = 0;
	virtual size_type		on_remove_all(const path_type& _path, error_type& _ec) const = 0;

	template<class Call>
	static auto throwmode(const path_type& _path, Call&& _call)
	{
		error_type ec;
		auto			retval	= _call(ec);
		if (ec)
		{
			std::system_error	se(ec, _path.string());
			AMTRS_DEBUG_LOG("%s", se.what());
			throw	se;
		}
		return	retval;
	}
};




AMTRS_FILESYSTEM_NAMESPACE_END
#endif
