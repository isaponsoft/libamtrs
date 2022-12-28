/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/net/selector.hpp>
AMTRS_NET_NAMESPACE_BEGIN
static constexpr selector::flags_type		read_ready		= 1<<8;
static constexpr selector::flags_type		write_ready		= 1<<9;

bool selector::dispatch(struct timeval _timeout)
{
	int			qCount		= 0;
	element*	elements[FD_SETSIZE];
	fd_set		rfds;		FD_ZERO(&rfds);
	fd_set		wfds;		FD_ZERO(&wfds);
	socket_type	maxfd		= 0;
	mSockets.foreach([&](element& e)
	{
		bool	add	= false;
		if (e.flags & readable)
		{
			FD_SET(e.s, &rfds);
			add	= true;
		}
		if (e.flags & writable)
		{
			FD_SET(e.s, &wfds);
			add	= true;
		}
		if (!add)
		{
			return	true;
		}
		maxfd				= maxfd > e.s ? maxfd : e.s;
		elements[qCount++]	= &e;
		if (qCount < FD_SETSIZE)
		{
			return	true;
		}
		::select(maxfd+1, &rfds, &wfds, nullptr, &_timeout);
		for (int i = 0; i < qCount; ++i)
		{
			if (elements[i]->flags & readable && FD_ISSET(elements[i]->s, &rfds)) { elements[i]->flags |= read_ready;  }
			if (elements[i]->flags & writable && FD_ISSET(elements[i]->s, &wfds)) { elements[i]->flags |= write_ready; }
		}
		qCount	= 0;
		maxfd	= 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		return	true;
	});
	if (qCount > 0)
	{
		::select(maxfd+1, &rfds, &wfds, nullptr, &_timeout);
		for (int i = 0; i < qCount; ++i)
		{
			if (elements[i]->flags & readable && FD_ISSET(elements[i]->s, &rfds)) { elements[i]->flags |= read_ready;  }
			if (elements[i]->flags & writable && FD_ISSET(elements[i]->s, &wfds)) { elements[i]->flags |= write_ready; }
		}
	}
	mSockets.foreach([&](element& e)
	{
		flags_type	flags	= 0;
		if (e.flags & read_ready ) { flags |= readable; e.flags &= ~read_ready;  }
		if (e.flags & write_ready) { flags |= writable; e.flags &= ~write_ready; }
		if (flags)
		{
			return	e.cb->invoke(flags);
		}
		return	true;
	});
	return	!empty();
}


bool selector::empty() const noexcept
{
	return	mSockets.empty();
}


void selector::remove(iterator _it)
{
	mSockets.remove(_it);
}

void selector::monitor(iterator _it, flags_type _monitor)
{
	reinterpret_cast<element*>(_it)->flags = _monitor;
}


auto selector::_add(socket_type _sock, flags_type _monitor, callback* _callback) -> iterator
{
	element	e;
	e.cb	= _callback;
	e.s		= _sock;
	e.flags	= _monitor;
	return	mSockets.add(std::move(e));
}


AMTRS_NET_NAMESPACE_END
