#ifndef	__libamtrs__crypto__md5__hpp
#define	__libamtrs__crypto__md5__hpp
#include <array>
#include <cstring>
#include <utility>
#include "../endian.hpp"
#include "def.hpp"
AMTRS_CRYPTO_NAMESPACE_BEGIN



template<class CharT>
class	basic_md5
{
public:
	using	value_type	= CharT;
	using	size_type	= size_t;


	struct	context
	{
		uint8_t			buff[64]			= {0};
		uint32_t		state[4]			= {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
		uint32_t		count[2]			= {0};
	};


	static constexpr size_type		hash_bit_size	= 128;
	static constexpr size_type		hash_size		= hash_bit_size / 8;

	//! MD5を計算して返します。
	//! 可能ならコンパイル時に計算を行います。
	static constexpr void compute(uint8_t (&_out)[16], uint8_t const* _in, size_type _size) noexcept
	{
		context	ctx;
		update(ctx, _in, _size);
		finish(_out, ctx);
	}

	//! MD5を計算して返します。
	//! 可能ならコンパイル時に計算を行います。
	template<class In>
	static constexpr void compute(uint8_t (&_out)[16], const In& _in) noexcept
	{
		compute(_out,  &*std::begin(_in), std::distance(std::begin(_in), std::end(_in)));
	}

	//! MD5を計算して返します。
	//! 可能ならコンパイル時に計算を行います。
	static constexpr std::array<uint8_t,16> compute(uint8_t const* _in, size_type _size) noexcept
	{
		uint8_t		out[16]		= {0};
		compute(out, _in, _size);
		return	make_array(out, std::make_integer_sequence<int, 16>());
	}


	//! MD5を計算して返します。
	//! 可能ならコンパイル時に計算を行います。
	template<class In>
	static constexpr std::array<uint8_t,16> compute(const In& _in) noexcept
	{
		uint8_t		out[16]		= {0};
		compute(out,  &*std::begin(_in), std::distance(std::begin(_in), std::end(_in)));
		return	make_array(out, std::make_integer_sequence<int, 16>());
	}


	//! 新しいデータを追加します。
	void update(value_type const* _in, size_type _size) noexcept
	{
		uint8_t const*	in		= reinterpret_cast<uint8_t const*>(_in);
		update(mContext, in, _size);
	}


	//! 新しいデータを追加します。
	void finish(value_type* _out = nullptr) noexcept
	{
		finish(mHash, mContext);
		if (_out)
		{
			get(_out);
		}
	}

	//! 生成されたハッシュを取得します。
	void get(value_type* _out) const noexcept
	{
		memcpy(_out, mHash, hash_size);
	}

private:
	template<class T, T... Seq>
	static constexpr std::array<uint8_t,16> make_array(uint8_t (&_in)[16], std::integer_sequence<T, Seq...> _seq) noexcept
	{
		return	{ (_in[Seq])... };
	}


	//! 新しいデータを追加します。
	static constexpr void update(context& _ctx, uint8_t const* _in, size_type _size) noexcept
	{
		size_type		index	= (unsigned int)((_ctx.count[0] >> 3) & 0x3F);


		_ctx.count[0]	+= _size << 3;
		if (_ctx.count[0] < (_size << 3))
		{
			++_ctx.count[1];
		}
		_ctx.count[1]	+= _size >> 29;


		if (size_type partLen = 64 - index; partLen <= _size)
		{
			for (int i = 0; i < partLen; ++i)
			{
				_ctx.buff[index + i] = _in[i];
			}
			transform(_ctx.state, _ctx.buff);

			size_type	i = partLen;
			for (; i + 64 <= _size; i += 64)
			{
				transform(_ctx.state, _in + i);
			}
			index	=  0;
			_in		+= i;
			_size	-= i;
		}
		for (int i = 0; i < _size; ++i)
		{
			_ctx.buff[index + i] = _in[i];
		}
	}


