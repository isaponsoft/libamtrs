﻿#ifndef	__libamtrs__node__pager_node__hpp
#define	__libamtrs__node__pager_node__hpp
#include "node.hpp"
#include "pagable_node.hpp"
AMTRS_NAMESPACE_BEGIN


template<class NodeT>
class	pager_node;


template<class BaseNodeT>
class	page_node_if;

template<class NodeT>
class	pager_node_if
{
public:
	using			node_type		= typename NodeT::node_type;
	using			page_type		= page_node_if<node_type>;
	using			pager_type		= pager_node_if<node_type>;
	using			index_type		= std::size_t;

	static constexpr std::size_t	npage	= static_cast<std::size_t>(-1);

	virtual ~pager_node_if() {}

	virtual node_type* get_node() = 0;

	template<class PageNodeT>
	void add_page(pagable_node<PageNodeT>* _page, bool _active = false)
	{
		static_assert(std::is_same<typename PageNodeT::node_type, node_type>::value);
		on_add_page(_page, _page, _active);
		if (mPageAutoResize)
		{
			_page->set_size(this->get_node()->size());
		}
	}

	template<class RefT>
	void add_page(const ref<RefT>& _page, bool _active = false)
	{
		static_assert(std::is_same<typename RefT::node_type, node_type>::value);
		on_add_page(_page.get(), _page.get(), _active);
		if (mPageAutoResize)
		{
			_page->set_size(this->get_node()->size());
		}
	}


	virtual index_type page_index() const noexcept = 0;
	virtual page_type* get_page(index_type _page) noexcept = 0;

	bool set_page(index_type _page)
	{
		return	set_page(_page, [](){});
	}

	bool set_page(index_type _page, std::function<void()>&& _pageAnimeFinished)
	{
		if (npage == _page)
		{
			return	false;
		}
		return	on_set_page(_page, std::move(_pageAnimeFinished));
	}


	bool back()
	{
		return	set_page(page_index()-1);
	}

	bool back(std::function<void()>&& _pageAnimeFinished)
	{
		return	set_page(page_index()-1, std::move(_pageAnimeFinished));
	}

	bool next()
	{
		return	set_page(page_index()+1);
	}

	bool next(std::function<void()>&& _pageAnimeFinished)
	{
		return	set_page(page_index()+1, std::move(_pageAnimeFinished));
	}


	void set_page_auto_resize(bool _enable)
	{
		if (mPageAutoResize == _enable)
		{
			mPageAutoResize = _enable;
			if (mPageAutoResize)
			{
				if (auto cp = get_page())
				{
					cp->set_size(get_node()->size());
				}
			}
		}
	}

	virtual index_type find_page_index(const std::string& _name) const noexcept = 0;


protected:
	virtual void on_add_page(node_type*, page_type*, bool _active) = 0;
	virtual bool on_set_page(index_type _page, std::function<void()>&& _pageAnimeFinished) = 0;


	bool	mPageAutoResize	= true;
};


