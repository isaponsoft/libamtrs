/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__crypto__aes__hpp
#define	__libamtrs__crypto__aes__hpp
#include "def.hpp"
AMTRS_CRYPTO_NAMESPACE_BEGIN

template<size_t Bit, class CharT = uint8_t>
class	basic_aes
{
public:
	using	char_type	= CharT;
	using	size_type	= size_t;

	// the Rijndael Algorithm symboles.
	static constexpr size_type	Nb			= 4;
	static constexpr size_type	Nk			= Bit/32;
	static constexpr size_type	Nr			= Nk + 6;

	// traits.
	static constexpr size_type	block_size	= 16;
	static constexpr size_type	key_bit		= Bit;
	static constexpr size_type	key_size	= Nk * 4;

	// 指定されたデータが収まるようにアライメントしたサイズを計算します。
	static constexpr size_type align_size(size_type _size) noexcept
	{
		return	((_size + block_size - 1) / block_size) * block_size;
	}


	constexpr basic_aes(char_type const* _key) noexcept
	{
		// KeyExpantion()
		for (size_type i = 0; i < Nk; ++i)
		{
			mRoundKey[(i * 4) + 0] = _key[(i * 4) + 0];
			mRoundKey[(i * 4) + 1] = _key[(i * 4) + 1];
			mRoundKey[(i * 4) + 2] = _key[(i * 4) + 2];
			mRoundKey[(i * 4) + 3] = _key[(i * 4) + 3];
		}
		for (size_type i = Nk; i < Nb * (Nr + 1); ++i)
		{
			char_type	tmp[4];
			tmp[0]	= mRoundKey[(i-1) * 4 + 0];
			tmp[1]	= mRoundKey[(i-1) * 4 + 1];
			tmp[2]	= mRoundKey[(i-1) * 4 + 2];
			tmp[3]	= mRoundKey[(i-1) * 4 + 3];
		
			if (i % Nk == 0)
			{
				// rotword
				char_type	k = tmp[0];
				tmp[0] = tmp[1];
				tmp[1] = tmp[2];
				tmp[2] = tmp[3];
				tmp[3] = k;
		
				// subword
				tmp[0] = sbox_enc[tmp[0]];
				tmp[1] = sbox_enc[tmp[1]];
				tmp[2] = sbox_enc[tmp[2]];
				tmp[3] = sbox_enc[tmp[3]];

				// rcon
				tmp[0] =  tmp[0] ^ rcon[i/Nk];
			}

			if constexpr (key_bit == 256)
			{
				if (i % Nk == 4)
				{
					// subword
					tmp[0] = sbox_enc[tmp[0]];
					tmp[1] = sbox_enc[tmp[1]];
					tmp[2] = sbox_enc[tmp[2]];
					tmp[3] = sbox_enc[tmp[3]];
				}
			}
			mRoundKey[i * 4 + 0] = mRoundKey[(i - Nk) * 4 + 0] ^ tmp[0];
			mRoundKey[i * 4 + 1] = mRoundKey[(i - Nk) * 4 + 1] ^ tmp[1];
			mRoundKey[i * 4 + 2] = mRoundKey[(i - Nk) * 4 + 2] ^ tmp[2];
			mRoundKey[i * 4 + 3] = mRoundKey[(i - Nk) * 4 + 3] ^ tmp[3];
		}
	}


	constexpr void encrypt_ecb(char_type* _out, const char_type* _in, size_type _size) noexcept
	{
		std::copy(_in, _in + _size, _out);
		enc_chiper((state_t*)_out, mRoundKey);
	}


	constexpr void decrypt_ecb(char_type *_out, const char_type* _in, size_type _size) noexcept
	{
		std::copy(_in, _in + _size, _out);
		dec_chiper((state_t*)_out, mRoundKey);
	}


	//! データを符号化します。
	//! block_size単位で符号化します。データが不足している場合は切り捨てられます。
	constexpr size_t encrypt_cbc(char_type* _out, char_type const* _in, size_type _size, char_type const* _iv) noexcept
	{
		size_type	i	= 0;
		for (; (i+block_size) <= _size; i += block_size)
		{
			xor_words(_out, _in, _iv);
			enc_chiper((state_t*)_out, mRoundKey);
			_iv		=  _out;
			_in		+= block_size;
			_out	+= block_size;
		}
		return	i;
	}


