/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libscheduler__application__hpp
#define	__libscheduler__application__hpp
#include <functional>
#include <amtrs/graphics/font.hpp>
#include <gframe/@>
#include "save/schedule.hpp"
#include "save/time.hpp"
#include "node.hpp"
#include "manager/purchase.hpp"

using	color_type		= amtrs::rgba<float>;
using	node_ref		= amtrs::ref<amtrs::node>;


// ********************************************************
//! 機能の有効化
// --------------------------------------------------------
struct	setting_configs
{
	enum	setting_types
	{
		st_bg_images				= 1,		//!< 背景画像を変更する
		st_bg_with_storages			= 2,		//!< 背景画像をストレージから選択する
		st_home_sound				= 3,		//!< ホーム画面タッチでサウンドを再生する
		st_schedule_show_seconds	= 4,		//!< リストページで秒を表示する
	};

	color_type		background_color;
};



// ********************************************************
//! ヘッダ
// --------------------------------------------------------
enum class	contents_header_action
{
	cha_close	= 1,			//!< コンテンツを閉じる
};


// ========================================================
//! ヘッダのベースクラス
// --------------------------------------------------------
class	contents_header
		: public amtrs::node
{
public:
	//! 機能の名称を渡します。
	virtual void set_name(std::string _name) = 0;

	//! ヘッダに表示するタイトルを表示します。
	virtual void set_title(std::string _title) = 0;
};


// ********************************************************
//! フォント
// --------------------------------------------------------
enum class	font_type
{
	ft_large		= 0,		//!< 大きなフォント（タイトルバーなど）
	ft_normal		= 1,		//!< 通常のフォント
	ft_small		= 2,		//!< 小さなフォント
	ft_button		= 3,		//!< ボタン用
	ft_big			= 4,		//!< さらに大きなフォント（スケジュール一覧などで使用）
	ft_license		= 5,		//!< ライセンス表示など

	__ft_max		= 6
};


// ********************************************************
//! サウンド
// --------------------------------------------------------
enum class	sound_types
{
	editor_open			= 1,
	editor_complite		= 2,
	setting_complite	= 3,
	menu_back			= 4,
};


//! 指定した設定を有効にするか問い合わせる。
bool use_setting(setting_configs::setting_types _types);



// ********************************************************
//! アプリケーションのコンフィグ
// --------------------------------------------------------
//! システムは起動時に
//!	void initialize_modules(application_modules*)
//! よ呼び出します。この中でアプリケーションがカスタマイズ
//! したい項目を設定してください。
// --------------------------------------------------------
struct	application_configs
{
	using	home_creator			= amtrs::ref<amtrs::node>(*)();
	using	sound_name				= std::string(*)(sound_types);
	using	font_creator			= amtrs::ref<amtrs::font>(*)(font_type);
	using	header_creator			= amtrs::ref<contents_header>(*)(std::function<void(contents_header_action)> _action);
	using	doalog_button_creator	= amtrs::ref<amtrs::button_node<amtrs::node>>(*)(int _button_id);


	std::string				alarm_package_name	= "";
	color_type				foreground_color	= {0,0,0,1};
	color_type				background_color	= {1,1,1,0.9f};
	home_creator			create_home			= nullptr;
	header_creator			create_header		= nullptr;
	font_creator			create_font			= nullptr;
	doalog_button_creator	create_button		= nullptr;
	sound_name				sound				= nullptr;

	//! 設定画面に関するコンフィグ
	setting_configs		settings;
};



void initialize_application_config(application_configs* _config);

application_configs const* get_application_config();





// ========================================================
//! コンテンツで使用するフォントを返します。
// --------------------------------------------------------
amtrs::ref<amtrs::font> get_font(font_type _type = font_type::ft_normal);


// ========================================================
//! ボタンを生成します。
// --------------------------------------------------------
amtrs::ref<amtrs::node> create_button(int _button_id, std::string _caption, std::function<void(int)> _callback);


// ========================================================
//! 汎用のボタン型メニューを作成します。
// --------------------------------------------------------
amtrs::ref<amtrs::node> create_bottom_menu(std::vector<std::string> const& _menus, int _backs_id, std::function<void(amtrs::node*, int)> _callback);



// ****************************************************************************
// スタッフ
// ****************************************************************************
struct	staff
{
	std::string	roll;
	std::string	name;
	std::string	url;
};
std::vector<staff> create_stafflist();




// ========================================================
//! グループの分割線
// --------------------------------------------------------
amtrs::ref<amtrs::node> create_setting_splitbar();


// ========================================================
//! グループのタイトル
// --------------------------------------------------------
amtrs::ref<amtrs::node> create_setting_group(std::string _title);




