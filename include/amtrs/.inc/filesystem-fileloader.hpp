/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__filesystem__fileloader__hpp
#define	__libamtrs__filesystem__fileloader__hpp
AMTRS_FILESYSTEM_NAMESPACE_BEGIN

class	fileloader
		: public cascadevfs
{
public:
	static ref<fileloader> get_instance()
	{
		if (!sInstance)
		{
			sInstance	= new fileloader();
		}
		return	sInstance;
	}

	~fileloader()
	{
		if (sInstance == this)
		{
			sInstance	= nullptr;
		}
	}
	void push_front(vfsptr _fs) override
	{
		mMaster->push_front(std::move(_fs));
	}

	void push_back(vfsptr _fs) override
	{
		mMaster->push_back(std::move(_fs));
	}

	void erase(vfs* _fs) override
	{
		mMaster->erase(std::move(_fs));
	}



protected:
	virtual io::vstreamif  on_open      (path_type _path, std::error_code& _ec)       override { return mMaster->open(_path, _ec); }
	virtual file_status    on_status    (path_type _path, std::error_code& _ec) const override { return mMaster->status(_path, _ec); }
	virtual std::uintmax_t on_file_size (path_type _path, std::error_code& _ec) const override { return mMaster->file_size(_path, _ec); }
	virtual bool		   on_remove    (path_type _path, std::error_code& _ec) const override { return mMaster->remove(_path, _ec); }
	virtual std::uintmax_t on_remove_all(path_type _path, std::error_code& _ec) const override { return mMaster->remove_all(_path, _ec); }


	fileloader()
		: mMaster(cascadevfs::create())
	{}

	static inline	fileloader*	sInstance	= nullptr;
	ref<cascadevfs>	mMaster;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
