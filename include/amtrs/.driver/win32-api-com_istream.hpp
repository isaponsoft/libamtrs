/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__os__win32__com_istream__hpp
#define	__libamtrs__os__win32__com_istream__hpp
#include "win32-api-windows.hpp"
#include <atomic>
#include <iostream>
#include <ObjIdlbase.h>
AMTRS_OS_WIN32_NAMESPACE_BEGIN

// ============================================================================
//! amtrs::ios::rstream を用いて GDI+ Bitmap を扱うためのラッパー。
// ----------------------------------------------------------------------------
template<class RStreamT>
class	com_istream_wrapper
		: public ::IStream
{
public:
	using	stream_type	= RStreamT;

	com_istream_wrapper(const com_istream_wrapper &) = delete;
	com_istream_wrapper & operator=(const com_istream_wrapper &) = delete;

	com_istream_wrapper(stream_type& in)
		: mRstream{in}
		, mRefCount{1}
	{
		auto	pos	= mRstream.tellg();
		in.seekg(0, std::ios_base::end);
		mStreamSize	= (std::size_t)mRstream.tellg();
		in.seekg(0, std::ios_base::beg);
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID _iid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *_ppv)
	{
		if (!_ppv)
		{
			return E_INVALIDARG;
		}
		if (_iid == __uuidof(IUnknown) || _iid == __uuidof(IStream) || _iid == __uuidof(ISequentialStream))
		{
			*_ppv = static_cast<IStream *>(this);
			this->AddRef();
			return S_OK;
		}
		return	E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef() override
	{
		return	++mRefCount;
	}

	virtual ULONG STDMETHODCALLTYPE Release() override
	{
		if (--mRefCount == 0)
		{
			delete	this;
			return	0;
		}
		return	mRefCount.load();
	}

	virtual HRESULT STDMETHODCALLTYPE Read(void* const _pv, const ULONG _cb, ULONG * const _pcbRead) override
	{
		if (!_pv)
		{
			return	STG_E_INVALIDPOINTER;
		}

		ULONG	read	= 0;
		try
		{
			this->mRstream.read(static_cast<char *>(_pv), _cb);
			read = static_cast<ULONG>(this->mRstream.gcount());
		}
		catch (const std::ios_base::failure &)
		{
			return	STG_E_ACCESSDENIED;
		}
		catch (const std::bad_alloc &)
		{
			return	E_OUTOFMEMORY;
		}
		catch (const std::exception &)
		{
			return	E_FAIL;
		}
		catch (...)
		{
			return	E_UNEXPECTED;
		}
		if (_pcbRead)
		{
			*_pcbRead	= read;
		}
		return (read < _cb)
			? S_FALSE
			: S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Write(const void*, ULONG, ULONG*) override
	{
		return	E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Seek(const LARGE_INTEGER _dlibMove, const DWORD _dwOrigin, ULARGE_INTEGER * const _plibNewPosition) override
	{
		try
		{
			switch (_dwOrigin)
			{
				case STREAM_SEEK_SET :
				{
					this->mRstream.seekg(static_cast<typename stream_type::pos_type>(_dlibMove.QuadPart), std::ios_base::beg);
					break;
				}

				case STREAM_SEEK_CUR:
				{
					this->mRstream.seekg(static_cast<typename stream_type::off_type>(_dlibMove.QuadPart), std::ios_base::cur);
					break;
				}

				case STREAM_SEEK_END:
				{
					this->mRstream.seekg(static_cast<typename stream_type::off_type>(_dlibMove.QuadPart), std::ios_base::end);
					break;
				}

				default:
				{
					return	STG_E_INVALIDFUNCTION;
				}
			}
			if (_plibNewPosition)
			{
				_plibNewPosition->QuadPart = this->mRstream.tellg();
			}
		}
		catch (const std::ios_base::failure &)
		{
			return	STG_E_ACCESSDENIED;
		}
		catch (const std::bad_alloc &)
		{
			return	E_OUTOFMEMORY;
		}
		catch (const std::exception &)
		{
			return	E_FAIL;
		}
		catch (...)
		{
			return	E_UNEXPECTED;
		}
		return	S_OK;
	}


	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* _pstatstg, DWORD _grfStatFlag) override
	{
		_pstatstg->type					= STGTY_STREAM;
		_pstatstg->cbSize.QuadPart		= mStreamSize;
		_pstatstg->grfLocksSupported	= LOCK_ONLYONCE;
		_pstatstg->clsid				= CLSID_NULL;
		return	S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER) override { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*) override { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD) override { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Revert() override { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) override { return E_NOTIMPL; }
	virtual	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) override { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Clone(IStream**) override { return E_NOTIMPL; }

protected:
	stream_type&			mRstream;
	std::atomic<unsigned>	mRefCount;
	std::size_t				mStreamSize;
};
AMTRS_OS_WIN32_NAMESPACE_END
#endif