	//! 新しいデータを追加します。
	static constexpr void finish(uint8_t (&_out)[16], context& _ctx) noexcept
	{
		constexpr uint8_t PADDING[64] = {
		  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		uint8_t			tmp[8]	= {0};
		encode(tmp, _ctx.count, 8);

		size_type	idx		= (unsigned int)((_ctx.count[0] >> 3) & 0x3f);
		update(_ctx, PADDING, (idx < 56) ? (56 - idx) : (120 - idx));

		update(_ctx, tmp, 8);
		encode(_out, _ctx.state, 16);
	}


	static constexpr void encode(uint8_t* _out, uint32_t* _in, size_type _size) noexcept
	{
		size_type	count	= _size / sizeof(uint32_t);
		for (size_type i = 0; i < count; ++i)
		{
			_out[i*4 + 0]	= (_in[i]>> 0) & 0xff;
			_out[i*4 + 1]	= (_in[i]>> 8) & 0xff;
			_out[i*4 + 2]	= (_in[i]>>16) & 0xff;
			_out[i*4 + 3]	= (_in[i]>>24) & 0xff;
		}
	}


	static constexpr void transform(uint32_t (&_state)[4], const uint8_t* _block) noexcept
	{
		// エンディアンの関係があるので、一度ネイティブ計算用のバッファに入れておく。
		uint32_t		x[16]	= {0};
		for (int i = 0; i < 64/4; ++i)
		{
			x[i]	= (((uint32_t)_block[i*4  ])      )
					| (((uint32_t)_block[i*4+1]) <<  8)
					| (((uint32_t)_block[i*4+2]) << 16)
					| (((uint32_t)_block[i*4+3]) << 24);
		}

		uint32_t		a = _state[0];
		uint32_t		b = _state[1];
		uint32_t		c = _state[2];
		uint32_t		d = _state[3];

		#define __CURRY_CRYPT_MD5_ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

		// Round 1
		#define __CURRY_CRYPT_MD5_Fx(x, y, z) (((x) & (y)) | ((~x) & (z)))
		#define __CURRY_CRYPT_MD5_F(a, b, c, d, x, s, ac) { \
			(a) += __CURRY_CRYPT_MD5_Fx ((b), (c), (d)) + (x) + (uint32_t)(ac); \
			(a) = __CURRY_CRYPT_MD5_ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		__CURRY_CRYPT_MD5_F(a, b, c, d, x[ 0],  7, 0xd76aa478);	/*  1 */
		__CURRY_CRYPT_MD5_F(d, a, b, c, x[ 1], 12, 0xe8c7b756);	/*  2 */
		__CURRY_CRYPT_MD5_F(c, d, a, b, x[ 2], 17, 0x242070db);	/*  3 */
		__CURRY_CRYPT_MD5_F(b, c, d, a, x[ 3], 22, 0xc1bdceee);	/*  4 */
		__CURRY_CRYPT_MD5_F(a, b, c, d, x[ 4],  7, 0xf57c0faf);	/*  5 */
		__CURRY_CRYPT_MD5_F(d, a, b, c, x[ 5], 12, 0x4787c62a);	/*  6 */
		__CURRY_CRYPT_MD5_F(c, d, a, b, x[ 6], 17, 0xa8304613);	/*  7 */
		__CURRY_CRYPT_MD5_F(b, c, d, a, x[ 7], 22, 0xfd469501);	/*  8 */
		__CURRY_CRYPT_MD5_F(a, b, c, d, x[ 8],  7, 0x698098d8);	/*  9 */
		__CURRY_CRYPT_MD5_F(d, a, b, c, x[ 9], 12, 0x8b44f7af);	/* 10 */
		__CURRY_CRYPT_MD5_F(c, d, a, b, x[10], 17, 0xffff5bb1);	/* 11 */
		__CURRY_CRYPT_MD5_F(b, c, d, a, x[11], 22, 0x895cd7be);	/* 12 */
		__CURRY_CRYPT_MD5_F(a, b, c, d, x[12],  7, 0x6b901122);	/* 13 */
		__CURRY_CRYPT_MD5_F(d, a, b, c, x[13], 12, 0xfd987193);	/* 14 */
		__CURRY_CRYPT_MD5_F(c, d, a, b, x[14], 17, 0xa679438e);	/* 15 */
		__CURRY_CRYPT_MD5_F(b, c, d, a, x[15], 22, 0x49b40821);	/* 16 */
		#undef	__CURRY_CRYPT_MD5_F
		#undef	__CURRY_CRYPT_MD5_Fx


		// Round 2
		#define __CURRY_CRYPT_MD5_Gx(x, y, z) (((x) & (z)) | ((y) & (~z)))
		#define __CURRY_CRYPT_MD5_G(a, b, c, d, x, s, ac) { \
			(a) += __CURRY_CRYPT_MD5_Gx ((b), (c), (d)) + (x) + (uint32_t)(ac); \
			(a) = __CURRY_CRYPT_MD5_ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		__CURRY_CRYPT_MD5_G(a, b, c, d, x[ 1],  5, 0xf61e2562);	/* 17 */
		__CURRY_CRYPT_MD5_G(d, a, b, c, x[ 6],  9, 0xc040b340);	/* 18 */
		__CURRY_CRYPT_MD5_G(c, d, a, b, x[11], 14, 0x265e5a51);	/* 19 */
		__CURRY_CRYPT_MD5_G(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);	/* 20 */
		__CURRY_CRYPT_MD5_G(a, b, c, d, x[ 5],  5, 0xd62f105d);	/* 21 */
		__CURRY_CRYPT_MD5_G(d, a, b, c, x[10],  9,  0x2441453);	/* 22 */
		__CURRY_CRYPT_MD5_G(c, d, a, b, x[15], 14, 0xd8a1e681);	/* 23 */
		__CURRY_CRYPT_MD5_G(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);	/* 24 */
		__CURRY_CRYPT_MD5_G(a, b, c, d, x[ 9],  5, 0x21e1cde6);	/* 25 */
		__CURRY_CRYPT_MD5_G(d, a, b, c, x[14],  9, 0xc33707d6);	/* 26 */
		__CURRY_CRYPT_MD5_G(c, d, a, b, x[ 3], 14, 0xf4d50d87);	/* 27 */
		__CURRY_CRYPT_MD5_G(b, c, d, a, x[ 8], 20, 0x455a14ed);	/* 28 */
		__CURRY_CRYPT_MD5_G(a, b, c, d, x[13],  5, 0xa9e3e905);	/* 29 */
		__CURRY_CRYPT_MD5_G(d, a, b, c, x[ 2],  9, 0xfcefa3f8);	/* 30 */
		__CURRY_CRYPT_MD5_G(c, d, a, b, x[ 7], 14, 0x676f02d9);	/* 31 */
		__CURRY_CRYPT_MD5_G(b, c, d, a, x[12], 20, 0x8d2a4c8a);	/* 32 */
		#undef	__CURRY_CRYPT_MD5_G
		#undef	__CURRY_CRYPT_MD5_Gx


		// Round 3
		#define __CURRY_CRYPT_MD5_Hx(x, y, z) ((x) ^ (y) ^ (z))
		#define __CURRY_CRYPT_MD5_H(a, b, c, d, x, s, ac) { \
			(a) += __CURRY_CRYPT_MD5_Hx ((b), (c), (d)) + (x) + (uint32_t)(ac); \
			(a) = __CURRY_CRYPT_MD5_ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		__CURRY_CRYPT_MD5_H(a, b, c, d, x[ 5],  4, 0xfffa3942);	/* 33 */
		__CURRY_CRYPT_MD5_H(d, a, b, c, x[ 8], 11, 0x8771f681);	/* 34 */
		__CURRY_CRYPT_MD5_H(c, d, a, b, x[11], 16, 0x6d9d6122);	/* 35 */
		__CURRY_CRYPT_MD5_H(b, c, d, a, x[14], 23, 0xfde5380c);	/* 36 */
		__CURRY_CRYPT_MD5_H(a, b, c, d, x[ 1],  4, 0xa4beea44);	/* 37 */
		__CURRY_CRYPT_MD5_H(d, a, b, c, x[ 4], 11, 0x4bdecfa9);	/* 38 */
		__CURRY_CRYPT_MD5_H(c, d, a, b, x[ 7], 16, 0xf6bb4b60);	/* 39 */
		__CURRY_CRYPT_MD5_H(b, c, d, a, x[10], 23, 0xbebfbc70);	/* 40 */
		__CURRY_CRYPT_MD5_H(a, b, c, d, x[13],  4, 0x289b7ec6);	/* 41 */
		__CURRY_CRYPT_MD5_H(d, a, b, c, x[ 0], 11, 0xeaa127fa);	/* 42 */
		__CURRY_CRYPT_MD5_H(c, d, a, b, x[ 3], 16, 0xd4ef3085);	/* 43 */
		__CURRY_CRYPT_MD5_H(b, c, d, a, x[ 6], 23,  0x4881d05);	/* 44 */
		__CURRY_CRYPT_MD5_H(a, b, c, d, x[ 9],  4, 0xd9d4d039);	/* 45 */
		__CURRY_CRYPT_MD5_H(d, a, b, c, x[12], 11, 0xe6db99e5);	/* 46 */
		__CURRY_CRYPT_MD5_H(c, d, a, b, x[15], 16, 0x1fa27cf8);	/* 47 */
		__CURRY_CRYPT_MD5_H(b, c, d, a, x[ 2], 23, 0xc4ac5665);	/* 48 */
		#undef	__CURRY_CRYPT_MD5_H
		#undef	__CURRY_CRYPT_MD5_Hx


		// Round 4
		#define __CURRY_CRYPT_MD5_Ix(x, y, z) ((y) ^ ((x) | (~z)))
		#define __CURRY_CRYPT_MD5_I(a, b, c, d, x, s, ac) { \
			(a) += __CURRY_CRYPT_MD5_Ix ((b), (c), (d)) + (x) + (uint32_t)(ac); \
			(a) = __CURRY_CRYPT_MD5_ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		__CURRY_CRYPT_MD5_I(a, b, c, d, x[ 0],  6, 0xf4292244);	/* 49 */
		__CURRY_CRYPT_MD5_I(d, a, b, c, x[ 7], 10, 0x432aff97);	/* 50 */
		__CURRY_CRYPT_MD5_I(c, d, a, b, x[14], 15, 0xab9423a7);	/* 51 */
		__CURRY_CRYPT_MD5_I(b, c, d, a, x[ 5], 21, 0xfc93a039);	/* 52 */
		__CURRY_CRYPT_MD5_I(a, b, c, d, x[12],  6, 0x655b59c3);	/* 53 */
		__CURRY_CRYPT_MD5_I(d, a, b, c, x[ 3], 10, 0x8f0ccc92);	/* 54 */
		__CURRY_CRYPT_MD5_I(c, d, a, b, x[10], 15, 0xffeff47d);	/* 55 */
		__CURRY_CRYPT_MD5_I(b, c, d, a, x[ 1], 21, 0x85845dd1);	/* 56 */
		__CURRY_CRYPT_MD5_I(a, b, c, d, x[ 8],  6, 0x6fa87e4f);	/* 57 */
		__CURRY_CRYPT_MD5_I(d, a, b, c, x[15], 10, 0xfe2ce6e0);	/* 58 */
		__CURRY_CRYPT_MD5_I(c, d, a, b, x[ 6], 15, 0xa3014314);	/* 59 */
		__CURRY_CRYPT_MD5_I(b, c, d, a, x[13], 21, 0x4e0811a1);	/* 60 */
		__CURRY_CRYPT_MD5_I(a, b, c, d, x[ 4],  6, 0xf7537e82);	/* 61 */
		__CURRY_CRYPT_MD5_I(d, a, b, c, x[11], 10, 0xbd3af235);	/* 62 */
		__CURRY_CRYPT_MD5_I(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);	/* 63 */
		__CURRY_CRYPT_MD5_I(b, c, d, a, x[ 9], 21, 0xeb86d391);	/* 64 */
		#undef	__CURRY_CRYPT_MD5_I
		#undef	__CURRY_CRYPT_MD5_Ix


		_state[0] += a;
		_state[1] += b;
		_state[2] += c;
		_state[3] += d;
	}

	value_type		mHash[hash_size]	= {0};
	context			mContext;
};


using	md5	= basic_md5<uint8_t>;


AMTRS_CRYPTO_NAMESPACE_END
#endif