#ifndef	__libamtrs__graphics__font__hpp
#define	__libamtrs__graphics__font__hpp
#include "../string/@"
#include "def.hpp"
#include "color.hpp"
#include "bitmap.hpp"
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

	using	pixel_type		= rgba<uint8_t>;
	using	font_size_type	= int;

	struct	metrics
	{
		font_size_type		ascent;			//!< �x�[�X���C������̍���
		font_size_type		descent;		//!< �x�[�X���C����艺�̍���
	};

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
	static ref<font> create(const char* _fontname, font_size_type _default_size, pixel_type _default_color = {255,255,255,255});

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


inline font::font(const char* /*_fontname*/, font_size_type _default_size, pixel_type _default_color)
	: mUpdateDirty(true)
	, mFontColor(_default_color)
	, mFontSize(_default_size)
{
}

inline void font::set_color(pixel_type _font_color)
{
	mFontColor		= _font_color;
	mUpdateDirty	= true;
}


inline void font::set_edge(pixel_type _edge_color, int _edge_size)
{
	mEdgeColor		= _edge_color;
	mEdgeSize		= _edge_size;
	mUpdateDirty	= true;
}

inline void font::set_line_break_mode(line_break _lb)
{
	mLineBreak		= _lb;
	mUpdateDirty	= true;
}


inline auto font::create_bitmap(std::string_view _text, int _align_x, int _align_y) -> bitmap<pixel_type>
{
	bounds				b		= compute_bounds(_text);
	if (_align_x > 1)
	{
		b.area.width	+= b.area.width % _align_x;
	}
	if (_align_y > 1)
	{
		b.area.height	+= b.area.height % _align_y;
	}
	draw_info			di;
	di.color		= mFontColor;
	di.edgeColor	= mEdgeColor;
	di.edgeSize		= mEdgeSize;
	vec2<int>			drwpos((int)-b.area.x, (int)-b.area.y);
	size2<int>			bmpsize(b.area.width, b.area.height);
	if (mEdgeSize > 0)
	{
		drwpos.x		+= mEdgeSize;
		drwpos.y		+= mEdgeSize;
		bmpsize.width	+= mEdgeSize * 2;	// ��+�E
		bmpsize.height	+= mEdgeSize * 2;	// ��+��
	}
	bitmap<pixel_type> 	bmp(bmpsize);
	draw(bmp, drwpos, _text, di);
	return	bmp;
}


inline void font::draw_mb(bitmap<pixel_type>& _out, std::string_view _text, const draw_info& _info)
{
	struct	line_info
	{
		std::string_view	text;
		rect<int>			lineRect;			// ���̍s�����̂ɕK�v�ȋ�`
	};

	// �܂��͍s�𕪊�����
	auto					lines	= split_chars(_text, "\n");
	std::vector<line_info>	infos;
	for (auto line : lines)
	{
		line	= trim(line, "\r");

		auto*	cur		= line.data();
		auto*	end		= cur + line.size();
		while (cur < end)
		{
			auto	block	= std::string_view(cur, (std::size_t)(end - cur));
			auto	lb		= compute_braek(block, _out.size().width);
			if (lb.first == 0)
			{
				return;
			}
			block	= std::string_view(cur, lb.first);
			auto	bounds	= compute_bounds(block);
			infos.push_back({block, bounds.area});
			cur	+= lb.first;
		}
	}


	// �`����s��
	int					y	= 0;
	for (auto& info : infos)
	{
		on_draw(_out, {0, -info.lineRect.y + y}, nullptr, info.text, _info);
		y += info.lineRect.height;
	}
}


