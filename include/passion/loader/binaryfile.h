#ifndef	__passion__loader__binaryfile__h
#define	__passion__loader__binaryfile__h
#ifdef	__cplusplus
#include <algorithm>
#include <string_view>
#include <utility>
extern  "C" {
#endif


/*!
 * 変数へのオフセット値をクラスロード時に計算するためのデータ。
 */
struct	passion_variableoffset
{
	uint32_t	dsize;				/* 動的に算出するサイズ(=intmax_tの数) */
	uint32_t	ssize;				/* 静的に算出できたサイズ */
};


#ifdef	__cplusplus
}
namespace psn {

	struct	variableoffset : passion_variableoffset
	{
		constexpr size_t compute_offset() const noexcept
		{
			return	dsize * sizeof(intmax_t) + ssize;
		}
		constexpr bool operator == (const passion_variableoffset& _b) const noexcept { return dsize == _b.dsize && ssize == _b.ssize; }
		constexpr bool operator != (const passion_variableoffset& _b) const noexcept { return dsize != _b.dsize || ssize != _b.ssize; }
	};
	static_assert(sizeof(variableoffset) == sizeof(passion_variableoffset));

	class	variableoffsettable
	{
	public:
		variableoffsettable() = default;
		variableoffsettable(const variableoffsettable&) = delete;
		variableoffsettable(variableoffsettable&& _b)
			: mSize(_b.mSize)
			, mData(_b.mData)
		{
			_b.mSize	= 0;
			_b.mData	= nullptr;
		}
		~variableoffsettable()
		{
			if (mData) { delete[] mData; }
		}

		size_t add(variableoffset _val)
		{
			size_t		retval	= mSize;
			variableoffset*	data	= new variableoffset[mSize + 1];
			data[mSize]	= std::move(_val);
			if (mData)
			{
				std::copy(begin(), end(), data);
				delete[] mData;
			}
			mData	= data;
			mSize	= retval + 1;
			return	retval;
		}

		const variableoffset& operator [] (size_t _i) const noexcept
		{
			return	mData[_i];
		}

		bool empty() const noexcept { return size() == 0; }
		size_t size() const noexcept { return mSize; }

		const variableoffset* begin() const noexcept { return mData; }
		const variableoffset* end() const noexcept { return mData + mSize; }

	private:
		size_t			mSize	= 0;
		variableoffset*		mData	= nullptr;
	};
}
#endif
#endif
