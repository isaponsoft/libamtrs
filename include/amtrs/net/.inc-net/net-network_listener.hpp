/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__network_listener__hpp
#define	__libamtrs__net__network_listener__hpp
AMTRS_NET_NAMESPACE_BEGIN


//! ネットワーク接続が行われた
struct	network_connecting_event
{
};

//! ネットワーク接続が切断された(error, failure, complite の後に必要に呼ばれます)
struct	network_disconnected_event
{
};


//! メモリ不足など実行時のエラーが発生した
struct	network_failure_event
{
};


struct	network_listener
{
};


AMTRS_NET_NAMESPACE_END
AMTRS_NAMESPACE_BEGIN
template<>
struct	listener<net::network_listener>
		: listener<
			void(net::network_connecting_event),
			void(net::network_disconnected_event),
			void(net::network_failure_event)
		>
{
};
AMTRS_NAMESPACE_END
#endif