	//! データを復元します。
	//! block_size単位で復元します。データが不足している場合は切り捨てられます。
	constexpr size_t decrypt_cbc(char_type* _out, char_type const* _in, size_type _size, char_type const* _iv) noexcept
	{
		size_type	i	= 0;
		for (; (i+block_size) <= _size; i += block_size)
		{
			memcpy(_out, _in, block_size);
			dec_chiper((state_t*)_out, mRoundKey);
			xor_words(_out, _out, _iv);
			_iv		=  _in;
			_in		+= block_size;
			_out	+= block_size;
		}
		return	i;
	}
	

private:
	using	state_t	= char_type[4][4];


	static constexpr void xor_words(char_type* _work, char_type const* _in, char_type const* _iv) noexcept
	{
		for (size_type i = 0; i < block_size; ++i)
		{
			_work[i] = _in[i] ^ _iv[i];
		}
	}

	
	static constexpr char_type xtime(char_type x) noexcept
	{
		return	(x<<1) ^ (((x>>7) & 1) * 0x1b);
	}


	static constexpr void add_round_key(state_t* _state, char_type const* _round_key, char_type _round) noexcept
	{
		for (size_type i = 0; i < 4; ++i)
		{
			for (size_type j = 0; j < 4; ++j)
			{
				(*_state)[i][j] ^= _round_key[_round * Nb * 4 + i * Nb + j];
			}
		}
	}

	
	static constexpr void enc_chiper(state_t* _state, char_type const* _round_key) noexcept
	{
		add_round_key(_state, _round_key, 0); 
		for (size_type round = 1; round < Nr; ++round)
		{
			enc_sub_bytes(_state);
			enc_shift_rows(_state);
			enc_mix_colmuns(_state);
			add_round_key(_state, _round_key, round);
		}
		enc_sub_bytes(_state);
		enc_shift_rows(_state);
		add_round_key(_state, _round_key, Nr);
	}

	
	static constexpr void enc_sub_bytes(state_t* _state) noexcept
	{
		for (size_type i = 0; i < 4; ++i)
		{
			for (size_type j = 0; j < 4; ++j)
			{
				(*_state)[j][i] = sbox_enc[(*_state)[j][i]];
			}
		}
	}
	

	static constexpr void enc_shift_rows(state_t* _state) noexcept
	{
		char_type t;
		
		t				= (*_state)[0][1];
		(*_state)[0][1] = (*_state)[1][1];
		(*_state)[1][1] = (*_state)[2][1];
		(*_state)[2][1] = (*_state)[3][1];
		(*_state)[3][1] = t;
		
		t				= (*_state)[0][2];
		(*_state)[0][2] = (*_state)[2][2];
		(*_state)[2][2] = t;
		
		t      			= (*_state)[1][2];
		(*_state)[1][2]	= (*_state)[3][2];
		(*_state)[3][2]	= t;
		
		t   		    = (*_state)[0][3];
		(*_state)[0][3] = (*_state)[3][3];
		(*_state)[3][3] = (*_state)[2][3];
		(*_state)[2][3] = (*_state)[1][3];
		(*_state)[1][3] = t;
	}
	

	static constexpr void enc_mix_colmuns(state_t* _state)
	{
		for (size_type i = 0; i < 4; ++i)
		{  
			char_type	t   = (*_state)[i][0];
			char_type	w	= (*_state)[i][0] ^ (*_state)[i][1] ^ (*_state)[i][2] ^ (*_state)[i][3];
			char_type	m;
			m	= (*_state)[i][0] ^ (*_state)[i][1];	m = xtime(m);		(*_state)[i][0] ^= m ^ w;
			m	= (*_state)[i][1] ^ (*_state)[i][2];	m = xtime(m);		(*_state)[i][1] ^= m ^ w;
			m	= (*_state)[i][2] ^ (*_state)[i][3];	m = xtime(m);		(*_state)[i][2] ^= m ^ w;
			m	= (*_state)[i][3] ^ t;					m = xtime(m);		(*_state)[i][3] ^= m ^ w;
		}
	}
	

	static constexpr void dec_chiper(state_t* _state, char_type const* _round_key) noexcept
	{
		add_round_key(_state, _round_key, Nr); 
		for (size_type round = Nr-1; round > 0; --round)
		{
			dec_shift_rows(_state);
			dec_sub_bytes(_state);
			add_round_key(_state, _round_key, round);
			dec_mix_columns(_state);
		}
		dec_shift_rows(_state);
		dec_sub_bytes(_state);
		add_round_key(_state, _round_key, 0);
	}

