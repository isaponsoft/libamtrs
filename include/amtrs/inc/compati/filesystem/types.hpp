/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__filesystem_types__hpp
#define	__libamtrs__filesystem_types__hpp
#include <chrono>
#include "../../amtrs.hpp"
#define	AMTRS_NAMESPACE_FILESYSTEM			filesystem
#define	AMTRS_NAMESPACE_FILESYSTEM_BEGIN	AMTRS_NAMESPACE_BEGIN namespace AMTRS_NAMESPACE_FILESYSTEM {
#define	AMTRS_NAMESPACE_FILESYSTEM_END		} AMTRS_NAMESPACE_END
#define	AMTRS_NAMESPACE_FILESYSTEM_FULL		AMTRS_NAMESPACE_FULL::AMTRS_NAMESPACE_FILESYSTEM


#ifdef	AMTRS_HAVE_STL17
	// STLの filesystem が存在する場合は
	// std::filesystem を使用する
	#include <experimental/filesystem>
	namespace std {
	namespace	filesystem	= std::experimental::filesystem;	// 正式版が出るまでの対応
	}
	#define	AMTRS_STL17_FILESYSTEM	1
#else
	// STL の filesystem が存在しない（もしくは使用できない）
	// 場合は std::filesystem を amtrs::filesystem で補う
	AMTRS_NAMESPACE_FILESYSTEM_BEGIN
	AMTRS_NAMESPACE_FILESYSTEM_END
	namespace std {
	namespace	filesystem	= AMTRS_NAMESPACE_FILESYSTEM_FULL;
	}
#endif

AMTRS_NAMESPACE_FILESYSTEM_BEGIN

// 基本型
#ifdef	AMTRS_STL17_FILESYSTEM
// std::filesystem があるなら、std::filesystem を使用する。
using	copy_options		= std::filesystem::copy_options;
using	directory_options	= std::filesystem::directory_options;
using	file_type			= std::filesystem::file_type;
using	perms				= std::filesystem::perms;
using	file_status			= std::filesystem::file_status;
using	file_time_type		= std::filesystem::file_time_type;
#else
// std::filesystem が無いなら列挙型などをエミュレート
// ==================================================================
//!	@brief	copy に使用されるオプション。
// ------------------------------------------------------------------
enum class	copy_options
{
	none				= 0,
	skip_existing		= 1,
	overwrite_existing	= 2,
	update_existing		= 3,
	recursive			= 4,
	copy_symlinks		= 5,
	skip_symlinks		= 6,
	directories_only	= 7,
	create_symlinks		= 8,
	create_hard_links	= 9,
};


// ==================================================================
//!	@brief	recursive_directory_iterator で使用される。
// ------------------------------------------------------------------
enum class	directory_options
{
	none					,
	follow_directory_symlink,
	skip_permission_denied	,
};


// ==================================================================
//!	@brief	ファイルの種類
// ------------------------------------------------------------------
enum class	file_type
{
	none		,
	not_found	,
	regular		,
	directory	,
	symlink		,
	block		,
	character	,
	fifo		,
	socket		,
	unknown		,
};


// ==================================================================
//!	@brief	所有権
// ------------------------------------------------------------------
enum class	perms
{
	none			= 0,
	owner_read		= 0400,			// S_IRUSR
	owner_write		= 0200,			// S_IWUSR
	owner_exec		= 0100,			// S_IXUSR
	owner_all		= 0700,			// S_IRWXU
	group_read		= 040,			// S_IRGRP
	group_write		= 020,			// S_IWGRP
	group_exec		= 010,			// S_IXGRP
	group_all		= 070,			// S_IRWXG
	others_read		= 04,			// S_IROTH
	others_write	= 02,			// S_IWOTH
	others_exec		= 01,			// S_IXOTH
	others_all		= 07,			// S_IRWXO
	all				= 0777,
	set_uid			= 04000,		// S_ISUID
	set_gid			= 02000,		// S_ISGID
	sticky_bit		= 01000,		// S_ISVTX
	mask			= 07777,
	unknown			= 0xFFFF
};




// ==================================================================
//!	@brief	ファイルの状態
//!			http://en.cppreference.com/w/cpp/filesystem/file_status
// ------------------------------------------------------------------
class	file_status
{
public:
	file_status() : file_status(file_type::none) { }
	file_status(const file_status& ) = default;
	file_status(      file_status&&) = default;
	explicit file_status(file_type _type, perms _permissions = perms::unknown) : m_type(_type), m_perms(_permissions) { }
	~file_status() = default;
	file_status& operator = (const file_status& ) = default;
	file_status& operator = (      file_status&&) = default;

