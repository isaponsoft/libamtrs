/*-
 * Copyright(c)2017IsaoShibuya
 * ReleasedundertheMITlicense
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef	__libamtrs__filesystem__filesystem__hpp
#define	__libamtrs__filesystem__filesystem__hpp
#include <chrono>
#include "types.hpp"
#include "path.hpp"
#include "filesystem_error.hpp"
AMTRS_NAMESPACE_FILESYSTEM_BEGIN

// ****************************************************************************
// ステータス取得系関数群ここから
// ----------------------------------------------------------------------------
#define	__MAKE_FILETYPE_FUNCTIONS(_name)	\
template<class PathTraits> bool _name(const basic_path<PathTraits>& _path                      ) { return filesystem::_name(status(_path     )); }\
template<class PathTraits> bool _name(const basic_path<PathTraits>& _path, std::error_code& _ec) { return filesystem::_name(status(_path, _ec)); }\
                    inline bool _name(const path& _path                                        ) { return filesystem::_name(status(_path     )); }\
                    inline bool _name(const path& _path, std::error_code& _ec                  ) { return filesystem::_name(status(_path, _ec)); }\


#define	__MAKE_FILETYPE_FUNCTION(_type, _name)	\
                                inline bool _name(file_status _status) noexcept { return _status.type() == _type; }\
__MAKE_FILETYPE_FUNCTIONS(_name)


template<class PathTraits, class ErrorHelper> file_status status(const basic_path<PathTraits>& _path, ErrorHelper _error);
template<class PathTraits> file_status status(const basic_path<PathTraits>& _path                      ) { return status(_path, error::make<basic_path<PathTraits>>(   )); }
template<class PathTraits> file_status status(const basic_path<PathTraits>& _path, std::error_code& _ec) { return status(_path, error::make<basic_path<PathTraits>>(_ec)); }
                    inline file_status status(const path& _path                      ) { return status(_path, error::make<path>(   )); }
                    inline file_status status(const path& _path, std::error_code& _ec) { return status(_path, error::make<path>(_ec)); }


inline bool status_known(file_status _status) noexcept { return _status.type() != file_type::none; }
inline bool exists(file_status _status) noexcept { return _status.type() != file_type::none && _status.type() != file_type::not_found; }
__MAKE_FILETYPE_FUNCTIONS(exists)
__MAKE_FILETYPE_FUNCTION(file_type::block,     is_block         )
__MAKE_FILETYPE_FUNCTION(file_type::block,     is_block_file    )
__MAKE_FILETYPE_FUNCTION(file_type::character, is_character_file)
__MAKE_FILETYPE_FUNCTION(file_type::directory, is_directory     )
__MAKE_FILETYPE_FUNCTION(file_type::fifo,      is_fifo          )
__MAKE_FILETYPE_FUNCTION(file_type::regular,   is_regular_file  )
__MAKE_FILETYPE_FUNCTION(file_type::socket,    is_socket        )
__MAKE_FILETYPE_FUNCTION(file_type::symlink,   is_symlink       )
inline bool is_other(file_status _status) noexcept { return filesystem::exists(_status) && !filesystem::is_regular_file(_status) && !filesystem::is_directory(_status) && !filesystem::is_symlink(_status); }
__MAKE_FILETYPE_FUNCTIONS(is_other)


#undef	__MAKE_FILETYPE_FUNCTIONS
#undef	__MAKE_FILETYPE_FUNCTION
// ----------------------------------------------------------------------------
// ステータス取得系関数群ここまで
// ****************************************************************************



// ****************************************************************************
// カレントディレクトリの取得と設定
// ----------------------------------------------------------------------------
template<class PathTraits, class ErrorHelper> basic_path<PathTraits> current_path(                                           ErrorHelper _error);
template<class PathTraits>                    basic_path<PathTraits> current_path(                                                               ) { return current_path<PathTraits                >(       error::make<path>(   )); }
template<class PathTraits>                    basic_path<PathTraits> current_path(                                           std::error_code& _ec) { return current_path<PathTraits                >(       error::make<path>(_ec)); }
                                 inline path                         current_path(                                                               ) { return current_path<typename path::path_traits>(       error::make<path>(   )); }
                                 inline path                         current_path(                                           std::error_code& _ec) { return current_path<typename path::path_traits>(       error::make<path>(_ec)); }
template<class PathTraits, class ErrorHelper> void                         current_path(const basic_path<PathTraits>& _path, ErrorHelper _error);
template<class PathTraits>                    void                         current_path(const basic_path<PathTraits>& _path                      ) { return current_path<PathTraits                >(_path, error::make<path>(   )); }
template<class PathTraits>                    void                         current_path(const basic_path<PathTraits>& _path, std::error_code& _ec) { return current_path<PathTraits                >(_path, error::make<path>(_ec)); }
                                       inline void                         current_path(const path&                   _path                      ) { return current_path<typename path::path_traits>(_path, error::make<path>(   )); }
                                       inline void                         current_path(const path&                   _path, std::error_code& _ec) { return current_path<typename path::path_traits>(_path, error::make<path>(_ec)); }



// ****************************************************************************
// ファイルサイズの取得
// ----------------------------------------------------------------------------
template<class PathTraits, class ErrorHelper> std::uintmax_t file_size(const basic_path<PathTraits>& _path, ErrorHelper   _error);
template<class PathTraits                   > std::uintmax_t file_size(const basic_path<PathTraits>& _path                      ) { return file_size<PathTraits                >(       error::make<path>(   )); }
template<class PathTraits                   > std::uintmax_t file_size(const basic_path<PathTraits>& _path, std::error_code& _ec) { return file_size<PathTraits                >(       error::make<path>(_ec)); }
                                       inline std::uintmax_t file_size(const path&                   _path                      ) { return file_size<typename path::path_traits>(_path, error::make<path>(   )); }
                                       inline std::uintmax_t file_size(const path&                   _path, std::error_code& _ec) { return file_size<typename path::path_traits>(_path, error::make<path>(_ec)); }



// ****************************************************************************
// 未実装項目
// ----------------------------------------------------------------------------

#if 0
path absolute(const path& p);
path absolute(const path& p, std::error_code& ec);


path canonical(const path& p, const path& base = current_path() );
path canonical(const path& p, std::error_code& ec );
path canonical(const path& p, const path& base, std::error_code& ec );

path weakly_canonical(const path& p);
path weakly_canonical(const path& p, std::error_code& ec);

path relative(const path& p, std::error_code& ec);
path relative(const path& p, const path& base = current_path());
path relative(const path& p, const path& base, std::error_code& ec);

path proximate(const path& p, std::error_code& ec);
path proximate(const path& p, const path& base = current_path());
path proximate(const path& p, const path& base, std::error_code& ec);




template<class PathTraits> file_status symlink_status(const basic_path<PathTraits>& p);
template<class PathTraits> file_status symlink_status(const basic_path<PathTraits>& p, std::error_code& ec);


template<class PathTraits> std::uintmax_t file_size(const basic_path<PathTraits>& p );
template<class PathTraits> std::uintmax_t file_size(const basic_path<PathTraits>& p, std::error_code& ec);

template<class PathTraits> bool remove(const basic_path<PathTraits>& p);
template<class PathTraits> bool remove(const basic_path<PathTraits>& p, std::error_code& ec);
template<class PathTraits> std::uintmax_t remove_all(const basic_path<PathTraits>& p);
template<class PathTraits> std::uintmax_t remove_all(const basic_path<PathTraits>& p, std::error_code& ec);

template<class PathTraits> basic_path<PathTraits> temp_directory_path();
template<class PathTraits> basic_path<PathTraits> temp_directory_path(std::error_code& ec);
#endif


AMTRS_NAMESPACE_FILESYSTEM_END
#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include "filesystem-win32.hpp"
#else
#include "filesystem-posix.hpp"
#endif
#endif