	static constexpr void dec_mix_columns(state_t* _state) noexcept
	{
		auto	multiply = [](char_type x, char_type y) noexcept
		{
			return	((y    & 1) *                         x    )
				^	((y>>1 & 1) *                   xtime(x)   )
				^	((y>>2 & 1) *             xtime(xtime(x))  )
				^	((y>>3 & 1) *       xtime(xtime(xtime(x))) )
				^	((y>>4 & 1) * xtime(xtime(xtime(xtime(x)))));
		};
		for (size_type i = 0; i < 4; ++i)
		{ 
			char_type	a = (*_state)[i][0];
			char_type	b = (*_state)[i][1];
			char_type	c = (*_state)[i][2];
			char_type	d = (*_state)[i][3];
			(*_state)[i][0] = multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d) ^ multiply(d, 0x09);
			(*_state)[i][1] = multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b) ^ multiply(d, 0x0d);
			(*_state)[i][2] = multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e) ^ multiply(d, 0x0b);
			(*_state)[i][3] = multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09) ^ multiply(d, 0x0e);
		}
	}


	static constexpr void dec_sub_bytes(state_t* _state) noexcept
	{
		for (size_type i = 0; i < 4; ++i)
		{
			for (size_type j = 0; j < 4; ++j)
			{
				(*_state)[j][i]	= sbox_dec[(*_state)[j][i]];
			}
		}
	}
	
	static constexpr void dec_shift_rows(state_t* _state) noexcept
	{
		char_type t;
		
		t				= (*_state)[3][1];
		(*_state)[3][1]	= (*_state)[2][1];
		(*_state)[2][1]	= (*_state)[1][1];
		(*_state)[1][1]	= (*_state)[0][1];
		(*_state)[0][1]	= t;

		t				= (*_state)[0][2];
		(*_state)[0][2]	= (*_state)[2][2];
		(*_state)[2][2]	= t;

		t				= (*_state)[1][2];
		(*_state)[1][2]	= (*_state)[3][2];
		(*_state)[3][2]	= t;

		t				= (*_state)[0][3];
		(*_state)[0][3] = (*_state)[1][3];
		(*_state)[1][3] = (*_state)[2][3];
		(*_state)[2][3] = (*_state)[3][3];
		(*_state)[3][3] = t;
	}


	char_type			mRoundKey[4 * Nb * (Nr + 1)];
	
	static constexpr char_type	sbox_enc[256] =
	{
		0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
		0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
		0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
		0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
		0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
		0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
		0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
		0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
		0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
		0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
		0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
		0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
		0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
	};
	
	static constexpr char_type	sbox_dec[256] =
	{
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
	};
	
	// The round constant word array, rcon[i], contains the values given by 
	// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
	static constexpr char_type	rcon[256] =
	{
		0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
		0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
		0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
		0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
		0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
		0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
		0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
		0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
		0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
		0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
		0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
		0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
		0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
		0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
		0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
		0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d
	};
	
};


using	aes128	= basic_aes<128>;
using	aes192	= basic_aes<192>;
using	aes256	= basic_aes<256>;

#if	0
template<size_t Bit, class C = uint8_t>
struct	encrypto<basic_aes<Bit, C>>
{
	size_t operator (C* _out, C const* _in, size_t _size, C const* _key)
	{
		basic_aes<Bit, C>	enc(_key);
		enc.encrypt_ecb(_out, _in, _size);
		return	_size;
	}

	size_t operator (C* _out, C const* _in, size_t _size, C const* _key, C const* _iv)
	{
		basic_aes<Bit, C>	enc(_key);
		return	enc.encrypt_cbc(_out, _in, _size, _iv);
		return	_size;
	}
};

template<size_t Bit, class C = uint8_t>
struct	decrypto<basic_aes<Bit, C>>
{
	size_t operator (C* _out, C const* _in, size_t _size, C const* _key)
	{
		basic_aes<Bit, C>	enc(_key);
		enc.decrypt_ecb(_out, _in, _size);
		return	_size;
	}

	size_t operator (C* _out, C const* _in, size_t _size, C const* _key, C const* _iv)
	{
		basic_aes<Bit, C>	enc(_key);
		enc.decrypt_cbc(_out, _in, _size, _iv);
		return	_size;
	}
};


encrypto<aes256>{}(_dest, _src, _size, "Helloworld", "aa138a9q99q2");
#endif

AMTRS_CRYPTO_NAMESPACE_END
#endif
