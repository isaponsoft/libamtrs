/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__socket_server__hpp
#define	__libamtrs__net__socket_server__hpp
#include "../util/async_queue.hpp"
#include "../util/callback.hpp"
#include "socket.hpp"
#include "selector.hpp"
AMTRS_NET_NAMESPACE_BEGIN
class	socket_client
{
	using	iterator	= selector::iterator;
public:
	using	on_recv_type	= callback<void(socket&)>;

	socket_client(socket _s)
		: mSocket(std::move(_s))
	{}


	virtual ~socket_client()
	{
	}

	virtual void close()
	{
		if (!mSocket.empty())
		{
			mSocket.shutdown(SHUT_RDWR);
			mSocket.close();
		}
	}

	void on_recv(on_recv_type _callback)
	{
		onRecv	= std::move(_callback);
	}

	      net::socket& sock()       noexcept { return mSocket; }
	const net::socket& sock() const noexcept { return mSocket; }

	virtual void event_recv()
	{
		if (!onRecv.empty())
		{
			onRecv(mSocket);
		}
	}

	virtual void event_send()
	{
	}

	socket_client() = delete;
	socket_client(const socket_client&) = delete;
	socket_client(const socket_client&&) = delete;
	socket_client& operator = (const socket_client& _r) = delete;
	socket_client& operator = (socket_client&& _r) = delete;
protected:
	net::socket			mSocket;
	net::address		mAddress;
	on_recv_type		onRecv;
};


struct	server_options
{
	int		port;
};

template<class ClientT = socket_client>
class	socket_server
{
public:
	using	socket_type		= socket::socket_type;
	using	client_type		= ClientT;
	using	on_request_type	= callback<void(client_type*)>;

	socket_server(selector* _selector)
		: mSelector(_selector)
	{}

	virtual ~socket_server()
	{
		mClients.foreach([](auto* cli)
		{
			if (cli->it)
			{
				delete	cli;
			}
			return	false;
		});

		mListener.shutdown(SHUT_RD);
		mListener.clear();
	}

	void close()
	{
		mClose.store(1);
	}

	bool start(int _port)
	{
		mListener	= net::socket(AF_INET, SOCK_STREAM, 0);

		if (mListener.bind_inet(_port))
		{
			return	false;
		}

		if (mListener.listen(3))
		{
			return	false;
		}

		mSelector->add(sock(), selector::readable, [this](selector::flags_type _flags) -> bool
		{
			socket	sock	= mListener.accept();
			auto	fd		= sock.get();
			auto*	cli		= new client(std::move(sock));
			cli->thiz	= this;
			cli->it		= mSelector->add(fd, selector::readable|selector::writable, [cli](auto _flags) -> int
			{
				if (_flags & selector::readable)
				{
					cli->event_recv();
				}
				if (_flags & selector::writable)
				{
					cli->event_send();
				}
				if (cli->sock().empty())
				{
					cli->close();
				}
				return	true;
			});
			event_accept(cli);
			mClients.add(cli);
			return	true;
		});

		return	true;
	}

	bool dispatch(struct timeval _timeout)
	{
		if (mClose.load())
		{
			if (!mListener.empty())
			{
				mListener.close();
				mClients.foreach([](auto* cli)
				{
					if (cli->it)
					{
						delete	cli;
					}
					return	false;
				});
			}
			return	false;
		}

		bool	r	= mSelector->dispatch(_timeout);
		mClients.foreach([](auto* cli)
		{
			if (!cli->it)
			{
				delete	cli;
				return	false;
			}
			return	true;
		});
		return	r;
	}

	void on_request(on_request_type _callback)
	{
		mOnRequest	= std::move(_callback);
	}


	socket_type sock() const noexcept
	{
		return	mListener.get();
	}

	template<class Callback>
	void foreach_clients(Callback _callback)
	{
		mClients.foreach([&](auto* c)
		{
			_callback(static_cast<client_type*>(c));
			return	true;
		});
	}

protected:
	struct	client : client_type
	{
		using	client_type::client_type;

		virtual ~client()
		{
		}

		virtual void close() override
		{
			client_type::close();
			if (it)
			{
				thiz->mSelector->remove(it);
				it	= nullptr;
			}
		}
		selector::iterator	it;
		socket_server*		thiz;
	};

	virtual void event_accept(client_type* _client)
	{
		if (!mOnRequest.empty())
		{
			mOnRequest(_client);
		}
	}

	selector*				mSelector;
	on_request_type			mOnRequest;
	async_queue<client*>	mClients;
	net::socket				mListener;
	int						mPort;
	std::atomic<int>		mClose	= false;
};
AMTRS_NET_NAMESPACE_END
#endif
