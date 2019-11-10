#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <iomanip> 
#include <random>

#include <inarikvs/inarikvs.hpp>
#include <inarikvs/blocklist.hpp>
#include <inarikvs/kv.hpp>
#include <inarikvs/kvlist.hpp>
#include <inarikvs/hash.hpp>
#include <inarikvs/value.hpp>
INARIKVS_NAMESPACE_BEGIN

struct	indexdata
{
	using	pos_type	= uint64_t;
	using	hash_type	= uint64_t;

	// 42bit
	uint64_t		key		: 63;
	uint64_t		type_h	:  1;		//!< 種類の上位1ビット

	uint64_t		value	: 21;		//!< 種類の上位1ビット
	uint64_t		pos		: 40;		//!< 種類の上位1ビット
	uint64_t		type_l	:  3;		//!< 種類の上位1ビット


	constexpr uint32_t key_length() const noexcept
	{
		return	hash_util{}.compute_length(key);
	}

	constexpr void key_hash(uint64_t _hash) noexcept
	{
		key	= _hash;
	}

	constexpr uint64_t value_size() const noexcept
	{
		return	key;
	}

	constexpr void value_size(uint32_t _size) noexcept
	{
		value	= _size;
	}

	constexpr uint8_t type() const noexcept
	{
		return	(type_h << 2) | type_l;
	}

	constexpr void type(uint8_t _type) noexcept
	{
		type_h	= (_type >> 2) & 0x01;
		type_l	= (_type >> 0) & 0x03;
	}

};
static_assert(sizeof(indexdata) == 0x10);




template<class IO, std::size_t DiskClusterSize = 4096>
class	index_storage
{
public:
	static constexpr std::size_t	disk_cluster_size	= DiskClusterSize;

	using	io_type		IO;

	template<class FileIO>
	index_storage(FileIO&& _index)
		: mIndexFile(std::forward<Index>(_index))
	{}

	// ========================================================================
	//! インデックスの更新キューを追加します。
	// ------------------------------------------------------------------------
	//! このメソッドで追加しただけでは書き込みは行われません。
	//! sync()を使うことで実際の書き込みを行います。
	// ------------------------------------------------------------------------
	void add(int64_t _indexPos, const kv& _idb)
	{
		int64_t	cluster	= _indexPos / disk_cluster_size;
		int64_t	offset	= _indexPos % disk_cluster_size;
		auto	res		= mWriteCache.emplace(cluster, index_write_cache{});
		if (res.second)
		{
			_file.read(cluster * disk_cluster_size, res.first->second.buff, disk_cluster_size);
			res.first->second.size	= 0;
		}
		std::memcpy(res.first->second.buff + offset, &_idb, sizeof(keyfile_data));
		res.first->second.size	= std::max<std::size_t>(res.first->second.size, offset + sizeof(keyfile_data));
	}

	// ========================================================================
	//! キューの内容を書き出します。
	// ------------------------------------------------------------------------
	void sync()
	{
		
	}

	// ========================================================================
	//! 書き込み用システムコールを抑えるためにクラスタ単位でデータを書き込む
	//! ようにするキャッシュです。
	// ------------------------------------------------------------------------
	struct	index_write_cache
	{
		char		buff[disk_cluster_size];
		std::size_t	size;
	};

	std::map<int64_t, index_write_cache>	mWriteCache;

	io_type									mIndexFile;
};


/*
template<std::size_t DiskClusterSize = 4096>
class	keyweiter
{
public:
	static constexpr std::size_t	disk_cluster_size	= DiskClusterSize;

	
	void set_index_write(file& _file, int64_t _indexPos, const keyfile_data& _idb)
	{
		int64_t	cluster	= _indexPos / disk_cluster_size;
		int64_t	offset	= _indexPos % disk_cluster_size;
		auto	res		= mIndexWriteCache.emplace(cluster, index_write_cache{});
		if (res.second)
		{
			_file.read(cluster * disk_cluster_size, res.first->second.buff, disk_cluster_size);
			res.first->second.size	= 0;
		}
		std::memcpy(res.first->second.buff + offset, &_idb, sizeof(keyfile_data));
		res.first->second.size	= std::max<std::size_t>(res.first->second.size, offset + sizeof(keyfile_data));
	}

	void flush(file& _file)
	{
		for (const auto& wc : mIndexWriteCache)
		{
			auto		cluster	= wc.first;
			const char*	buff	= wc.second.buff;
			_file.write(cluster * disk_cluster_size, buff, wc.second.size);
		}
		mIndexWriteCache.clear();
	}

private:
	// インデックスデータをまとめてディスクに書き出すためのライトキャッシュ
	struct	index_write_cache
	{
		char		buff[disk_cluster_size];
		std::size_t	size;
	};

	std::map<int64_t, index_write_cache>	mIndexWriteCache;
};



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
*/




INARIKVS_NAMESPACE_END
using namespace inari;



int main(int _argc, char** _args)
{
	indexdata	idx;






	return	0;
}
