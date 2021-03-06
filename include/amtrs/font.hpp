﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__font__hpp
#define	__libamtrs__font__hpp
#include "amtrs.hpp"
#include "graphics.hpp"
AMTRS_NAMESPACE_BEGIN

//! ���̃N���X�̓v���b�g�t�H�[���̋@�\�𗘗p���܂��B
class	font
		: public ref_object
{
public:
	enum class	line_break
	{
		clipping,				//!< �i�f�t�H���g�j���ɉ��������؂�̂Ă܂��B
		charcter,				//!< �����ŉ��s����B
		truncating,				//!< ������ "..." �ɂ��܂��B
	};

	enum class	family_type
	{
		normal		= 0,		//!< �f�t�H���g�^�ʏ�
		monospace	= 1,		//!< ����
		serif		= 2,		//!< ������
		sans_serif	= 3,		//!< �S�V�b�N
	};

	static constexpr family_type	normal		= family_type::normal;
	static constexpr family_type	monospace	= family_type::monospace;
	static constexpr family_type	serif		= family_type::serif;
	static constexpr family_type	sans_serif	= family_type::sans_serif;



	using	pixel_type		= rgba<uint8_t>;
	using	font_size_type	= int;

	struct	metrics
	{
		font_size_type		ascent;			//!< �x�[�X���C������̍���
		font_size_type		descent;		//!< �x�[�X���C����艺�̍���
	};


	// ========================================================================
	//! �t�H���g�t�@�~�����w�肵�ăV�X�e���f�t�H���g�̃t�H���g���쐬���܂��B
	// ------------------------------------------------------------------------
	//!
	// ------------------------------------------------------------------------
	static ref<font> create(family_type _family, font_size_type _default_size = -1, pixel_type _default_color = {255,255,255,255});


	//! �V�X�e���ɐݒ肳��Ă���t�H���g�̑傫�����擾���܂��B
	static font_size_type system_font_size();

	// ========================================================================
	//! �V�X�e���f�t�H���g�̃t�H���g���쐬���܂��B
	// ------------------------------------------------------------------------
	//! �t�H���g����t�H���g�̑傫�����V�X�e���f�t�H���g�ɂȂ�܂��B
	// ------------------------------------------------------------------------
	static ref<font> create(pixel_type _default_color = {255,255,255,255});

	// ========================================================================
	//! �V�X�e���f�t�H���g�̃t�H���g���쐬���܂��B
	// ------------------------------------------------------------------------
	//!	_default_size
	//! 	�t�H���g�̑傫���B0�ȉ��̏ꍇ�̓V�X�e���f�t�H���g�T�C�Y�ɂȂ�܂��B
	// ------------------------------------------------------------------------
	static ref<font> create(font_size_type _default_size, pixel_type _default_color = {255,255,255,255});

	//! _fontname	�g�p����t�H���g�̖��O�Bnullptr �̏ꍇ�̓V�X�e���̃f�t�H���g���g�p����B
	static ref<font> create(const char* _fontname, font_size_type _default_size = -1, pixel_type _default_color = {255,255,255,255});

	font_size_type size() const noexcept { return mFontSize; }
	pixel_type color() const noexcept { return mFontColor; }
	line_break line_break_mode() const noexcept { return mLineBreak; }

	void set_color(pixel_type _font_color);
	void set_line_break_mode(line_break _lb);
	void set_edge(pixel_type _edge_color, int _edge_size);

	// ========================================================================
	//! �t�H���g�`����s�����߂̃I�u�W�F�N�g
	// ------------------------------------------------------------------------
	struct	draw_info
	{
		pixel_type	color;				//!< �`��F
		int			edgeSize	= 0;	//!< �g���̑傫��(0�ȉ��̏ꍇ�͘g����`�悵�Ȃ�)
		pixel_type	edgeColor;			//!< �g���̐F
	};


	// ========================================================================
	//! compute_bounds() ���Ԃ��\���́B
	// ------------------------------------------------------------------------
	struct	bounds
	{
		rect<int>	area;			//!< �`��G���A�̍��W�Ƒ傫��
		int			advance;		//!< �������蕝(���ɕ`����J�n���ׂ�x���W�ɋ߂���)
	};

	// ========================================================================
	//! �w�肷�镶�����`��r�b�g�}�b�v�֕`�悵�܂��B
	// ------------------------------------------------------------------------
	//! _out
	//!		�`��Ώۂ̃r�b�g�}�b�v�B
	//!		�摜�̓N���A����܂��B
	//!
	//! _drew, _drewSize
	//!		�`�悵�������̕`��ʒu���i�[����o�b�t�@�Bnullptr��n�����ꍇ��
	//!		�T�C�Y���s�������ꍇ�͂���ȏ�͏������݂܂���B
	//
	//! _origin
	//!		�`����J�n���錴�_�ʒu�B�c�����ł̓x�[�X���C���ƈ�v���܂��B
	// ------------------------------------------------------------------------
	void draw(bitmap<pixel_type>& _out, vec2<int> _origin, std::string_view _text, const draw_info& _info)
	{ on_draw(_out, _origin, nullptr, _text, _info); }


	// ========================================================================
	//! �w�肷�镶�����`��r�b�g�}�b�v�֕`�悵�܂��B
	// ------------------------------------------------------------------------
	//! _out
	//!		�`��Ώۂ̃r�b�g�}�b�v�B
	//!		�摜�̓N���A����܂��B
	//!
	//! _drew, _drewSize
	//!		�`�悵�������̕`��ʒu���i�[����o�b�t�@�Bnullptr��n�����ꍇ��
	//!		�T�C�Y���s�������ꍇ�͂���ȏ�͏������݂܂���B
	//
	//! _origin
	//!		�`����J�n���錴�_�ʒu�B�c�����ł̓x�[�X���C���ƈ�v���܂��B
	// ------------------------------------------------------------------------
	void draw(bitmap<pixel_type>& _out, rect<int> _origin, std::string_view _text, const draw_info& _info)
	{ on_draw(_out, _origin, &_origin, _text, _info); }


	void draw_mb(bitmap<pixel_type>& _out, std::string_view _text, const draw_info& _info);



	// ========================================================================
	//! �t�H���g�̏����擾���܂��B
	// ------------------------------------------------------------------------
	virtual metrics get_metrics() = 0;


	// ========================================================================
	//! ������������_�����O�����Ƃ��̉摜�̑傫�����v�Z���܂��B
	// ------------------------------------------------------------------------
	//! �`����W��(0,0)�Ƃ����Ƃ��ɁA���ۂɕ`�悳���摜�̋�`�T�C�Y���v�Z���܂��B
	// ------------------------------------------------------------------------
	virtual bounds compute_bounds(std::string_view _text) = 0;


	// ========================================================================
	//! ������������_�����O�����Ƃ��ɉ������ڂ܂ŕ`��ł��邩�𒲂ׂ܂��B
	// ------------------------------------------------------------------------
	//! return
	//!		.first	= �`��\�ȕ����̃T�C�Y�B
	//!		.second	= �`�悵�����̕�
	// ------------------------------------------------------------------------
	virtual auto compute_braek(std::string_view _text, int _maxWidth) -> std::pair<typename std::string_view::size_type, int> = 0;


	// ========================================================================
	//! �����񂪃s�b�^�����܂�悤�ȃr�b�g�}�b�v�𐶐����A�`����s���܂��B
	// ------------------------------------------------------------------------
	//! �F��g���̗L���� set_color() �ȂǂŎw�肳�ꂽ�l���g�p���܂��B
	// ------------------------------------------------------------------------
	bitmap<pixel_type> create_bitmap(std::string_view _text, int _align_x = 2, int _align_y = 2);


	// ========================================================================
	//! �w�肵�������Ɏ��܂�悤�ɕ�����������܂�Ԃ������ĕ`�悵�܂��B
	// ------------------------------------------------------------------------
	//! ���s�R�[�h���܂܂��ꍇ�͉��s�R�[�h���܂�Ԃ��܂��B
	//! �����񂪃s�b�^�����܂����r�b�g�}�b�v��Ԃ��܂��B
	//
	//!	_origin
	//!		nullptr �ȊO���Z�b�g�����ꍇ�A�`��̍ۂ� _origin �Ɏw�肵���l��
	//!		�Ԃ��܂��B
	// ------------------------------------------------------------------------
	bitmap<pixel_type> create_bitmap_mb(vec2<int>* _origin, std::string_view _text, const draw_info& _info, int _maxWidth);


	bitmap<pixel_type> create_bitmap_mb(std::string_view _text, const draw_info& _info, int _maxWidth) { return create_bitmap_mb(nullptr,  _text, _info, _maxWidth);  }


	// ========================================================================
	//! �w�肵���T�C�Y�Ɏ��܂�悤�Ƀt�H���g�T�C�Y�𒲐����܂��B
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