	file_type type() const { return m_type; }
	void type(file_type _type) { m_type = _type; }

	perms permissions() const { return m_perms; }
	void permissions(perms _perms) { m_perms = _perms; }

protected:
	file_type	m_type;
	perms		m_perms;
};


// ==================================================================
//!	@brief	ファイルのタイムスタンプ
//!			http://en.cppreference.com/w/cpp/filesystem/file_status
// ------------------------------------------------------------------
using	file_time_type	= std::chrono::time_point<std::chrono::system_clock>;
#endif


// Prototypes. Not use.
template<class PathTraits>
class	basic_path;

template<class>
class	basic_filesystem_error;

template<class>
class	basic_directory_entry;

template<class>
class	basic_directory_iterator;

template<class>
class	basic_recursive_directory_iterator;



// ==================================================================
//!	@brief	ファイルのパスの特性を示します。
//!	文字型、ディレクトリセパレータ、ルート名の有無などを示します。
// ------------------------------------------------------------------
struct	path_traits_generic
{
	using	value_type	= char;
	using	root_name	= std::false_type;
	using	distinction	= std::true_type;

	static constexpr value_type	separator		= '/';

	// 使用しても良い字かどうか
	template<class V>
	static constexpr bool is_path_character(V _c) noexcept
	{
		return	_c != '/' && _c != '\0';
	}

	template<class V>
	static constexpr bool is_separator(V _c) noexcept
	{
		return	_c == '/';
	}

	template<class V>
	static constexpr int compare(V _l, V _r) noexcept
	{
		return	is_separator(_l) && is_separator(_r)
				? 0
				: _r - _l;
	}
};


// ==================================================================
//!	@brief	ファイルのパスの特性を示します。
//!	文字型、ディレクトリセパレータ、ルート名の有無などを示します。
// ------------------------------------------------------------------
struct	path_traits_windows
{
	using	value_type	= wchar_t;
	using	root_name	= std::integral_constant<value_type, ':'>;
	using	distinction	= std::false_type;

	static constexpr value_type	separator	= L'\\';

	// 使用しても良い字かどうか
	template<class V>
	static constexpr bool is_path_character(V _c) noexcept
	{
		return	_c != '\\' && _c != '/' && _c != ':' && _c != '*' && _c != '?' && _c != '<' && _c != '>' && _c != '|' && _c != '\0';
	}

	template<class V>
	static constexpr bool is_separator(V _c) noexcept
	{
		return	_c == '/' || _c == '\\';
	}

	template<class V>
	static constexpr int compare(V _l, V _r) noexcept
	{
		return	is_separator(_l) && is_separator(_r)
				? 0
				: _r - _l;
	}
};


// ==================================================================
//!	@brief	ファイルのパスの特性を示します。
//!	文字型、ディレクトリセパレータ、ルート名の有無などを示します。
// ------------------------------------------------------------------
struct	path_traits_macos
{
	using	value_type	= char;
	using	root_name	= std::false_type;
	using	distinction	= std::false_type;

	static constexpr value_type	separator		= '/';

	// 使用しても良い字かどうか
	template<class V>
	static constexpr bool is_path_character(V _c) noexcept
	{
		return	_c != '/';
	}

	template<class V>
	static constexpr bool is_separator(V _c) noexcept
	{
		return	_c == '/';
	}

	template<class V>
	static constexpr int compare(V _l, V _r) noexcept
	{
		return	is_separator(_l) && is_separator(_r)
				? 0
				: _r - _l;
	}
};


#if		AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
using		path_traits						= path_traits_windows;
#elif	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_MACOS
using		path_traits						= path_traits_macos;
#else
using		path_traits						= path_traits_generic;
#endif


using		path							= basic_path<path_traits>;
using		directory_entry					= basic_directory_entry<path>;
using		directory_iterator				= basic_directory_iterator<directory_entry>;
using		recursive_directory_iterator	= basic_recursive_directory_iterator<directory_iterator>;
using		filesystem_error				= basic_filesystem_error<path>;



AMTRS_NAMESPACE_FILESYSTEM_END

#endif
