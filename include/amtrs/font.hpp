/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__font__hpp
#define	__libamtrs__font__hpp
#include "amtrs.hpp"
#include "graphics.hpp"
AMTRS_NAMESPACE_BEGIN

//! このクラスはプラットフォームの機能を利用します。
class	font
		: public ref_object
{
public:
	enum class	line_break
	{
		clipping,				//!< （デフォルト）特に何もせず切り捨てます。
		charcter,				//!< 文字で改行する。
		truncating,				//!< 末尾を "..." にします。
	};

	enum class	family_type
	{
		normal		= 0,		//!< デフォルト／通常
		monospace	= 1,		//!< 等幅
		serif		= 2,		//!< 明朝体
		sans_serif	= 3,		//!< ゴシック
	};

	static constexpr family_type	normal		= family_type::normal;
	static constexpr family_type	monospace	= family_type::monospace;
	static constexpr family_type	serif		= family_type::serif;
	static constexpr family_type	sans_serif	= family_type::sans_serif;



	using	pixel_type		= rgba<uint8_t>;
	using	font_size_type	= int;

	struct	metrics
	{
		font_size_type		ascent;			//!< ベースラインより上の高さ
		font_size_type		descent;		//!< ベースラインより下の高さ
	};


	// ========================================================================
	//! フォントファミリを指定してシステムデフォルトのフォントを作成します。
	// ------------------------------------------------------------------------
	//!
	// ------------------------------------------------------------------------
	static ref<font> create(family_type _family, font_size_type _default_size = -1, pixel_type _default_color = {255,255,255,255});


	//! システムに設定されているフォントの大きさを取得します。
	static font_size_type system_font_size();

	// ========================================================================
	//! システムデフォルトのフォントを作成します。
	// ------------------------------------------------------------------------
	//! フォント名やフォントの大きさもシステムデフォルトになります。
	// ------------------------------------------------------------------------
	static ref<font> create(pixel_type _default_color = {255,255,255,255});

	// ========================================================================
	//! システムデフォルトのフォントを作成します。
	// ------------------------------------------------------------------------
	//!	_default_size
	//! 	フォントの大きさ。0以下の場合はシステムデフォルトサイズになります。
	// ------------------------------------------------------------------------
	static ref<font> create(font_size_type _default_size, pixel_type _default_color = {255,255,255,255});

	//! _fontname	使用するフォントの名前。nullptr の場合はシステムのデフォルトを使用する。
	static ref<font> create(const char* _fontname, font_size_type _default_size = -1, pixel_type _default_color = {255,255,255,255});

	font_size_type size() const noexcept { return mFontSize; }
	pixel_type color() const noexcept { return mFontColor; }
	line_break line_break_mode() const noexcept { return mLineBreak; }

	void set_color(pixel_type _font_color);
	void set_line_break_mode(line_break _lb);
	void set_edge(pixel_type _edge_color, int _edge_size);

	// ========================================================================
	//! フォント描画を行うためのオブジェクト
	// ------------------------------------------------------------------------
	struct	draw_info
	{
		pixel_type	color;				//!< 描画色
		int			edgeSize	= 0;	//!< 枠線の大きさ(0以下の場合は枠線を描画しない)
		pixel_type	edgeColor;			//!< 枠線の色
	};


	// ========================================================================
	//! compute_bounds() が返す構造体。
	// ------------------------------------------------------------------------
	struct	bounds
	{
		rect<int>	area;			//!< 描画エリアの座標と大きさ
		int			advance;		//!< 文字送り幅(次に描画を開始すべきx座標に近しい)
	};

	// ========================================================================
	//! 指定する文字列を描画ビットマップへ描画します。
	// ------------------------------------------------------------------------
	//! _out
	//!		描画対象のビットマップ。
	//!		画像はクリアされます。
	//!
	//! _drew, _drewSize
	//!		描画した文字の描画位置を格納するバッファ。nullptrを渡した場合や
	//!		サイズが不足した場合はそれ以上は書き込みません。
	//
	//! _origin
	//!		描画を開始する原点位置。縦方向ではベースラインと一致します。
	// ------------------------------------------------------------------------
	void draw(bitmap<pixel_type>& _out, vec2<int> _origin, std::string_view _text, const draw_info& _info)
	{ on_draw(_out, _origin, nullptr, _text, _info); }


	// ========================================================================
	//! 指定する文字列を描画ビットマップへ描画します。
	// ------------------------------------------------------------------------
	//! _out
	//!		描画対象のビットマップ。
	//!		画像はクリアされます。
	//!
	//! _drew, _drewSize
	//!		描画した文字の描画位置を格納するバッファ。nullptrを渡した場合や
	//!		サイズが不足した場合はそれ以上は書き込みません。
	//
	//! _origin
	//!		描画を開始する原点位置。縦方向ではベースラインと一致します。
	// ------------------------------------------------------------------------
	void draw(bitmap<pixel_type>& _out, rect<int> _origin, std::string_view _text, const draw_info& _info)
	{ on_draw(_out, _origin, &_origin, _text, _info); }


	void draw_mb(bitmap<pixel_type>& _out, std::string_view _text, const draw_info& _info);



	// ========================================================================
	//! フォントの情報を取得します。
	// ------------------------------------------------------------------------
	virtual metrics get_metrics() = 0;


	// ========================================================================
	//! 文字列をレンダリングしたときの画像の大きさを計算します。
	// ------------------------------------------------------------------------
	//! 描画座標が(0,0)としたときに、実際に描画される画像の矩形サイズを計算します。
	// ------------------------------------------------------------------------
	virtual bounds compute_bounds(std::string_view _text) = 0;


	// ========================================================================
	//! 文字列をレンダリングしたときに何文字目まで描画できるかを調べます。
	// ------------------------------------------------------------------------
	//! return
	//!		.first	= 描画可能な文字のサイズ。
	//!		.second	= 描画した時の幅
	// ------------------------------------------------------------------------
	virtual auto compute_braek(std::string_view _text, int _maxWidth) -> std::pair<typename std::string_view::size_type, int> = 0;


	// ========================================================================
	//! 文字列がピッタリ収まるようなビットマップを生成し、描画を行います。
	// ------------------------------------------------------------------------
	//! 色や枠線の有無は set_color() などで指定された値を使用します。
	// ------------------------------------------------------------------------
	bitmap<pixel_type> create_bitmap(std::string_view _text, int _align_x = 2, int _align_y = 2);


	// ========================================================================
	//! 指定した横幅に収まるように文字列を自動折り返しをして描画します。
	// ------------------------------------------------------------------------
	//! 改行コードが含まれる場合は改行コードも折り返します。
	//! 文字列がピッタリ収まったビットマップを返します。
	//
	//!	_origin
	//!		nullptr 以外をセットした場合、描画の際に _origin に指定した値を
	//!		返します。
	// ------------------------------------------------------------------------
	bitmap<pixel_type> create_bitmap_mb(vec2<int>* _origin, std::string_view _text, const draw_info& _info, int _maxWidth);


	bitmap<pixel_type> create_bitmap_mb(std::string_view _text, const draw_info& _info, int _maxWidth) { return create_bitmap_mb(nullptr,  _text, _info, _maxWidth);  }


	// ========================================================================
	//! 指定したサイズに収まるようにフォントサイズを調整します。
	// ------------------------------------------------------------------------
	bitmap<pixel_type> create_bitmap_adjust_size(vec2<int>* _origin, std::string_view _text, const draw_info& _info, size2<int> _size);


	const draw_info& default_draw_info() noexcept
	{
		mDrawInfo.color		= mFontColor;
		mDrawInfo.edgeColor	= mEdgeColor;
		mDrawInfo.edgeSize	= mEdgeSize;
		return	mDrawInfo;
	}

protected:
	virtual void on_draw(bitmap<pixel_type>& _out, vec2<int> _origin, size2<int>* _clip, std::string_view _text, const draw_info& _info) = 0;


	font(const char* _fontname, font_size_type _default_size, pixel_type _default_color);
	font(const font&) = delete;
	font(font&&) = delete;
	font& operator = (const font&) = delete;
	font& operator = (font&&) = delete;

	draw_info		mDrawInfo;
	bool			mUpdateDirty;
	pixel_type		mFontColor		= {255, 255, 255, 255};
	pixel_type		mEdgeColor		= {0, 0, 0, 255};
	int				mEdgeSize		= 0;
	font_size_type	mFontSize;
	line_break		mLineBreak		= line_break::clipping;
};




AMTRS_NAMESPACE_END
#endif
