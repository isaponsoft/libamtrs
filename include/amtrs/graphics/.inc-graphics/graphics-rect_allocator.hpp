/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__rect_allocator__hpp
#define	__libamtrs__graphics__rect_allocator__hpp
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! ２次元矩形の領域管理を行います。
// ----------------------------------------------------------------------------
//! コンストラクタで指定した２次元サイズから、allocate() で指定する２次元領域を
//! 確保したり、開放したりします。
//!	
//!	rect_allocator<int>	my_image_area({100, 100});
//!	rect<int>			area1 = my_image_area.allocate(50, 100);
//!	rect<int>			area2 = my_image_area.allocate(50, 100);
//!	my_image_area.deallocate(area1);
//!	my_image_area.deallocate(area2);
// ----------------------------------------------------------------------------
template<class T>
class	rect_allocator
{
public:
	using	value_type		= T;
	using	rect_type		= rect<value_type>;
	using	position_type	= typename rect_type::position_type;
	using	size_type		= typename rect_type::size_type;

	//! 指定したサイズの矩形領域を管理します。
	constexpr rect_allocator(const size_type& _size)
		: mRoot(status::nouse, {{0, 0}, _size})
	{}

	//! 指定した位置とサイズの矩形領域を管理します。
	constexpr rect_allocator(const rect_type& _rect)
		: mRoot(status::nouse, {_rect})
	{}

	rect_allocator() = default;
	rect_allocator(const rect_allocator& _r) = default;
	rect_allocator(rect_allocator&& _r) = default;
	rect_allocator& operator = (const rect_allocator& _r) = default;
	rect_allocator& operator = (rect_allocator&& _r) = default;
	~rect_allocator() = default;

	//! 確保に失敗した場合は empty() を返します。
	rect_type allocate(const size_type& _size)
	{
		auto	it	= mRoot.allocate(_size, std::true_type{});
		return	(it)
				? it->block
				: rect_type(0, 0, 0, 0);
	}

	//! 存在しない領域の場合は false を返し、何もしません。
	bool deallocate(const rect_type& _r)
	{
		return	mRoot.deallocate(_r);
	}

	//! 確保可能か問い合わせます。
	bool fetch(const size_type& _size) const noexcept
	{
		return	const_cast<rect_allocator*>(this)->mRoot.allocate(_size, std::false_type{})
				? true
				: false;
	}

	//! 管理領域全体の大きさを返します。
	constexpr size_type size() const noexcept { return {mRoot.block.width, mRoot.block.height}; }

	//! 領域がからかどうか調べます。
	constexpr bool empty() const noexcept { return size().empty(); }

	//! 使用中の領域を列挙します。
	std::vector<rect_type> uses() const
	{
		std::vector<rect_type>	retval;
		mRoot.each([&](auto* _block)
		{
			if (_block->flags == status::use)
			{
				retval.push_back(_block->block);
			}
		});
		return	retval;
	}

	//! 空いているの領域を列挙します。
	std::vector<rect_type> spaces() const
	{
		std::vector<rect_type>	retval;
		mRoot.each([&](auto* _block)
		{
			if (_block->flags == status::nouse)
			{
				retval.push_back(_block->block);
			}
		});
		return	retval;
	}

	template<class FuncT>
	void each(FuncT&& _func) const
	{
		mRoot.each(std::forward<FuncT>(_func));
	}


public:
	enum class	status
	{
		nouse,		//!< 完全な空き領域
		use,		//!< 完全に使用中
		divided		//!< 分割中
	};

	struct	node
	{
		status		flags;			//!< ブロックの状態
		rect_type	block;
		node*		children[3]	= { nullptr, nullptr, nullptr };		//!< 子ブロック

		constexpr node()
			: flags(status::nouse)
			, block(0, 0, 0, 0)
		{}

		constexpr node(status _status, const rect_type& _block)
			: flags(_status)
			, block(_block)
		{}

		node(const node& _r)
			: flags(_r.flags)
			, block(_r.block)
		{
			if (flags == status::divided)
			{
				for (std::size_t i = 0; i < 3; ++i)
				{
					children[i] = new node(*_r.children[i]);
				}
			}
		}

		node(node&& _r)
			: flags(_r.flags)
			, block(_r.block)
		{
			for (std::size_t i = 0; i < 3; ++i)
			{
				children[i] = _r.children[i];
			}
			_r.flags	= status::nouse;
			_r.block	= {0, 0, 0, 0};
		}

		~node()
		{
			if (flags == status::divided)
			{
				for (auto* child : children)
				{
					delete	child;
				}
			}
		}

		template<class FuncT>
		void each(FuncT&& _func) const
		{
			_func(this);
			if (flags == status::divided)
			{
				for (auto& c : children)
				{
					c->each(std::forward<FuncT>(_func));
				}
			}
		}

		template<class Exec>
		node* allocate(const size_type& _requestSize, Exec&&)
		{
			// 使用済みかサイズが収まらない場合は確保失敗を返す。
			if ((flags == status::use) || (_requestSize.width > block.width) || (_requestSize.height > block.height))
			{
				return	nullptr;
			}

			// 分割中なら子領域から確保
			if (flags == status::divided)
			{
				for (auto* c : children)
				{
					if (auto* alloc = c->allocate(_requestSize, Exec()))
					{
						return	alloc;
					}
				}
				return	nullptr;
			}

			if constexpr (Exec::value)
			{
				// テクスチャサイズがぴったり収まるならそのまま使用中にする。
				if (_requestSize == block.size())
				{
					flags = status::use;
					return	this;
				}

				auto	[ use, f1, f2 ] = block.split_from_topleft(_requestSize);
				children[0]	= new node(status::use, use);
				children[1]	= new node(status::nouse,  f1);
				children[2]	= new node(status::nouse,  f2);
				flags		= status::divided;
				return	children[0];
			}
			else
			{
				// 確保可能であることを返すだけ
				return	this;
			}
		}

		bool deallocate(const rect_type& _r)
		{
			if (flags == status::nouse)
			{
				return	false;
			}
			if (flags == status::use)
			{
				if (block == _r)
				{
					flags	= status::nouse;
					return	true;
				}
				return	false;
			}
			// 子の中から領域を解放できるかトライ。
			bool	isdeallocate	= false;
			bool	hasAlive	= false;
			for (auto* child : children)
			{
				if (child->deallocate(_r))
				{
					isdeallocate	= true;
				}
				if (child->flags != status::nouse)
				{
					hasAlive	= true;
				}
			}
			if (!hasAlive)
			{
				// 子はすべて解放済みなのですべて削除する
				for (auto* child : children)
				{
					delete	child;
				}
				flags	= status::nouse;
			}
			return	isdeallocate;
		}
	};

	node		mRoot;
};



AMTRS_NAMESPACE_END
#endif
