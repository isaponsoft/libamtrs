#ifndef	__inarikvs__database__hpp
#define	__inarikvs__database__hpp
#include  <iomanip>
#include <map>
#include <set>
#include "hashkey.hpp"
#include "keyweiter.hpp"
#include "key_group.hpp"
#include "kv_data.hpp"
#include "log_data.hpp"
#include "logger.hpp"
#include "tree.hpp"
INARIKVS_NAMESPACE_BEGIN

class	database
{
public:
	database(const database&) = delete;
	database(database&&) = default;


	// ========================================================================
	//! ジャーナルからリカバリします
	// ------------------------------------------------------------------------
	void recover()
	{
		mLogger.load_recover([this](kv_data* _kv) -> kv_data*
		{
			return	get(std::string_view(_kv->key));
		});
		flush();
	}


	// ========================================================================
	//! 指定したファイル名のデータベースをロードします。
	// ------------------------------------------------------------------------
	//! \param _dbfile
	//!		.idx と .val のセットととなるファイルの拡張子を除くファイル名を
	//!		渡します。
	// ------------------------------------------------------------------------
	static database load(const std::string& _dbfile)
	{
		database	db(_dbfile);
		db.load_index();
		return	db;
	}


	bool del(std::string_view _key)
	{
		hashkey		idx	= _key;
		auto		it	= mKeyMap.find(idx);
		if (it != mKeyMap.end())
		{
			auto	kv	= it->second.del(mValueFile, _key);
			if (kv)
			{
				mLogger.add({log_type::del, std::move(kv)});
				if (it->second.empty())
				{
					mKeyMap.erase(idx);
				}
				return	true;
			}
		}
		return	false;
	}



	result set(std::string_view _key, std::string_view _value)
	{
		using	vt	= typename tree::basic_unordered_map<hashkey, key_group>::value_type;

		hashkey		idx	= _key;
		auto		r	= mKeyMap.insert(vt{idx, key_group{}});
		auto&		kg	= r.first->second;
		kv_data*	kv	= kg.find(mValueFile, _key);
		if (!kv)
		{
			// 新規追加
			kv	= kg.add(new kv_data());
			kv->key.create(_key);
			kv->hash(idx.hash);
			kv->key.hash	= idx.hash;
			kv->value.create(_value);
			kv->state	= kv_state::created;
			mLogger.add({log_type::set, kv});
			return	result::create;
		}
		else
		{
			// 上書き
			kv->load(mValueFile);
			if (kv->value == _value)
			{
				// 値の変更がない場合は上書を無視する(ディスクへの書き込みをなるべく少なくする)
				return	result::nochange;
			}
			kv->value.create(_value);
			if (kv->state == kv_state::keep)
			{
				kv->state	= kv_state::update;
				mLogger.add({log_type::set, kv});
			}
			return	result::change;
		}
	}


	kv_data* get(std::string_view _key)
	{
		auto		it	= mKeyMap.find(_key);
		if (it == mKeyMap.end())
		{
			// 見つからなかった
			return	nullptr;
		}

		auto*	kv	= it->second.find(mValueFile, _key);
		kv->load(mValueFile);
		return	kv;
	}



	void flush()
	{
		int64_t				newIndexPos	= mIndexFile.size();
		keyweiter<4096>		kwQueue;						// キーデータを書き込むための書き込みキュー

		// 値を書き込む
		for (const auto& log : mLogger)
		{
			auto&	kv	= *log.kv;

			// 削除、アップデートの場合は現在のインデックスを削除する。
			if (log.type == log_type::del || kv.state == kv_state::update)
			{
				if (!kv.empty())
				{
					mFreeblockQueue.push_back({kv.filepos(), kv.filesize()});
					kv.filepos(keyfile_data::npos);
					kwQueue.set_index_write(mIndexFile, kv.index_pos, kv);
					mFreeIndicesQueue.emplace_back(kv.index_pos);
					kv.index_pos	= -1;
				}
			}

			// 更新されたデータを追記する。
			if (kv.state == kv_state::created || kv.state == kv_state::update)
			{
				add_file_block(mValueFile, kv);
				if (!mFreeIndecies.empty())
				{
					// 空いているインデックスがあれば再利用する
					kv.index_pos	= mFreeIndecies.front();
					mFreeIndecies.pop_front();
				}
				else
				{
					// 空いているものがないのでファイルの末尾に追加
					kv.index_pos		= newIndexPos;
					newIndexPos	+= sizeof(keyfile_data);
				}
				kwQueue.set_index_write(mIndexFile, kv.index_pos, kv);
			}

			log.kv->state	= kv_state::keep;
		}

		// インデックス書き込みを行う
		kwQueue.flush(mIndexFile);

		if (!mFreeIndicesQueue.empty())
		{
			for (auto idx : mFreeIndicesQueue)
			{
				mFreeIndecies.push_back(idx);
			}
			mFreeIndicesQueue.clear();
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
			mValueFile.sync();		// 先にvalファイルから書き込むこと！そうしないとインデックスの先が無いなんてことも起こりうる。
			mIndexFile.sync();		// 
			mLogger.clear();
		}
	}