template<class NodeT>
class	pager_node
		: public NodeT
		, public pager_node_if<typename NodeT::node_type>
{
	using	_base_type	= NodeT;
public:
	using			node_type				= typename NodeT::node_type;
	using			page_type				= page_node_if<node_type>;
	using			pager_type				= pager_node_if<node_type>;
	using			page_event_type			= typename page_type::page_event_type;
	using			index_type				= typename pager_type::index_type;

	static constexpr std::size_t	npage	= static_cast<std::size_t>(-1);

	pager_node()
		: mPageTransform(this)
	{}



	virtual node_type* get_node() override { return this; }

	// ========================================================================
	//! 持っているページの数を返します。
	// ------------------------------------------------------------------------
	std::size_t count() const noexcept { return mPageList.size(); }

	// ========================================================================
	//! ページを持っていない場合は true を返します。
	// ------------------------------------------------------------------------
	bool empty() const noexcept { return count() == 0; }


	virtual index_type page_index() const noexcept override
	{
		auto	it	= std::find_if(mPageList.begin(), mPageList.end(), [this](auto& a) -> bool { return a.second == mPage; });
		return	it != mPageList.end()
				? std::distance(mPageList.begin(), it)
				: npage;
	}

	virtual page_type* get_page(index_type _page) noexcept override
	{
		if (_page < mPageList.size())
		{
			return	nullptr;
		}
		return	mPageList[_page].second;
	}

	index_type find_page_index(const std::string& _name) const noexcept override
	{
		auto	it	= std::find_if(mPageList.begin(), mPageList.end(), [_name](auto& a) -> bool { return a.first->name() == _name; });
		return	it != mPageList.end()
				? std::distance(mPageList.begin(), it)
				: npage;
	}


	void set_page_changed_callback(std::function<void()>&& _callback)
	{
		mPageChangedCallback	= std::move(_callback);
	}

	auto pages() const noexcept { return mPageList; }

protected:
	virtual void on_add_page(node_type* _cn, page_type* _pn, bool _active) override
	{
		mPageList.push_back({_cn, _pn});
		_base_type::add_child(_cn);
		_pn->mPagerIf		= this;
		_cn->set_parent_transform(&mPageTransform);
		if (_active || (mPage == nullptr))
		{
			this->set_page(mPageList.size() - 1);
		}
		else
		{
			reposition();
		}
	}

	virtual bool on_set_page(index_type _page, std::function<void()>&& _pageAnimeFinished) override final
	{
		if ((_page >= 0) && (_page < count()))// && (mPage != mPageList[_page].second))
		{
			if (mPage != mPageList[_page].second)
			{
				mPage	= mPageList[_page].second;
				if (mPageChangedCallback)
				{
					mPageChangedCallback();
				}
			}
			if (auto spos = -compute_page_position(page_index()); spos != mPageTransform.position())
			{
				_base_type::run_action(
					(new move_by_action(&mPageTransform, spos, 0.2f))
					->set_finish_callback(std::move(_pageAnimeFinished))
				);
			}
			reposition();
			check_visible();
			return	true;
		}
		return	false;
	}

	virtual void on_child(node_type* _child, typename _base_type::on_child_event _event) override
	{
		_base_type::on_child(_child, _event);
		if (_event == _base_type::on_child_event::remove)
		{
			auto	index	= page_index();
			auto	it		= std::remove_if(mPageList.begin(), mPageList.end(), [_child](auto& a) -> bool
								{ return a.first == _child; }
							);
			if (it != mPageList.end())
			{
				it->second->mPagerIf	= nullptr;
				mPageList.erase(it, mPageList.end());
				if (index == mPageList.size())
				{
					if (index == 0)
					{
						mPage	= nullptr;
					}
					else
					{
						mPage	= mPageList[index - 1].second;

						mPageTransform.set_position(-compute_page_position(page_index()));

					}
				}
				sort();
				reposition();
				check_visible();
			}
		}
	}


	virtual void on_size_change(typename _base_type::size_type& _size) noexcept override
	{
		_base_type::on_size_change(_size);
		sort();
		mPageTransform.set_position(-compute_page_position(page_index()));
		reposition();
		check_visible();
	}


	virtual void on_coordinate_modify() noexcept override
	{
		_base_type::on_coordinate_modify();
		mPageTransform.coordinate_modify();
	}


	virtual vec3<float> compute_page_position(index_type _page) const = 0;

	// ========================================================================
	//! ページの座標を設定します。
	// ------------------------------------------------------------------------
	//! ページを一時的にスクロールさせたい時などに使用します。
	// ------------------------------------------------------------------------
	void set_pages_position(typename _base_type::position_type _position) noexcept
	{
		if (mPageTransform.position() != _position)
		{
			mPageTransform.set_position (_position);
			check_visible();
		}
	}

private:
	void check_visible()
	{
		rect<float>	visibleArea(-mPageTransform.position().x, -mPageTransform.position().y, this->size().width, this->size().height);
		for (auto& p : mPageList)
		{
			node_type*	c			= p.first;
			rect<float>	pageArea	= {c->position().x, c->position().y, c->size().width, c->size().height};
			bool		visible		= visibleArea.is_collision(pageArea);
			fireVisible(p.second, visible ? page_event_type::visible : page_event_type::invisible);
		}
	}

	void reposition()
	{
		index_type	page	= 0;
		for (auto& p : mPageList)
		{
			node_type*	c			= p.first;
			c->set_size(this->size());
			c->set_pivot({0, 0, 0});
			c->set_position(compute_page_position(page));
			++page;
		}
	}


	void fireVisible(page_type* _page, page_event_type _event)
	{
		_page->fireVisible(_event);
	}


	void sort()
	{
		std::sort(mPageList.begin(), mPageList.end(), [](auto& a, auto& b)
		{
			return	a.first->priority() < b.first->priority();
		});
	}

	class	local_transform
			: public component::transform_component<node_type>
	{
		using	_base_type	= component::transform_component<node_type>;
	public:
		local_transform(pager_node* _owner)
			: _base_type(_owner, _owner)
			, mOwner(_owner)
		{}

		virtual void on_coordinate_modify() noexcept override
		{
			component::transform_component<node_type>::on_coordinate_modify();
			mOwner->check_visible();
		}

	private:
		pager_node*	mOwner;
	};

	std::vector<std::pair<node_type*, page_type*>>	mPageList;
	page_type*										mPage		= nullptr;
	local_transform									mPageTransform;
	std::function<void()>							mPageChangedCallback;

	friend	local_transform;
};


AMTRS_NAMESPACE_END
#endif
