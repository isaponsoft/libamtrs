/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__crypto__sha256__hpp
#define	__libamtrs__crypto__sha256__hpp
#include "def.hpp"
AMTRS_CRYPTO_NAMESPACE_BEGIN
namespace sha256_impl {

// https://ja.wikipedia.org/wiki/SHA-2
class	sha256_imp
{
public:
	static constexpr std::size_t	chunk_size	= 64;

	void update(unsigned char* _data, std::size_t _size)
	{
		std::size_t	dataSize	= _size;
		auto		cursor		= _data;

		// 端数バッファにデータがあれば、端数バッファの空き部分へデータを追加
		if (m_chunkSize > 0)
		{
			std::size_t	freeSize	= chunk_size - m_chunkSize;	// 端数バッファの空きサイズ
			std::size_t	copySize	= freeSize < dataSize ? freeSize : dataSize;
			std::memcpy(m_chunk + m_chunkSize, cursor, copySize);
			cursor		+= copySize;
			m_chunkSize += copySize;
			if (m_chunkSize == chunk_size)
			{
				// 端数バッファがたまった！
				push_chunk(m_chunk);
				m_chunkSize	= 0;
			}
		}

		// チャンク単位で処理
		while (dataSize >= chunk_size)
		{
			push_chunk(cursor);
			cursor		+= chunk_size;
			dataSize	-= chunk_size;
		}

		// 64文字に満たない部分は端数バッファへ送っておく
		std::memcpy(m_chunk + m_chunkSize, cursor, dataSize);
		m_chunkSize += dataSize;
	}