// ========================================================
//! サブ項目の選択
// --------------------------------------------------------
class	setting_sub_contents
		: public amtrs::button_node<amtrs::node>
{
public:
	using	super_type	= amtrs::button_node<amtrs::node>;

	using	super_type::super_type;

	virtual void set_string(std::string _title) = 0;
};
amtrs::ref<setting_sub_contents> create_setting_sub_contents(std::string _title, std::string _current);


// ========================================================
//! スライダー
// --------------------------------------------------------
amtrs::ref<amtrs::node> create_setting_slider(std::string _title);


class	setting_switch_node
		: public amtrs::node
{
public:
	virtual void set_flag(bool _on) = 0;
};

// ========================================================
//! スイッチ
// --------------------------------------------------------
amtrs::ref<setting_switch_node> create_setting_switch(std::string _title, std::string _help, bool _switch, std::function<void(bool)> _listener);


// ****************************************************************************
//
// 背景画像選択
//
// ****************************************************************************
enum	bgitem_mode : unsigned int
{
	bgitem_mode_locked			= 1<<0,			//!< ロックされている
	bgitem_mode_selected		= 1<<1,			//!< 選択されている
	bgitem_mode_storage			= 1<<2,			//!< ストレージからの選択
};
amtrs::ref<amtrs::node> create_bgitem_node(amtrs::ref<amtrs::g3d::texture> _texture, unsigned int _modes);




// ****************************************************************************
//
// サウンド選択
//
// ****************************************************************************
enum	sounditem_mode : unsigned int
{
	sounditem_mode_locked		= 1<<0,			//!< ロックされている
	sounditem_mode_selected		= 1<<1,			//!< 選択されている
};
amtrs::ref<amtrs::node> create_sounditem_node(std::string const& _soundtitle, unsigned int _modes);



class	weekitem_node
		: public amtrs::node
{
public:
	virtual void set_status(bool _enable) = 0;
};

amtrs::ref<weekitem_node> create_weekitem_node(int _week_id);



// ****************************************************************************
//
// スケジュール関連
//
// ****************************************************************************



// ========================================================
//! スケジュール
// --------------------------------------------------------
class	schedule_contents
		: public amtrs::button_node<amtrs::node>
{
public:
	using	super_type	= amtrs::button_node<amtrs::node>;
	using	value_type	= save::schedule;

	using	super_type::super_type;

	//! ステータスの更新を行う。
	virtual void refresh() {}

	virtual void update() = 0;

	virtual void update(value_type const& _data) = 0;

	virtual value_type* data() = 0;

	//! 背景色を返します。
	//! _alpha
	//!		設定で選択した背景色の透明度
	static amtrs::rgba<float> background_color(float _alpha);


	static amtrs::ref<schedule_contents> create(amtrs::ref<value_type> _schedule);

	static amtrs::ref<amtrs::node> create_new_schedule(std::function<void(save::schedule_type)> _trigger);
};



// ========================================================
//! スケジュールエディット
// --------------------------------------------------------
struct	editor_info
{
	enum types
	{
		title_edit	= 1,
		date_edit,
		time_edit,
		weeks_edit,
		sound_edit,
		loop_sound_edit,
		note_edit,
		notify_edit,
		wakeup_edit,
		saveto_edit,
	};

	struct	editor
	{
		using	update_listener	= std::function<void()>;
		types									type;
		std::string								label;
		amtrs::node*							node		= nullptr;
		std::function<void(update_listener)>	callback;

		operator bool () noexcept { return (bool)callback; }
		void operator () (update_listener _listener = [](){}) { callback(std::move(_listener)); }
	};
};
amtrs::ref<amtrs::node> create_editor(editor_info::editor _editor);


// ****************************************************************************
//
// 課金アイテム
//
// ****************************************************************************
namespace purchase
{
	using	purchase_item_listener	= std::function<unsigned int()>;

	struct	create_params
	{
		purchase_product				product;	//!< プロダクト情報
		amtrs::ref<amtrs::g3d::texture>	thumb;		//!< サムネイル
	};

	// 課金アイテム選択ノード
	amtrs::ref<amtrs::node> create_purchase_item(create_params _params, purchase_item_listener _listener);


	// 課金アイテム再ダウンロードノード
	amtrs::ref<amtrs::node> create_purchase_redownload(purchase_item_listener _listener);
}


namespace locale {

//! ロケールシステムを初期化します。
void initialize();

//! ロケールシステムにロケールファイルを追加します。
void add_locale_text(std::string _localefilename);

//! 対応する文字列を取得します。
char const* gettext(char const* _tag, char const* _default = nullptr);


}


#endif
