/*
 * File selector node.
 *
 * Have not a header bar.
 */
#ifndef	__libamtrs__node__file_selector__hpp
#define	__libamtrs__node__file_selector__hpp
#include "../application.hpp"
#include "../filesystem.hpp"
#include "../inc/node/vertical_list_node.hpp"
#include "file_status.hpp"
AMTRS_NAMESPACE_BEGIN



// ============================================================================
//! ファイル選択モード
// ----------------------------------------------------------------------------
enum class	file_selector_mode
{
	fileselect,				//!< ファイル選択
	directoryselect,		//!< ディレクトリ（フォルダ）選択
};


// ============================================================================
//! ファイル選択の特性
// ----------------------------------------------------------------------------
template<class NodeT, class ItemNodeT = file_status_node<typename NodeT::node_type>>
struct	file_selector_traits
{
	using	node_type			= typename NodeT::node_type;
	using	select_callback		= void(std::string_view _select_path);
	using	path_type			= filesystem::path;
	using	file_status_type	= ItemNodeT;
};


// ============================================================================
//! ファイル選択
// ----------------------------------------------------------------------------
template<class SuperType, class Traits = file_selector_traits<SuperType>>
class	file_selector_node
		: public SuperType
{
public:
	using	super_type			= SuperType;
	using	node_type			= typename super_type::node_type;
	using	traits_type			= Traits;
	using	select_callback		= typename traits_type::select_callback;
	using	path_type			= typename traits_type::path_type;
	using	file_status_type	= typename traits_type::file_status_type;
	using	list_type			= vertical_list_node<node_type, file_status_type>;


	static const file_selector_mode	fileselect		= file_selector_mode::fileselect;
	static const file_selector_mode	directoryselect	= file_selector_mode::directoryselect;




	template<class... Args>
	file_selector_node(file_selector_mode _mode, Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
		, mDialogMode(_mode)
	{
		initialize();
	}


	//! Set current path.
	path_type set_current_directory(path_type&& _path)
	{
		if (mCurrentDir.string() == _path.string())
		{
			return	mCurrentDir;
		}
		path_type	old	= std::move(mCurrentDir);
		mCurrentDir	= std::move(_path);
		if (mOnSelect)
		{
			mOnSelect(mCurrentDir.string());
		}
		update_request();
		return	old;
	}


	//! Set item select callback.
	void set_select_callback(std::function<select_callback>&& _callback) noexcept
	{
		mOnSelect	= std::move(_callback);
	}

protected:
	void initialize()
	{
		mList	= new list_type();
		this->add_child(mList);
	}


	void update_request()
	{
		if (mUpdateRequest) { return; }
		this->schedule_once([this](auto...)
		{
			refresh();
		});
	}

	virtual void on_created() override
	{
		super_type::on_created();
		if (mCurrentDir.empty())
		{
			set_current_directory(application::get_application()->files_dir(true));
		}
	}


	virtual void on_size_change(typename super_type::size_type& _size) noexcept override
	{
		super_type::on_size_change(_size);
		mList->set_size(_size);
	}


private:
	void refresh()
	{
		AMTRS_TRACE_LOG("refresh filelist current dir is '%s'.", mCurrentDir.string().c_str());
		for (auto e : filesystem::directory_iterator(mCurrentDir))
		{
			AMTRS_TRACE_LOG(" entry : %s", e.path().string().c_str());
		}
	}

	file_selector_mode				mDialogMode;
	std::function<select_callback>	mOnSelect;
	path_type						mCurrentDir;
	bool							mUpdateRequest	= false;
	ref<list_type>					mList;
};


AMTRS_NAMESPACE_END
#endif
