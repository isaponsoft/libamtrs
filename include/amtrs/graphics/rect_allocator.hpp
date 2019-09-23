#ifndef	__libamtrs__graphics__rect_allocator__hpp
#define	__libamtrs__graphics__rect_allocator__hpp
#include <tuple>
#include <vector>
#include "def.hpp"
#include "rect.hpp"
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! �Q������`�̗̈�Ǘ����s���܂��B
// ----------------------------------------------------------------------------
//! �R���X�g���N�^�Ŏw�肵���Q�����T�C�Y����Aallocate() �Ŏw�肷��Q�����̈��
//! �m�ۂ�����A�J�������肵�܂��B
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

	//! �w�肵���T�C�Y�̋�`�̈���Ǘ����܂��B
	constexpr rect_allocator(const size_type& _size)
		: mRoot(status::nouse, {{0, 0}, _size})
	{}

	//! �w�肵���ʒu�ƃT�C�Y�̋�`�̈���Ǘ����܂��B
	constexpr rect_allocator(const rect_type& _rect)
		: mRoot(status::nouse, {_rect})
	{}

	rect_allocator() = default;
	rect_allocator(const rect_allocator& _r) = default;
	rect_allocator(rect_allocator&& _r) = default;
	rect_allocator& operator = (const rect_allocator& _r) = default;
	rect_allocator& operator = (rect_allocator&& _r) = default;
	~rect_allocator() = default;

	//! �m�ۂɎ��s�����ꍇ�� empty() ��Ԃ��܂��B
	rect_type allocate(const size_type& _size)
	{
		auto	it	= mRoot.allocate(_size, std::true_type{});
		return	(it)
				? it->block
				: rect_type(0, 0, 0, 0);
	}

	//! ���݂��Ȃ��̈�̏ꍇ�� false ��Ԃ��A�������܂���B
	bool deallocate(const rect_type& _r)
	{
		return	mRoot.deallocate(_r);
	}

	//! �m�ۉ\���₢���킹�܂��B
	bool fetch(const size_type& _size) const noexcept
	{
		return	const_cast<rect_allocator*>(this)->mRoot.allocate(_size, std::false_type{})
				? true
				: false;
	}

	//! �Ǘ��̈�S�̂̑傫����Ԃ��܂��B
	constexpr size_type size() const noexcept { return {mRoot.block.width, mRoot.block.height}; }

	//! �̈悪���炩�ǂ������ׂ܂��B
	constexpr bool empty() const noexcept { return size().empty(); }

	//! �g�p���̗̈��񋓂��܂��B
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

	//! �󂢂Ă���̗̈��񋓂��܂��B
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
		nouse,		//!< ���S�ȋ󂫗̈�
		use,		//!< ���S�Ɏg�p��
		divided		//!< ������
	};

	struct	node
	{
		status		flags;			//!< �u���b�N�̏��
		rect_type	block;
		node*		children[3]	= { nullptr, nullptr, nullptr };		//!< �q�u���b�N

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
			// �g�p�ς݂��T�C�Y�����܂�Ȃ��ꍇ�͊m�ێ��s��Ԃ��B
			if ((flags == status::use) || (_requestSize.width > block.width) || (_requestSize.height > block.height))
			{
				return	nullptr;
			}

			// �������Ȃ�q�̈悩��m��
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
				// �e�N�X�`���T�C�Y���҂�������܂�Ȃ炻�̂܂܎g�p���ɂ���B
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
				// �m�ۉ\�ł��邱�Ƃ�Ԃ�����
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
			// �q�̒�����̈������ł��邩�g���C�B
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
				// �q�͂��ׂĉ���ς݂Ȃ̂ł��ׂč폜����
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
