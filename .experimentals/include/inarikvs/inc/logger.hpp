/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs__logger__hpp
#define	__inarikvs__logger__hpp
#include "kv_data.hpp"
INARIKVS_NAMESPACE_BEGIN

template<class StorageT>
class	logger
{
public:
	using	storage_type	= StorageT;

	struct	log_data
	{
		log_data() = default;
		log_data(log_data&&) = default;
		log_data(log_type _type, kv_data* _kv)
			: type(_type)
			, kv(_kv)
		{}

		log_type			type		= log_type::del;
		amtrs::ref<kv_data>	kv;
	};


	logger() = default;
	logger(logger&&) = default;


	void add(log_type _type, kv_data* _kv)
	{
		auto	key		= _kv->key();
		auto	value	= _kv->value();

		{
			uint8_t		type	= static_cast<uint8_t>(_type);
			mJournalFile.write(&type, sizeof(type));

			uint16_t	klen	= static_cast<uint16_t>(key.size());
			mJournalFile.write(&klen, sizeof(klen));
			mJournalFile.write(key);
		}
		if (_type == log_type::del)
		{
			// CheckSum
			uint32_t	cs		= compute_checksum(key.data(), key.size());
			mJournalFile.write(&cs, sizeof(cs));
		}
		if (_type == log_type::set)
		{
			uint32_t	vlen	= static_cast<uint16_t>(value.size());
			mJournalFile.write(&vlen, sizeof(vlen));
			mJournalFile.write(value);

			// CheckSum
			uint32_t	cs		= compute_checksum(key.data(), key.size(), value.data(), value.size());
			mJournalFile.write(&cs, sizeof(cs));
		}

		mAppendLogs.push_back(log_data(_type, _kv));
	}


	template<class GetKey>
	void load_recover(GetKey&& _getkey)
	{
		mJournalFile.set_position(0);

		for (;;)
		{
			log_data	log;

			uint8_t		type	= 0;
			auto	rs	= mJournalFile.read(&type, sizeof(type));
			if (rs == 0)
			{
				break;
			}


			log.type	= static_cast<log_type>(type);
			log.kv		= new kv_data();

			// キーの読み取り
			log.kv->journal_key_load(mJournalFile);

			// 同一キーがDBにロードされているか調べる
			if (kv_data* kv = _getkey(log.kv); kv)
			{
				log.kv	= kv;
			}


			if (log.type == log_type::del)
			{
				uint32_t	cs		= 0;
				mJournalFile.read(&cs, sizeof(cs));
				log.kv->set_state(kv_state::del);
			}

			// 値の読み取り
			if (log.type == log_type::set)
			{
				log.kv->journal_value_load(mJournalFile);

				uint32_t	cs	= 0;
				mJournalFile.read(&cs, sizeof(cs));
				log.kv->set_state(kv_state::update);
			}

			mAppendLogs.push_back(std::move(log));
		}
	}


	void create_journal(const std::string& _filename)
	{
		mJournalFile.open(_filename);
	}

	uint32_t compute_checksum(const char* _key, std::size_t _keyLength)
	{
		uint32_t	cs	= 0;
		for (const char* end = _key + _keyLength; _key != end; ++_key)
		{
			cs += *_key;
		}
		return	cs;
	}

	uint32_t compute_checksum(const char* _key, std::size_t _keyLength, const char* _value, std::size_t _valueLength)
	{
		uint32_t	cs	= 0;
		for (const char* end = _key + _keyLength; _key != end; ++_key)
		{
			cs += *_key;
		}
		for (const char* end = _value + _valueLength; _value != end; ++_value)
		{
			cs += *_key;
		}
		return	cs;
	}


	bool empty() const noexcept { return mAppendLogs.empty(); }

	void clear()
	{
		for (auto& log : mAppendLogs)
		{
			auto&	kv	= *log.kv;
			log.kv->set_state(kv_state::keep);
		}
		mAppendLogs.clear();
		mJournalFile.remove();
	}

	auto begin() noexcept { return mAppendLogs.begin(); }
	auto end() noexcept { return mAppendLogs.end(); }

	void sync()
	{
		mJournalFile.sync();
	}

	auto size() const noexcept { return mAppendLogs.size(); }

private:
	std::vector<log_data>			mAppendLogs;
	storage_type					mJournalFile;
};


INARIKVS_NAMESPACE_END
#endif
