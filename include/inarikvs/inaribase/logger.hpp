#ifndef	__inarikvs__logger__hpp
#define	__inarikvs__logger__hpp
#include "file.hpp"
#include "kv_data.hpp"
INARIKVS_NAMESPACE_BEGIN

class	logger
{
public:
	logger() = default;
	logger(logger&&) = default;

	file		mJournalFile;

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

			uint16_t	klen	= 0;
			mJournalFile.read(&klen, sizeof(klen));

			log.type	= static_cast<log_type>(type);
			log.kv		= new kv_data();

			// キーの読み取り
			{
				log.kv->key.length		= klen;
				log.kv->key.data		= new char[log.kv->key.length + 1];
				mJournalFile.read(log.kv->key.data, log.kv->key.length);
				log.kv->key.data[log.kv->key.length]	= 0;
			}

			// 同一キーがDBにロードされているか調べる
			if (kv_data* kv = _getkey(log.kv); kv)
			{
				log.kv	= kv;
			}


			if (log.type == log_type::del)
			{
				uint32_t	cs		= 0;
				mJournalFile.read(&cs, sizeof(cs));
				log.kv->state	= kv_state::del;
			}

			// 値の読み取り
			if (log.type == log_type::set)
			{
				uint32_t	vlen	= 0;
				mJournalFile.read(&vlen, sizeof(vlen));

				log.kv->value.length	= vlen;
				log.kv->value.data		= new char[log.kv->value.length + 1];
				mJournalFile.read(log.kv->value.data, log.kv->value.length);
				log.kv->value.data[log.kv->value.length]	= 0;

				uint32_t	cs	= 0;
				mJournalFile.read(&cs, sizeof(cs));
				log.kv->state	= kv_state::update;
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


	void add(log_data&& _log)
	{
		{
			uint8_t		type	= static_cast<uint8_t>(_log.type);
			mJournalFile.write(&type, sizeof(type));

			uint16_t	klen	= static_cast<uint16_t>(_log.kv->key.length);
			mJournalFile.write(&klen, sizeof(klen));
			mJournalFile.write(_log.kv->key.data, _log.kv->key.length);
		}
		if (_log.type == log_type::del)
		{
			// CheckSum
			uint32_t	cs		= compute_checksum(_log.kv->key.data, _log.kv->key.length);
			mJournalFile.write(&cs, sizeof(cs));
		}
		if (_log.type == log_type::set)
		{
			uint32_t	vlen	= static_cast<uint16_t>(_log.kv->value.length);
			mJournalFile.write(&vlen, sizeof(vlen));
			mJournalFile.write(_log.kv->value.data, _log.kv->value.length);

			// CheckSum
			uint32_t	cs		= compute_checksum(_log.kv->key.data, _log.kv->key.length, _log.kv->value.data, _log.kv->value.length);
			mJournalFile.write(&cs, sizeof(cs));
		}

		mAppendLogs.push_back(std::move(_log));
	}


	bool empty() const noexcept { return mAppendLogs.empty(); }

	void clear()
	{
		for (auto& log : mAppendLogs)
		{
			auto&	kv	= *log.kv;
			log.kv->state	= kv_state::keep;
		}
		mAppendLogs.clear();
		mJournalFile.remove();
	}

	auto begin() noexcept { return mAppendLogs.begin(); }
	auto end() noexcept { return mAppendLogs.end(); }

private:
	std::vector<log_data>			mAppendLogs;
};


INARIKVS_NAMESPACE_END
#endif