	template<class T>
	void finalize(T _destinate)
	{
		// １チャンクに満たないデータ
		for (uint32_t i = 0; i < m_chunkSize; ++i)
 		{
			W[i/4]	|= m_chunk[i]<<(24-(i*8));
		}
		m_dataSize			+= m_chunkSize;

		// 終端ビット
		W[m_chunkSize/4]	|= 0x80000000 >> ((m_chunkSize % 4) * 8);
		m_chunkSize			= (m_chunkSize + 1 + 3) & ~(4-1);

		if (m_chunkSize > 56)
		{
			// データサイズを格納する空きが無いなら先にハッシュをフラッシュする。
			compute_intermediate_hash();
		}
		else
		{
			for (int i = m_chunkSize/4; i < 14; ++i)
			{
				W[i] 	= 0;
			}
		}

		// データサイズ
		uint64_t	totalHashSize	= m_dataSize * 8;
		W[14]	= static_cast<uint32_t>(totalHashSize >> 32);
		W[15]	= static_cast<uint32_t>(totalHashSize);	
		compute_intermediate_hash();

		// 生成したハッシュを出力
		auto	out = cursor::make_put(_destinate);
		for (int i = 0; i < 32; ++i)
		{
			out.put(H[i/4] >> ((3 - (i % 4)) * 8));
		}
	}

private:
	// Hash values
	uint32_t		H[8]		= { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
	uint32_t		W[64] 		= { 0 };
	uint64_t		m_dataSize	= 0;

	// update で 64bytes に満たなかったデータを一時的に蓄えておくバッファ
	unsigned char	m_chunk[64];
	std::size_t		m_chunkSize	= 0;



	template<class X, class Y, class Z> static constexpr X Ch (X _x, Y _y, Z _z) { return (_x & _y) ^ (~_x & _z);             }
	template<class X, class Y, class Z> static constexpr X Maj(X _x, Y _y, Z _z) { return (_x & _y) ^ ( _x & _z) ^ (_y & _z); }

	template<class L, class B> static constexpr L SHR (B _bit, L _l) { return _l >> _bit; }
	template<class L, class B> static constexpr L ROTR(B _bit, L _l) { return SHR(_bit, _l) | (_l << (32 - _bit)); }
	template<class X> static constexpr X S256_0(X _x) { return ROTR(2, _x) ^ ROTR(13, _x) ^ ROTR(22, _x); }
	template<class X> static constexpr X S256_1(X _x) { return ROTR(6, _x) ^ ROTR(11, _x) ^ ROTR(25, _x); }
	template<class X> static constexpr X S_0_256(X _x) { return ROTR( 7, _x) ^ ROTR(18, _x) ^ SHR( 3, _x); }
	template<class X> static constexpr X S_1_256(X _x) { return ROTR(17, _x) ^ ROTR(19, _x) ^ SHR(10, _x); }

	template<class T>
	void ROUND(T _dest[], int _round, int n, int a, int b, int c, int d, int e, int f, int g, int h)
	{
		static constexpr uint32_t K[64] = 
		{
			0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
			0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
			0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
			0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
			0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
			0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
			0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
			0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
		};

		int		tmp	= _dest[h] + S256_1(_dest[e]) + Ch(_dest[e], _dest[f], _dest[g]) + K[_round + n] + W[_round + n];
		_dest[d]	= tmp + _dest[d];
		_dest[h]	= tmp + S256_0(_dest[a]) + Maj(_dest[a], _dest[b], _dest[c]);
	}


	//!	@brief	チャンクを計算します。
	//!	※SHA-2 は 64文字 = １チャンク
	void push_chunk(unsigned char* _chunk)
	{
		for (uint32_t t = 0, i = 0; i < chunk_size; ++t, i += 4)
		{
			W[t]	|= (_chunk[i]<<24) | (_chunk[i+1]<<16) | (_chunk[i+2]<<8) | (_chunk[i+3]);
		}
		m_dataSize	+= chunk_size;
		compute_intermediate_hash();
	}


	void compute_intermediate_hash()
	{
		uint32_t	w[8];
		std::memcpy(w, H, 32);

		// Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule 
		for (int i = 16; i < 64; ++i)
		{
			W[i]	= S_1_256(W[i-2]) + W[i-7] + S_0_256(W[i-15]) + W[i-16];
		}

		for (int round = 0; round < 64; round += 8)
		{
			ROUND(w, round, 0,0,1,2,3,4,5,6,7);
			ROUND(w, round, 1,7,0,1,2,3,4,5,6);
			ROUND(w, round, 2,6,7,0,1,2,3,4,5);
			ROUND(w, round, 3,5,6,7,0,1,2,3,4);
			ROUND(w, round, 4,4,5,6,7,0,1,2,3);
			ROUND(w, round, 5,3,4,5,6,7,0,1,2);
			ROUND(w, round, 6,2,3,4,5,6,7,0,1);
			ROUND(w, round, 7,1,2,3,4,5,6,7,0);
		}

		H[0] += w[0];
		H[1] += w[1];
		H[2] += w[2];
		H[3] += w[3];
		H[4] += w[4];
		H[5] += w[5];
		H[6] += w[6];
		H[7] += w[7];
		std::memset(W, 0, sizeof(W));
	}
};

} // namespace sha256_impl


inline void sha256(void* _sha_buffer, const void* _data, std::size_t _size, bool _bin_mode = false)
{
	unsigned char	hash[32];
	sha256_impl::sha256_imp	s;
	s.update((unsigned char*)_data, _size);
	s.finalize(hash);

	if (!_bin_mode)
	{
		// 文字列として出力する
		int8_t*		p	= (int8_t*)_sha_buffer;
		for (std::size_t i = 0; i < 32; ++i)
		{
			int8_t	h	= (hash[i] >> 4) & 0x0f;
			int8_t	l	= (hash[i]     ) & 0x0f;
			h	= (h < 10) ? ('0' + h) : ('a' + h - 10);
			l	= (l < 10) ? ('0' + l) : ('a' + l - 10);
			*p++ = h;
			*p++ = l;
		}
	}
	else
	{
		// バイナリ列として出力する
		std::memcpy(_sha_buffer, hash, 32);
	}
}


AMTRS_CRYPTO_NAMESPACE_END
#endif

