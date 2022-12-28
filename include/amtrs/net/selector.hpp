/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__selector__hpp
#define	__libamtrs__net__selector__hpp
#include "../util/async_queue.hpp"
#include "socket.hpp"
AMTRS_NET_NAMESPACE_BEGIN
class	selector
{
protected:
	struct	element;
public:
	using	flags_type		= unsigned int;
	using	socket_type		= socket::socket_type;
	using	iterator		= typename async_queue<element>::iterator;
	static constexpr flags_type		readable	= 1<<0;
	static constexpr flags_type		writable	= 1<<1;


	template<class Callback>
	iterator add(socket_type _sock, flags_type _monitor, Callback _callback)
	{
		struct	invoker : callback
		{
			Callback	cb;
			invoker(Callback _args) : cb(std::move(_args)) {}
			virtual bool invoke(flags_type _flags) override { return cb(_flags); }
		};
		return	_add(_sock, _monitor, new invoker(std::move(_callback)));
	}

	bool dispatch(struct timeval _timeout);
	bool empty() const noexcept;
	void remove(iterator _it);
	void monitor(iterator _it, flags_type _monitor);

protected:
	struct	callback
	{
		virtual ~callback() {}
		virtual bool invoke(flags_type _flags) = 0;
	};

	struct	element
	{
		callback*					cb			= nullptr;
		socket_type					s;
		std::atomic<flags_type>		flags		= 0;
		element() = default;
		element(const element&) = delete;
		~element() { if (cb) { delete cb; } }
		element(element&& _r)
			: cb(_r.cb)
			, s(_r.s)
		{
			flags.store(_r.flags.load());
			_r.cb	= nullptr;
		}
	};

	iterator _add(socket_type _sock, flags_type _monitor, callback* _callback);

	async_queue<element>	mSockets;
};
AMTRS_NET_NAMESPACE_END
#endif
