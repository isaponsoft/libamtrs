/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__vmmemprymap__hxx
#define	__libamtrs__lang__passion__vmmemprymap__hxx
#include <amtrs/system/log.hpp>
#include "pvlnode.hxx"
PSNVM_LANG_NAMESPACE_BEGIN


// 使用中のメモリの整合性を調べるためのもの
class	vmmemorymap
{
public:
	static constexpr size_t	child_size	= 4;

	struct	range
	{
		void*	begin;
		void*	end;
		bool empty() const noexcept { return begin == nullptr; }
	};

	struct	value_type
	{
		void*		begin;
		void*		end;
		value_type*	s;
		value_type*	l;
	};


	// 指定するメモリが範囲に含まれるか調べる
	range is_bounds(range const& _m)
	{
		auto	n	= mRoot;
		while (n)
		{
			if (_m.begin >= n->begin && _m.end <= n->end)
			{
				return	{n->begin, n->end};
			}
			if (_m.begin <= n->begin)
			{
				n	= n->s;
				continue;
			}
			else
			{
				n	= n->l;
				continue;
			}
		}
		return	{ nullptr, nullptr };
	}
	


	// 指定するメモリを登録する
	void add(range _m)
	{
		value_type*	node	= new value_type();
		node->begin	= _m.begin;
		node->end	= _m.end;
		node->s		= nullptr;
		node->l		= nullptr;
		if (!mRoot)
		{
			mRoot	= node;
			return;
		}

		value_type*	n	= mRoot;
		while (n)
		{
			if (_m.begin <= n->begin)
			{
				if (n->s)
				{
					n	= n->s;
					continue;
				}
				n->s	= node;
				return;
			}
			else
			{
				if (n->l)
				{
					n	= n->l;
					continue;
				}
				n->l	= node;
				return;
			}
		}
	}
	
	// 指定するメモリが解放されたことを通知する
	range release(void* _m)
	{
		value_type**	n	= &mRoot;
		while (*n)
		{
			if (_m >= (*n)->begin && _m <= (*n)->end)
			{
				value_type*	target	= (*n);
				if ((*n)->s)
				{
					value_type*	tail	= (*n)->s;
					if (tail->l)
					{
						do
						{
							tail	= tail->l;
						} while (tail->l);
						tail->l	= target->l;
					}
					(*n)	= (*n)->s;
				}
				else if ((*n)->l)
				{
					value_type*	tail	= (*n)->l;
					if (tail->s)
					{
						do
						{
							tail	= tail->s;
						} while (tail->s);
						tail->s	= target->s;
					}
					(*n)	= (*n)->l;
				}
				else
				{
					(*n)	= nullptr;
				}
				range	r{target->begin, target->end};
				delete	target;
				return	r;
			}
			if (_m <= (*n)->begin)
			{
				n	= &(*n)->s;
				continue;
			}
			else
			{
				n	= &(*n)->l;
				continue;
			}
		}
		return	{nullptr, nullptr};
	}

protected:
	value_type*	mRoot	= nullptr;
};


PSNVM_LANG_NAMESPACE_END
#endif
