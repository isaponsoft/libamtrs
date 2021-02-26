/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__line_segment__hpp
#define	__libamtrs__graphics__line_segment__hpp
AMTRS_NAMESPACE_BEGIN

template<class VecT>
struct	basic_line_segment;


using	line_segment	= basic_line_segment<float>;


//! 線分（２点を持つ有限の直線）
template<class VecT>
struct	basic_line_segment
{
	using	vector_type	= VecT;

	union
	{
		vector_type	org;		//!< 開始位置
		vector_type	p1;			//!< 開始位置
	};
	union
	{
		vector_type	end;		//!< 終了位置
		vector_type	p2;			//!< 終了位置
	};

	basic_line_segment() = default;
	basic_line_segment(basic_line_segment const&) = default;
	basic_line_segment& operator = (basic_line_segment const&) = default;

	basic_line_segment(vector_type o, vector_type e)
		: org(o)
		, end(e)
	{}
};



AMTRS_NAMESPACE_END
#endif