	// ========================================================================
	//! キーを列挙し、コールバックを呼び出します。
	// ------------------------------------------------------------------------
	template<class Callback>
	kv_data* keys(Callback&& _callback)
	{
		for (auto& kg : mKeyMap)
		{
			for (kv_data* kv : kg.second)
			{
				kv->load(mValueFile);
				if (_callback(kv))
				{
					return	kv;
				}
			}
		}
		return	nullptr;
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


private:
	database(std::string&& _dbfile)
		: mStem(_dbfile)
	{}

	database(const std::string& _dbfile = "inari")
		: mStem(_dbfile)
	{}


	void add_file_block(file& _file, kv_data& _kv)
	{
		auto	vs	= _kv.key.length + _kv.value.length;
		_kv.key_length(_kv.key.length);
		_kv.val_length(_kv.value.length);
		auto	fs	= _kv.filesize();
		int64_t	pos	= mFreeBlock.get(fs);
		if (pos == -1)
		{
			pos	= _file.size();
		}
		_file.write(pos, _kv.key.data, _kv.key.length);
		_file.write(pos + _kv.key.length, _kv.value.data, _kv.value.length);
		if (vs != fs)
		{
			static constexpr char	buf[4]	= {'*', '*', '*', '*'};
			_file.write(pos + _kv.key.length + _kv.value.length, buf, fs - vs);
		}
		_kv.filepos(pos);
	}


	void load_index()
	{
		mValueFile.open(get_value_filename());
		auto	totalSize	= mValueFile.size();
		mValueFile.close();

		blocklist	kb;
		if (mIndexFile.open(get_index_filename(), false))
		{
			// ファイルをある程度一括で読みながら処理する。
			std::vector<keyfile_data>	databuff(8192);	// てけとー
			for (int64_t fpos = 0; fpos < mIndexFile.size(); )
			{
				auto				pos			= mIndexFile.position();
				std::size_t			readSize	= mIndexFile.read(databuff.data(), databuff.size() + sizeof(keyfile_data));
				auto				endPos		= pos + readSize;
				keyfile_data*		ifbp		= databuff.data();
				for (; pos < endPos; pos += sizeof(keyfile_data), ++ifbp)
				{
					keyfile_data&	ifb(*ifbp);
					if (ifb.empty())
					{

						// 未使用のインデックスデータ
						mFreeIndecies.push_back(pos);
					}
					else
					{
						amtrs::ref<kv_data>	kv		= new kv_data(pos, ifb);
						auto				r		= mKeyMap.insert({kv->hashkey(), key_group{}});
						auto&				kg		= r.first->second;
						kv->index_pos	= pos;
						kg.add(kv);
						kb.add(kv->filepos(),   kv->filesize());
					}
				}
				fpos	= pos;
			}
			mIndexFile.close();
		}

		#if	0
		std::cout << "----------------------" << std::endl;
		std::cout << std::hex;
		for (auto* b = kb.mFirst.get() ; b ; b = b->next)
		{
			std::cout << b->pos << " : " << b->size << std::endl;
		}
		#endif


		mFreeBlock	= kb.reverse(totalSize);


		#if	0
		std::cout << "----------------------" << totalSize << std::endl;
		std::cout << std::hex;
		for (auto* b = mFreeBlock.mFirst.get() ; b ; b = b->next)
		{
			std::cout << b->pos << " : " << b->size << std::endl;
		}
		#endif

		mIndexFile.open(get_index_filename());
		mValueFile.open(get_value_filename());

		mLogger.create_journal(get_journal_filename());
	}


	std::string get_journal_filename() const
	{
		return	mStem + ".jnl";
	}


	std::string get_index_filename() const
	{
		return	mStem + ".key";
	}


	std::string get_value_filename() const
	{
		return	mStem + ".val";
	}


	file											mIndexFile;
	file											mValueFile;
	tree::basic_unordered_map<hashkey, key_group>	mKeyMap;	
	blocklist										mFreeBlock;
	std::vector<std::pair<int64_t, uint32_t>>		mFreeblockQueue;
	std::list<int64_t>								mFreeIndecies;
	std::vector<int64_t>							mFreeIndicesQueue;
	std::string										mStem;
	logger											mLogger;
};

INARIKVS_NAMESPACE_END
#endif
