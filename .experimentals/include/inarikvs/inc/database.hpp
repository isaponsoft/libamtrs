/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__inarikvs__database__hpp
#define	__inarikvs__database__hpp
#include "table.hpp"
INARIKVS_NAMESPACE_BEGIN



// ************************************************************************
//! get/setを行う単純なKVSを提供します。
// ------------------------------------------------------------------------
//!
// ------------------------------------------------------------------------
template<class Storage>
class	database
{
public:
	using	storage_type	= Storage;

	database() = default;
	database(const database&) = delete;
	database(database&&) = default;


	// ========================================================================
	//! ジャーナルからリカバリします
	// ------------------------------------------------------------------------
	void recover()
	{
		mLogger.load_recover([this](kv_data* _kv) -> kv_data*
		{
			return	get(_kv->key());
		});
		flush();
	}



	bool del(std::string_view _key) { return mGlobalTable.del(_key); }
	result set(std::string_view _key, std::string_view _value) { return mGlobalTable.set(_key, _value); }
	kv_data* get(std::string_view _key) { return mGlobalTable.get(_key); }

	void start()
	{
		mLogger.create_journal(get_journal_filename());
	}

	void flush()
	{
		int64_t					newIndexPos	= mIndexFile.size();
		keyweiter<4096>			kwQueue;						// キーデータを書き込むための書き込みキュー


		std::vector<int64_t>	freeSegmenetQueue;
		freeSegmenetQueue.reserve(mLogger.size());


		mLogger.sync();

		// 値を書き込む
		for (const auto& log : mLogger)
		{
			auto&	kv	= *log.kv;

			// 削除、アップデートの場合は現在のインデックスを削除する。
			if (log.type == log_type::del || kv.state() == kv_state::update)
			{
				if (kv.saved())
				{
					mFreeblockQueue.push_back({kv.saved_status().filepos(), kv.saved_status().filesize()});
					kv.unsave();
					kwQueue.set_index_write(mIndexFile, kv.index_pos(), kv.saved_status());
					freeSegmenetQueue.emplace_back(kv.index_pos());
					kv.clear_index_pos();
				}
			}

			// 更新されたデータを追記する。
			if (kv.state() == kv_state::created || kv.state() == kv_state::update)
			{
				add_file_block(mValueFile, kv);
				if (!mFreeSegments.empty())
				{
					// 空いているインデックスがあれば再利用する
					kv.set_index_pos(mFreeSegments.front());
					mFreeSegments.pop_front();
				}
				else
				{
					// 空いているものがないのでファイルの末尾に追加
					kv.set_index_pos(newIndexPos);
					newIndexPos	+= sizeof(segment_desc);
				}
				kwQueue.set_index_write(mIndexFile, kv.index_pos(), kv.saved_status());
			}

			log.kv->set_state(kv_state::keep);
		}

		// インデックス書き込みを行う
		kwQueue.flush(mIndexFile);

		if (!freeSegmenetQueue.empty())
		{
			for (auto idx : freeSegmenetQueue)
			{
				mFreeSegments.push_back(idx);
			}
			freeSegmenetQueue.clear();
		}

		if (!mFreeblockQueue.empty())
		{
			for (auto& f : mFreeblockQueue)
			{
				mFreeBlock.add(f.first, f.second);
			}
			mFreeblockQueue.clear();
		}

		if (!mLogger.empty())
		{
			mValueFile.sync();		// 先にvalファイルから同期する！
			mIndexFile.sync();		// そうしないとインデックスの先が無いなんてことも起こりうる。
			mLogger.clear();		// ログが残っていればなんとかなるので、ログの削除は最後。
		}
	}


	// ========================================================================
	//! キーを列挙し、コールバックを呼び出します。
	// ------------------------------------------------------------------------
	template<class Callback>
	kv_data* keys(Callback&& _callback)
	{
		return	mGlobalTable.keys(std::forward<Callback>(_callback));
	}


	// ========================================================================
	//! valueファイルのブロックを列挙し、コールバックを呼び出します。
	// ------------------------------------------------------------------------
	template<class CallbackT>
	void blocks(CallbackT&& _callback)
	{
		auto	totalSize	= mValueFile.size();
		mFreeBlock.each(totalSize, std::forward<CallbackT>(_callback));
	}


	void load(std::string const& _dbfile)
	{
		mStem	= _dbfile;

		mValueFile.open(get_value_filename(), false);
		auto	totalSize	= mValueFile.size();
		mValueFile.close();

		blocklist	kb;
		if (mIndexFile.open(get_segment_filename(), false))
		{
			// ファイルをある程度一括で読みながら処理する。
			blocklist	lkb;
			std::vector<segment_desc>	databuff(65536);	// てけとー
			for (int64_t fpos = 0; fpos < mIndexFile.size(); )
			{
				uint64_t			pos			= mIndexFile.position();
				std::size_t			readSize	= mIndexFile.read(databuff.data(), databuff.size() + sizeof(segment_desc));
				uint64_t			endPos		= pos + readSize;
				segment_desc*		ifbp		= databuff.data();
				for (; pos < endPos; pos += sizeof(segment_desc), ++ifbp)
				{
					segment_desc&	ifb(*ifbp);
					if (ifb.empty())
					{

						// 未使用のインデックスデータ
						mFreeSegments.push_back(pos);
					}
					else
					{
						auto	kv	= mGlobalTable.restore(ifb, pos);
						lkb.add(kv->saved_status().filepos(),   kv->saved_status().filesize());
					}
				}
				fpos	= pos;
			}
			mIndexFile.close();
			lkb.each_block([&](auto pos, auto size)
			{
				kb.add(pos, size);
			});
		}
		mFreeBlock	= kb.reverse(totalSize);

		mIndexFile.open(get_segment_filename(), true);
		mValueFile.open(get_value_filename(), true);


		mGlobalTable.set_storage(&mIndexFile, &mValueFile, &mLogger);
	}

private:
	database(std::string&& _dbfile)
		: mStem(_dbfile)
	{}


	void add_file_block(storage_type& _file, kv_data& _kv)
	{
		auto	vs	= _kv.key().size() + _kv.value().size();
		auto	fs	= _kv.value_file_size();
		int64_t	pos	= mFreeBlock.get(fs);
		if (pos == -1)
		{
			pos	= _file.size();
		}
		_file.write(pos, _kv.key());
		_file.write(_kv.value());
		if (vs != fs)
		{
			static constexpr char	buf[4]	= {'*', '*', '*', '*'};
			_file.write(buf, fs - vs);
		}
		_kv.save(pos);
	}

	// ========================================================================
	//! 値ファイルの指定位置から指定サイズだけ読み取ります。
	// ------------------------------------------------------------------------
	void read_value(void* _buffer, int64_t _pos, size_t _size)
	{
		mValueFile.read(_pos, void* _buffer, _size);
	}





	std::string get_journal_filename() const
	{
		return	mStem + ".jnl";
	}


	std::string get_segment_filename() const
	{
		return	mStem + ".key";
	}


	std::string get_value_filename() const
	{
		return	mStem + ".val";
	}


	storage_type									mIndexFile;
	storage_type									mValueFile;
	blocklist										mFreeBlock;
	std::vector<std::pair<int64_t, uint32_t>>		mFreeblockQueue;
	table<storage_type>								mGlobalTable;
	std::list<int64_t>								mFreeSegments;			//!< 未使用のセグメントディスクリプタのインデックス
	std::string										mStem;
	logger<storage_type>							mLogger;
};

INARIKVS_NAMESPACE_END
#endif
