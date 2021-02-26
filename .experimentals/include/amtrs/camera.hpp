/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__camera__hpp
#define	__libamtrs__camera__hpp
#include <functional>
#include "graphics.hpp"
AMTRS_NAMESPACE_BEGIN

class	camera
		: public ref_object
{
public:
	enum class	facing_types
	{
		external	= 0,		//!< 外部カメラ
		front		= 1,		//!< インカメラ
		back		= 2,		//!< リアカメラ
	};


	enum class	format_types
	{
		unkown		= -1,
		bitmap		= 1,		//!< Bitmap
		yuv_420_888	= 2,		//!< YUV 420
		jpeg		= 3,		//!< jpeg compression
	};


	struct	image_info
	{
		format_types	type;
		color_format	color;
		size2<int>		size;
	};


	struct	planedata
			: imageview<uint8_t const>
	{
		size_t	length;
	};


	struct	info_data
	{
		std::string					name;
		int							number;
		facing_types				facing;
		size2<int>					size;
		shared_buffer<image_info>	streams;		//!< streaming image formats.(preview and movie)
	};

	using	image_type	= planedata;

	struct	format
	{
		format_types	type;
		color_format	color;
		size2<int>		size;
	};


	// ========================================================================
	//! カメラの許可を得ます。
	// ------------------------------------------------------------------------
	//! カメラ系の機能を使う場合、最初にpermissionを呼び出し、必ずOSからカメラ
	//! 呼び出し権限を受け取る必要があります。さもなければカメラ系の呼び出しは
	//! 失敗します。
	// ------------------------------------------------------------------------
	static void permission(std::function<void(bool)> _callback);


	// ========================================================================
	//! カメラを列挙します。
	// ------------------------------------------------------------------------
	static bool enumlate(std::function<bool(ref<camera>)> _callback);


	// ========================================================================
	//! 指定したフォーマットに最も近いカメラのフォーマットを返します。
	// ------------------------------------------------------------------------
	static std::pair<ref<camera>, format> choise(int _min, int _max, facing_types _facing = facing_types::back, format_types _format = format_types::yuv_420_888);


	// ========================================================================
	//! サポートするフォーマットを列挙します。
	// ------------------------------------------------------------------------
	virtual bool formats(std::function<void(format)> _callback) = 0;


	// ========================================================================
	//! 動画のキャプチャを開始します。
	// ------------------------------------------------------------------------
	//! コールバックはなるべく短い時間で終了してください。
	//! 画像処理などを行う場合はスレッド化するとよいでしょう。
	// ------------------------------------------------------------------------
	virtual bool start(format _format, std::function<void(image_type const*, size_t)> _capture) = 0;

	//! pause() しているキャプチャを再開します。
	virtual bool restart() = 0;

	//! 一時停止します。restart() で再開できます。
	virtual bool pause() = 0;

	// ========================================================================
	//! カメラデバイスの使用を停止します。
	//! -----------------------------------------------------------------------
	//! デバイスは破棄されます。
	//! -----------------------------------------------------------------------
	//! return
	//!		成功した場合は true を返します。停止済みのデバイスを停止させても
	//!		true を返します。
	//! -----------------------------------------------------------------------
	virtual bool stop() = 0;

	static ref<camera> create(std::string _cameraId);
};



AMTRS_NAMESPACE_END
#endif
