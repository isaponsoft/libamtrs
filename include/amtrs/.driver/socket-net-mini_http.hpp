/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__http__mini_http__socket__hpp
#define	__libamtrs__net__http__mini_http__socket__hpp
AMTRS_NET_NAMESPACE_BEGIN


template<class StringT>
class	mini_http_socket : public basic_mini_http<StringT>
{
public:
	using	super_type			= basic_mini_http<StringT>;
	using	traits_type			= typename super_type::traits_type;
	using	string_type			= typename traits_type::string_type;
	using	string_view_type	= typename super_type::string_view_type;
	using	size_type			= typename super_type::size_type;
	using	listener_type		= typename super_type::listener_type;
	using	value_type			= typename string_type::value_type;

	struct	instance : basic_mini_http<StringT>::instance
	{
		using	listener_wrapper	= io::listener_stream<value_type, io::vstream<char>>;

		std::string			url;
		uri::info<char>		uinf;
		net::socket			sock;
		io::vstream<char>	io;
		http::parser		psr;

		virtual uri::info<char> const& uri_info() const noexcept override
		{
			return	uinf;
		}

		virtual size_type contents_length() const noexcept override
		{
			return	psr.contents_length();
		}


		virtual void request() override
		{
			psr	= net::http::parser(uinf.scheme == "https");

			std::string		req = "GET ";
			req += uinf.uri;
			req += " HTTP/1.1";
			req += "\r\n";
			req += "Host: ";
			req += uinf.host;
			req += "\r\n";
			req += "User-Agent: libamtrs\r\n";
			req += "Accept: */*\r\n";
			req += "Connection: Close\r\n";
			req += "\r\n";

			// データを送信
			size_t	ws	= 0;
			while (!io.bad() && !io.eof() && (ws != req.size()))
			{
				io.write(req.data() + ws, req.size() - ws);
				ws += io.pcount();
			}

			auto	l	= make_listener<listener_wrapper>([this](auto e)
			{
				listener_type&	l(*this->listener);
				if constexpr (std::is_same<decltype(e), io::on_read<value_type>>{})
				{
					if (this->listener)
					{
						l(typename super_type::http_recieve_event{ string_view_type(e.data, e.size) });
					}
				}
			});

			listener_wrapper	io2(&l, &io);

			psr.header(io2, [&](char const* _line, size_t _size)
			{

				if (this->listener)
				{
					(*this->listener)(typename super_type::http_header_recieve_event{ string_view_type(_line, _size) });
				}
				return	true;
			});
			if (this->listener)
			{
				(*this->listener)(typename super_type::http_header_complite_event{});
			}

			if (io2.good())
			{
				std::vector<char>	buff((sock.mtu() - 40) * 2);
				do
				{
					auto	rs	= psr.read(buff.data(), io2, buff.size());
					if (this->listener)
					{
						(*this->listener)(typename super_type::http_body_recieve_event{ string_view_type(buff.data(), rs) });
					}
				} while (io2.good() && !psr.empty());

			}
			if (this->listener)
			{
				(*this->listener)(typename super_type::http_body_complite_event{});
			}
		}

		virtual bool empty() const noexcept override
		{
			return	sock.empty() || (!io.good() && !io.eof());
		}

	};

	static ref<instance> create(std::string_view _url)
	{
		ref<instance>	retval	= new instance();
		retval->url		= _url;


		retval->sock	= net::socket::connect(&retval->uinf, retval->url, SOCK_STREAM);
		if (retval->sock.empty())
		{
			return	{};
		}

		#if	AMTRS_SSL_SUPPORTED
		retval->io	= retval->uinf.scheme == "https"
					? io::make_vstream(net::basic_ssl_stream<char, socket>(retval->sock.get()))
					: io::make_vstream(net::socket_stream(retval->sock.get()));
		#else
		retval->io	= io::make_vstream(net::socket_stream(retval->sock.get()));
		#endif

		return	retval;
	}


	template<class, class>
	friend	class	basic_mini_http;
};


template<class StringT, class Traits>
basic_mini_http<StringT, Traits>::basic_mini_http(std::string_view _url)
{
	mInstance	= mini_http_socket<StringT>::create(_url);
}

AMTRS_NET_NAMESPACE_END
#endif
