/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__http__mini_http__hpp
#define	__libamtrs__net__http__mini_http__hpp
AMTRS_NET_NAMESPACE_BEGIN


//! エラーレスポンスが返された
struct	http_error_response_event
{
};

//! メモリ不足など実行時のエラーが発生した
struct	http_failure_event
{
};

//! 通信が完了した
struct	http_complite_event
{
};

//! データを受信しした
template<class View>
struct	http_recieve_event
{
	View	data;
};


//! ヘッダ部分のデータを受信しした
template<class View>
struct	http_header_recieve_event
{
	View	data;
};

//! 本体部分のデータを受信しした
template<class View>
struct	http_body_recieve_event
{
	View	data;
};

//! ヘッダ部分のデータを受信しした
struct	http_header_complite_event
{
};

//! 本体部分のデータを受信しした
struct	http_body_complite_event
{
};


class	http_connect {};


struct	mini_http_base
{
	enum class recieve_type
	{
		header_line		= 1,
		header_complite	= 2,
		body_data		= 3,
		body_complite	= 4,
	};

	static constexpr recieve_type	header_line		= recieve_type::header_line;
	static constexpr recieve_type	header_complite	= recieve_type::header_complite;
	static constexpr recieve_type	body_data		= recieve_type::body_data;
	static constexpr recieve_type	body_complite	= recieve_type::body_complite;
};




template<class StringT>
struct	mini_http_traits
{
	using	string_type			= StringT;
	using	header_list_type	= std::unordered_map<string_type, string_type>;
};



template<class StringT, class Traits = mini_http_traits<StringT>>
class	basic_mini_http
		: public mini_http_base
{
public:
	using	traits_type			= Traits;
	using	string_type			= typename traits_type::string_type;
	using	string_view_type	= std::basic_string_view<typename string_type::value_type>;
	using	header_list_type	= typename traits_type::header_list_type;
	using	recieve_callback	= bool(recieve_type _type, string_view_type _data);
	using	size_type			= size_t;

	// listener events.
	using	http_error_response_event	= net::http_error_response_event;
	using	http_failure_event			= net::http_failure_event;
	using	http_complite_event			= net::http_complite_event;
	using	http_recieve_event			= net::http_recieve_event<string_view_type>;
	using	http_header_recieve_event	= net::http_header_recieve_event<string_view_type>;
	using	http_body_recieve_event		= net::http_body_recieve_event<string_view_type>;
	using	http_header_complite_event	= net::http_header_complite_event;
	using	http_body_complite_event	= net::http_body_complite_event;

	// listener
	using	listener					= amtrs::listener<basic_mini_http<StringT, Traits>>;


	basic_mini_http() = default;
	basic_mini_http(basic_mini_http const&) = delete;
	basic_mini_http(basic_mini_http&&) = default;
	basic_mini_http(std::string_view _url);
	basic_mini_http& operator = (basic_mini_http const&) = delete;
	basic_mini_http& operator = (basic_mini_http&&) = default;

	bool empty() const noexcept { return mInstance.empty() || mInstance->empty(); }


	uri::info<char> const& uri_info() const noexcept { return mInstance->uri_info(); }


	void set_listener(listener* _listener)
	{
		mInstance->listener	= _listener;
	}

	void request()
	{
		mInstance->request();
	}

	
	size_type contents_length() const noexcept
	{
		return	mInstance->contents_length();
	}


protected:
	using	listener_type	= listener;

	struct	instance : ref_object
	{

		virtual uri::info<char> const& uri_info() const noexcept = 0;
		virtual void request() = 0;
		virtual size_type contents_length() const noexcept = 0;
		virtual bool empty() const noexcept = 0;


		header_list_type	headers;
		listener_type*		listener	= nullptr;
	};

	ref<instance>	mInstance;
};


using	mini_http	= basic_mini_http<std::string>;

AMTRS_NET_NAMESPACE_END

AMTRS_NAMESPACE_BEGIN

template<>
struct	listener<net::http_connect>
		: listener<
			void(net::http_error_response_event),
			void(net::http_failure_event),
			void(net::http_complite_event)
		>
{
};


template<class StringT, class Traits>
struct	listener<net::basic_mini_http<StringT, Traits>>
		: listener<
			net::network_listener,
			net::http_connect,
			//
			void(typename net::basic_mini_http<StringT, Traits>::http_recieve_event),
			void(typename net::basic_mini_http<StringT, Traits>::http_header_recieve_event),
			void(typename net::basic_mini_http<StringT, Traits>::http_body_recieve_event),
			//
			void(typename net::basic_mini_http<StringT, Traits>::http_header_complite_event),
			void(typename net::basic_mini_http<StringT, Traits>::http_body_complite_event)
		>
{
	using	http_error_response_event	= typename net::basic_mini_http<StringT, Traits>::http_error_response_event;
	using	http_failure_event			= typename net::basic_mini_http<StringT, Traits>::http_failure_event;
	using	http_complite_event			= typename net::basic_mini_http<StringT, Traits>::http_complite_event;

	using	http_recieve_event			= typename net::basic_mini_http<StringT, Traits>::http_recieve_event;
	using	http_header_recieve_event	= typename net::basic_mini_http<StringT, Traits>::http_header_recieve_event;
	using	http_body_recieve_event		= typename net::basic_mini_http<StringT, Traits>::http_body_recieve_event;

	using	http_header_complite_event	= typename net::basic_mini_http<StringT, Traits>::http_header_complite_event;
	using	http_body_complite_event	= typename net::basic_mini_http<StringT, Traits>::http_body_complite_event;
};

AMTRS_NAMESPACE_END
#include AMTRS_PLATFORM_INCLUDE(net-mini_http.hpp)
#endif
