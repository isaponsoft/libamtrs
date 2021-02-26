/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__syntax__hpp
#define	__libamtrs__passion__syntax__hpp
#include "lexar.hpp"
#include "string.hpp"
#include "token.hpp"
#include "vmtypelist.hpp"
PASSION_NAMESPACE_BEGIN

struct	stntaxparser;
class	typenamepath_node;


class	syntax_node
		: public amtrs::ref_object
{
public:
	enum	phase_type
	{
		start	= 0,		//!< フェーズの開始
		end		= 1,		//!< 処理を終了した
		err		= 2,		//!< エラー
		proc	= 3,		//!< 処理を継続中
	};

	struct	result
	{
		int						phase;
		amtrs::ref<syntax_node>	next;
	};


	virtual result parse(int _phase, stntaxparser& _parser) = 0;

private:
	int				mPhase	= start;
	syntax_node*	mParent	= nullptr;

	friend	stntaxparser;
};


struct	stntaxparser
{
	int						phase;
	lexar*					lex;
	amtrs::ref<syntax_node>	root;
	amtrs::ref<syntax_node>	current;
	amtrs::ref<syntax_node>	prev;

	vmtypelist				typeList;

	void set_root(amtrs::ref<syntax_node> _node)
	{
		phase	= syntax_node::start;
		root	= _node;
		current	= _node;
	}

	void parse()
	{
		for (;;)
		{
			auto	r	= current->parse(current->mPhase, *this);
			current->mPhase	= r.phase;
			if (current->mPhase == syntax_node::err)
			{
				break;
			}
			if (current->mPhase == syntax_node::end)
			{
				prev	= current;
				current	= current->mParent;
			}
			else if (!r.next.empty() && r.next != current)
			{
				r.next->mParent	= current;
				prev	= current;
				current	= r.next;
			}
		}
	}
};



// ******************************************************************
//! ルートノード
// ------------------------------------------------------------------
class	declaration_node
		: public syntax_node
{
public:
	using	super_type	= syntax_node;

	virtual result parse(int _phase, stntaxparser& _parser) override;


private:
	std::list<amtrs::ref<syntax_node>>	mChilds;
};



// ******************************************************************
//! 名前空間
// ------------------------------------------------------------------
//! syntax)
//!		namespace NAME { Declaration-node }
//!		namespace { Declaration-node }
// ------------------------------------------------------------------
class	namespace_block_node
		: public syntax_node
{
public:
	using	super_type	= syntax_node;

	virtual result parse(int _phase, stntaxparser& _parser) override;

private:
	amtrs::ref<typenamepath_node>		mTypename;
	std::list<amtrs::ref<syntax_node>>	mChilds;
};


// ******************************************************************
//! struct
// ------------------------------------------------------------------
class	struct_node
		: public syntax_node
{
public:
	using	super_type	= syntax_node;


	struct	base_attribute_rules
	{
	};


	virtual result parse(int _phase, stntaxparser& _parser) override;

private:
	vmtypedata							mTypedata;
	std::list<amtrs::ref<syntax_node>>	mChilds;
};


// ******************************************************************
//! 型名のパス
// ------------------------------------------------------------------
class	typenamepath_node
		: public syntax_node
{
public:
	using	super_type	= syntax_node;

	enum	phase_type
	{
		separator	= 100,
	};

	vmstring const& name() const noexcept
	{
		return	mName;
	}

	virtual result parse(int _phase, stntaxparser& _parser) override;


private:
	vmstring	mName;
};


PASSION_NAMESPACE_END
#endif