inline auto font::create_bitmap_adjust_size(vec2<int>* _origin, std::string_view _text, const draw_info& _info, size2<int> _size) -> bitmap<pixel_type>
{
	struct	line_info
	{
		std::string_view	text;
		rect<int>			lineRect;			// ���̍s�����̂ɕK�v�ȋ�`
	};

	if (_size.width <= 0 || _size.height <= 0)
	{
		return	bitmap<pixel_type>(size2<int>(0, 0));
	}

	// �܂��͍s�𕪊�����
	ref<font>				f		= this;
	auto					lines	= split_chars(_text, "\n");
	std::vector<line_info>	infos;
	rect<int>				canvasSize;
	for (;;)
	{
		infos	= {};
		for (auto line : lines)
		{
			line	= trim(line, "\r");
			auto*	cur		= line.data();
			auto*	end		= cur + line.size();
			while (cur < end)
			{
				auto	block	= std::string_view(cur, (std::size_t)(end - cur));
				auto	lb		= f->compute_braek(block, _size.width);
				if (lb.first == 0)
				{
					return	{};
				}
				block	= std::string_view(cur, lb.first);
				auto	bounds	= f->compute_bounds(block);
				infos.push_back({block, bounds.area});
				cur	+= lb.first;
			}
		}

		// �L�����o�X�̑傫�����v�Z����
		canvasSize			= infos[0].lineRect;
		canvasSize.y		= 0;
		canvasSize.height	= 0;
		for (auto& info : infos)
		{
			int		right	= canvasSize.right();
			if (canvasSize.x > info.lineRect.x)
			{
				canvasSize.x		= info.lineRect.x;
				canvasSize.width	= right - canvasSize.x;
				right				= canvasSize.right();
			}
			
			if (right < info.lineRect.right())
			{
				canvasSize.width	= info.lineRect.right() - canvasSize.x;
			}
			canvasSize.height	+= info.lineRect.height;
		}

		float	scale	= std::min<float>((float)_size.width / (float)canvasSize.width, (float)_size.height / (float)canvasSize.height);
		if (scale >= 1.0f)
		{
			break;
		}
		f		= font::create((int)((float)f->size() * scale), f->color());
	}

	// �`����s��
	bitmap<pixel_type>	retval(size2<int>(canvasSize.width, canvasSize.height));
	int					y	= 0;
	for (auto& info : infos)
	{
		f->on_draw(retval, {-canvasSize.x, -info.lineRect.y + y}, nullptr, info.text, _info);
		y += info.lineRect.height;
	}
	if (_origin)
	{
		_origin->x	= -canvasSize.x;
		_origin->y	= -infos[0].lineRect.y;
	}
	return	retval;
}


inline auto font::create_bitmap_mb(vec2<int>* _origin, std::string_view _text, const draw_info& _info, int _maxWidth) -> bitmap<pixel_type>
{
	struct	line_info
	{
		std::string_view	text;
		rect<int>			lineRect;			// ���̍s�����̂ɕK�v�ȋ�`
	};

	// �܂��͍s�𕪊�����
	auto					lines	= split_chars(_text, "\n");
	std::vector<line_info>	infos;
	for (auto line : lines)
	{
		line	= trim(line, "\r");

		auto*	cur		= line.data();
		auto*	end		= cur + line.size();
		while (cur < end)
		{
			auto	block	= std::string_view(cur, (std::size_t)(end - cur));
			auto	lb		= compute_braek(block, _maxWidth);
			if (lb.first == 0)
			{
				return	{};
			}
			block	= std::string_view(cur, lb.first);
			auto	bounds	= compute_bounds(block);
			infos.push_back({block, bounds.area});
			cur	+= lb.first;
		}
	}

	// �L�����o�X�̑傫�����v�Z����
	rect<int>				canvasSize	= infos[0].lineRect;
	canvasSize.y		= 0;
	canvasSize.height	= 0;
	for (auto& info : infos)
	{
		int		right	= canvasSize.right();
		if (canvasSize.x > info.lineRect.x)
		{
			canvasSize.x		= info.lineRect.x;
			canvasSize.width	= right - canvasSize.x;
			right				= canvasSize.right();
		}
		
		if (right < info.lineRect.right())
		{
			canvasSize.width	= info.lineRect.right() - canvasSize.x;
		}
		canvasSize.height	+= info.lineRect.height;
	}

	// �`����s��
	bitmap<pixel_type>	retval(size2<int>(canvasSize.width, canvasSize.height));
	int					y	= 0;
	for (auto& info : infos)
	{
		on_draw(retval, {-canvasSize.x, -info.lineRect.y + y}, nullptr, info.text, _info);
		y += info.lineRect.height;
	}
	if (_origin)
	{
		_origin->x	= -canvasSize.x;
		_origin->y	= -infos[0].lineRect.y;
	}
	return	retval;
}



AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(font)
#endif
