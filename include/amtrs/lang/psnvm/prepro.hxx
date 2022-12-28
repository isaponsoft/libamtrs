/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__prepro__hpp
#define	__libamtrs__lang__passion__prepro__hpp
PSNVM_LANG_NAMESPACE_BEGIN

struct	declare
{
	sstring	filename;
	int		line;
	sstring	code;
};


struct	group
{
	sstring					identify;
	simplearray<declare>	decls;	
};


struct	prepro
{
	// 削除した行を置き換えた後の文字列(デフォルトは "\n")
	text						removed_line;
	text						removed_line_begin;
	text						removed_line_end;
	bool						line_directive	= true;
	simplearray<group>			defines;
	simplearray<sstring>		paths;
	simplearray<errorinfo>		errors;

	prepro(text _removed_line = "\n")
		: removed_line(_removed_line)
	{}

	group* find_macro_group(text _name)
	{
		for (auto& g : defines)
		{
			if (g.identify == _name)
			{
				return	&g;
			}
		}
		return	nullptr;
	}

	sstring process(text _code, text _filename);
};



PSNVM_LANG_NAMESPACE_END
#endif
