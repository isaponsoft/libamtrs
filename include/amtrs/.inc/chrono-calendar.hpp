/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__chrono__calendar__hpp
#define	__libamtrs__chrono__calendar__hpp
AMTRS_CHRONO_NAMESPACE_BEGIN
namespace v1 {
	enum enumrate_flag
	{
		enum_systems	= 1<<0,		//!< システムのカレンダーも列挙します
	};
	struct	event
	{
		std::string	identify;
		std::string	title;
		std::string	description;
		std::string	url;
		timev		start;
		timev		end;
	};

	template<class SizeT = uint32_t, class Out>	bool serialize(Out& _out, event const& _val);
	template<class In>							bool deserialize(event& _buff, In& _in);
}


class	calendar
		: public ref_object
{
public:
	using	enumrate_flag	= v1::enumrate_flag;
	using	event			= v1::event;


	//! カレンダーへのアクセス権を取得します。
	static void permission(std::function<void(bool _enable)> _callback);

	//! カレンダーを列挙します。
	static void enumrate(unsigned int _flags, std::function<void(ref<calendar> _calendar)> _callback);

	//! 識別子に一致するカレンダーを返す
	static ref<calendar> create(std::string _identify);

	//! カレンダーの識別子。
	virtual std::string identify() const = 0;

	//! カレンダーの名称。
	virtual std::string name() const = 0;

	//! カレンダーのアカウント。
	virtual std::string account() const = 0;

	//! カレンダーのオーナーアカウント。
	virtual std::string owner_account() const = 0;

	//! カレンダーの色。
//	virtual rgb<uint8_t> color() const = 0;

	//! イベントを列挙します。
	virtual void events(std::function<void(event const&)> _callback) = 0;

	//! イベントを新規保存します。
	//! identify は無視され、戻り値に新しいidentifyを返します。
	virtual std::string insert(event const& _e) = 0;

	//! イベントを保存します。
	//! identify を　empty() にした場合は新規書き込みになります。
	virtual bool update(event const& _e) = 0;

	//! イベントを削除します。
	virtual bool erase(std::string const& _identify) = 0;

	//! 書き込み可能なら true を返します。
	virtual bool writable() const = 0;

	//! OpenEV サーバーからロードします。
	static ref<calendar> openev_with_url(std::string _url, std::string _title = {});

};



namespace v1 {
	//! コンテナをシリアライズします。
	template<class SizeT, class Out>
	bool serialize(Out& _out, event const& _val)
	{
		using	traits	= decltype(io::choise_stream_traits(std::declval<Out&>()));
		if (!traits{}.good(_out))
		{
			return	false;
		}

		// バージョンコード
		uint8_t	v	= 1;
		if (!amtrs::serialize(_out, v))
		{
			return	false;
		}
		// 
		if (!amtrs::serialize(_out, _val.identify)
		 || !amtrs::serialize(_out, _val.title)
		 || !amtrs::serialize(_out, _val.description)
		 || !amtrs::serialize(_out, _val.url)
		 || !amtrs::serialize(_out, _val.start)
		 || !amtrs::serialize(_out, _val.end))
		{
			return	false;
		}
		return	traits{}.good(_out);
	}


	//! 整数型の値を指定したエンディアンでデシリアライズします。
	template<class In>
	bool deserialize(event& _buff, In& _in)
	{
		using	traits	= decltype(io::choise_stream_traits(std::declval<In&>()));
		if (!traits{}.good(_in))
		{
			return	false;
		}
		uint8_t	version;
		if (!amtrs::deserialize(version, _in))
		{
			return	false;
		}
		if (version != 1)
		{
			return	false;
		}
		if (!amtrs::deserialize(_buff.identify,		_in)
		 || !amtrs::deserialize(_buff.title,		_in)
		 || !amtrs::deserialize(_buff.description,	_in)
		 || !amtrs::deserialize(_buff.url,			_in)
		 || !amtrs::deserialize(_buff.start,		_in)
		 || !amtrs::deserialize(_buff.end,			_in))
		{
			return	false;
		}
		return	traits{}.good(_in);
	}
}
AMTRS_CHRONO_NAMESPACE_END
AMTRS_NAMESPACE_BEGIN

//! コンテナをシリアライズします。
template<class SizeT, class Out>
bool serialize(Out& _out, amtrs::chrono::v1::event const& _val)
{
	return	chrono::v1::serialize(_out, _val);
}


//! 整数型の値を指定したエンディアンでデシリアライズします。
template<class In>
bool deserialize(amtrs::chrono::v1::event& _buff, In& _in)
{
	return	chrono::v1::deserialize(_buff, _in);
}

AMTRS_NAMESPACE_END
#endif
