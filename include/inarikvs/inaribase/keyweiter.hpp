#ifndef	__inarikvs__keyweiter__hpp
#define	__inarikvs__keyweiter__hpp
#include "kv_data.hpp"
INARIKVS_NAMESPACE_BEGIN


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


INARIKVS_NAMESPACE_END
#endif
