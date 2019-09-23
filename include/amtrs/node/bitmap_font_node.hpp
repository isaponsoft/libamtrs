/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__node__bitmap_font_node__hpp
#define	__libamtrs__node__bitmap_font_node__hpp
#include <vector>
#include "node.hpp"
#include "batch_sprite_node.hpp"
#include "sprite_node.hpp"
AMTRS_NAMESPACE_BEGIN

#if	0
template<class NodeT>
class	bitmap_font_node
		: public batch_sprite_node<NodeT>
{
	using	base_type	= batch_sprite_node<NodeT>;
	using	map_type	= std::map<int, int>;
	using	value_type	= typename map_type::value_type;
public:
	bitmap_font_node(size2<unsigned int> _divCount, g3d::texture* _texture)
		: base_type(_divCount, _texture)
	{}


	void set_charmap(std::initializer_list<value_type> _chars)
	{
		mCharMap.clear();
		mCharMap	= map_type(_chars.begin(), _chars.end());
	}

	void set_string(std::string_view _string)
	{
		base_type::mChars.clear();
		auto	filter	= make_bucket(std::begin(_string), std::end(_string), [this](auto c) -> int
		{
			return	mCharMap[c];
		});
		base_type::mChars.assign(filter.begin(), filter.end());
		base_type::build_vertex();
	}


protected:
	map_type	mCharMap;
};
#endif
AMTRS_NAMESPACE_END
#endif
