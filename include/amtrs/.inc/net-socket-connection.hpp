/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__socket__socket_connection__hpp
#define	__libamtrs__net__socket__socket_connection__hpp
AMTRS_NET_NAMESPACE_BEGIN

template<class Socket>
class	basic_socket_connection;

using	socket_connection	= basic_socket_connection<socket>;



//! ソケット接続が行われた
struct	socket_connection_event
{};

//! ソケットが切断された
struct	socket_disconnect_event
{};



template<class Socket>
struct	basic_socket_listener : network_listener
{};



// ****************************************************************************
//! socketにリスナーなどの機能を付与する。
// ----------------------------------------------------------------------------
template<class Socket>
class	basic_socket_connection
{
public:
	using	socket_type	= Socket;
	using	listener	= basic_socket_listener<Socket>;

	basic_socket_connection() = default;
	basic_socket_connection(basic_socket_connection&&) = default;
	basic_socket_connection(basic_socket_connection const&) = delete;
	basic_socket_connection& operator = (basic_socket_connection&&) = default;
	basic_socket_connection& operator = (basic_socket_connection const&) = delete;


	template<class URI>
	static basic_socket_connection connect(URI const& _uri)
	{
		basic_socket_connection	retval;
		uri::info<char>			uinf;
		retval.mSocket	= net::socket::connect(&uinf, _uri);
		return	retval;
	}


	bool empty() const noexcept
	{
		return	mSocket.empty();
	}
	


protected:
	socket_type	mSocket;
	listener*	mListener	= nullptr;
};


AMTRS_NET_NAMESPACE_END
#endif
