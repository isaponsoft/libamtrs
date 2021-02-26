/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__token_reader__hpp
#define	__passion__token_reader__hpp
#include "token.hpp"
PASSION_NAMESPACE_BEGIN


// ****************************************************************************
//!	トークン管理
// ****************************************************************************

class	TextReader
		: public source_location
{
public:
	TextReader(const source_location& _locate);
	~TextReader();
	Token read(int skipTokenType = 0);
	void setCommentEnable(bool bEnable);

	static std::string decode_string(std::string_view _text);

private:
	Token readToken();
	unsigned int readCode();
	std::size_t nextCode();

	bool			m_bEnableComment;
	std::size_t		m_nIndex;
};



inline std::string TextReader::decode_string(std::string_view _text)
{
	if (_text.size() > 2 && _text.front() == '"' && _text.back() == '"')
	{
		std::ostringstream	tmp;
		auto				text	= _text.substr(1, _text.size() - 2);
		auto				it		= text.begin();
		for (;;)
		{
			if (it == text.end())
			{
				return	tmp.str();
			}
			auto	c = *it;
			if (c != '\\')
			{
				tmp << (char)c;
				++it;
				continue;
			}

			// エスケープシーケンス
			if (++it == text.end())
			{
				// \ の後ろに何もない
				break;
			}
			c	= *it;
			switch (c)
			{
				case 'n' :	c = '\n'; break;
			}
			tmp << c;
			++it;
		}
	}
	throw	std::logic_error("decode string error.");
}



inline TextReader::TextReader(const source_location& _locate)
	: source_location(_locate)
	, m_bEnableComment(false)
	, m_nIndex(0)
{}


inline TextReader::~TextReader()
{}


inline Token TextReader::read(int skipTokenType)
{
	Token	token	= readToken();
	if (skipTokenType)
	{
		while (token != Token::EOFTOKEN && (token.type()&skipTokenType))
		{
			token	= readToken();
		}
	}
	return	token;
}


inline void TextReader::setCommentEnable(bool bEnable)
{
	m_bEnableComment = bEnable;
}


inline Token TextReader::readToken()
{
	for (;;)
	{
		unsigned int		code = readCode();
		source_location		location(*this);
		if (code == 0x00)
		{
			// ファイルの終端
			return	Token(Token::EOFTOKEN, "", location);
		}
		else if ((code == ' ') || (code == '\t'))
		{
			//
			// 空白を発見
			//
			auto	nStartIndex	= m_nIndex;
			nextCode();
			code = readCode();
			while (code == ' ' || code == '\t')
			{
				nextCode();
				code = readCode();
			}
			return	Token(Token::SPACE, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
		}
		else if (code == '\n')
		{
			//
			// 改行を発見
			//
			auto	nStartIndex	= m_nIndex;
			nextCode();
			if (readCode() == '\r')
			{
				nextCode();
				code = readCode();
				while (code == '\n')
				{
					nextCode();
					if (readCode() == '\r')
					{
						nextCode();
					}
				}
			}
			return	Token(Token::RETURN, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
		}
		else if (code == '\r')
		{
			//
			// 改行を発見
			//
			auto	nStartIndex	= m_nIndex;
			nextCode();
			if (readCode() == '\n')
			{
				nextCode();
				code = readCode();
				while (code == '\r')
				{
					nextCode();
					if (readCode() == '\n')
					{
						nextCode();
					}
				}
			}
			return	Token(Token::RETURN, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
		}
		else if ((code == '"') || (code == '\''))
		{
			//
			// 文字列を発見
			//
			auto			nStartLine	= location.sourceline();
			auto			nStartIndex	= m_nIndex;
			unsigned int	firstCode	= code;
			nextCode();
			for (;;)
			{
				code = readCode();
				if ((code == 0x00) || (code == '\n') || (code == '\r'))
				{
					// 文字列が途中で折り返されたか、途中でファイルが終端に来た。
					std::string	strMessage;
					if (code == 0x00)
					{
						strMessage	= std::to_string(nStartLine) + ("行目の文字列が閉じられていません。");
					}
					else
					{
						strMessage	= std::to_string(nStartLine) + ("行目の文字列が複数行に渡って定義されています。");
					}
					return	Token(Token::Error, strMessage, location);
				}

				if (code == '\\')
				{
					// エスケープコードを発見
					if (nextCode() >= sourcedata().size())
					{
						std::string	msg	= std::to_string(nStartLine) + ("行目の文字列中にある '\\' の後ろに文字がありません。");
						return	Token(Token::Error, msg, location);
					}
					code = readCode();
					nextCode();
				}
				else if (code == firstCode)
				{
					nextCode();
					if (readCode() != firstCode)
					{
						break;
					}
					nextCode();
				}
				else
				{
					nextCode();
				}
			}
			return	Token(Token::STRING, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
		}
		else if (((code >= 'A') && (code <= 'Z')) || ((code >= 'a') && (code <= 'z')) || (code == '_'))
		{
			//
			// ASCII文字列
			//
			auto	nBeginIndex	= m_nIndex;
			while (m_nIndex < sourcedata().size() && ((code >= '0' && code <= '9') || (code >= 'A' && code <= 'Z') || (code >= 'a' && code <= 'z') || code == '_'))
			{
				nextCode();
				code = readCode();
			}
			return	Token(Token::COMMAND, sourcedata().substr(nBeginIndex, m_nIndex - nBeginIndex).c_str(), location);
		}
		else if (code >= '0' && code <= '9')
		{
			//
			// 数値を発見
			//
			auto	nBeginIndex	= m_nIndex;
			int		nNumber		= code - '0';
			if (nextCode() >= sourcedata().size())
			{
				// １桁の値
				return	Token(Token::NUMBER, sourcedata().substr(nBeginIndex, m_nIndex - nBeginIndex).c_str(), nNumber, location);
			}
			else
			if (code != '0')
			{
				// １０進数
				code = readCode();
				while (m_nIndex < sourcedata().size() && code >= '0' && code <= '9')
				{
					nNumber = (nNumber * 10) + (code - '0');
					nextCode();
					code = readCode();
				}
				return	Token(Token::NUMBER, sourcedata().substr(nBeginIndex, m_nIndex - nBeginIndex).c_str(), nNumber, location);
			}
			else
			{
				// １６進数か８進数
				code = readCode();
				if (code != 'x' && code != 'X')
				{
					//
					// ８進数
					//
					for (;;)
					{
						code = readCode();
						if (code == '8' || code == '9')
						{
							return	Token(Token::Error, "Oct numeric error", location);
						}
						else
						if (code < '0' || code > '7')
						{
							return	Token(Token::NUMBER, sourcedata().substr(nBeginIndex, m_nIndex - nBeginIndex).c_str(), nNumber, location);
						}
						nNumber = (nNumber * 8) + (code - '0');
						nextCode();
					}
				}
				else
				{
					//
					// １６進数
					//
					bool	bSaved = false;
					nextCode();
					for (;;)
					{
						code = readCode();
						if (code >= '0' && code <= '9')
						{
							bSaved = true;
							nNumber = (nNumber * 16) + (code - '0');
							nextCode();
						}
						else
						if (code >= 'a' && code <= 'f')
						{
							bSaved = true;
							nNumber = (nNumber * 16) + (code - 'a') + 10;
							nextCode();
						}
						else
						if (code >= 'A' && code <= 'F')
						{
							bSaved = true;
							nNumber = (nNumber * 16) + (code - 'A') + 10;
							nextCode();
						}
						else
						{
							if (!bSaved)
							{
//								std::string	msg	= ("0xの後ろに１６進数として認識可能な文字列がありません。\n");
								std::string	msg	= ("0x after unkown 16h code.\n");
								return	Token(Token::Error, msg, location);
							}
							else
							{
								return	Token(Token::NUMBER, sourcedata().substr(nBeginIndex, m_nIndex - nBeginIndex).c_str(), nNumber, location);
							}
						}
					}
				}
			}
		}
		else if (sourcedata().find("/*", m_nIndex) == m_nIndex)
		{
			//
			// 複数行コメント発見
			//
			auto	nStartIndex	= m_nIndex;
			auto	nBeginLine	= location.sourceline();
			while (sourcedata().find("*/", m_nIndex) != m_nIndex)
			{
				nextCode();
				if (m_nIndex >= sourcedata().size())
				{
//					std::string	msg	= std::to_string(nBeginLine) + ("行目の /* で始まるコメントが閉じられていません。");
					std::string	msg	= std::to_string(nBeginLine) + ("lines /* is not closed.");
					return	Token(Token::Error, msg, location);
				}
			}
			nextCode();
			nextCode();
			if (m_bEnableComment)
			{
				return	Token(Token::COMMENT, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
			}
		}
		else if (sourcedata().find("//", m_nIndex) == m_nIndex)
		{
			//
			// 行コメント発見
			//
			auto	nStartIndex	= m_nIndex;
			nextCode();
			nextCode();
			if (sourcedata().find("#line ", m_nIndex) == m_nIndex)
			{
				// ライン命令を発見
				std::size_t	line	= 0;
				auto		name	= location.sourcename();

				m_nIndex += std::string("#line ").size();

				// 数値があれば行番号として認識する
				code = readCode();
				if (code >= '0' && code <= '9')
				{
					int	nNumber		= 0;
					do
					{
						nNumber = (nNumber * 10) + (code - '0');
						nextCode();
						code = readCode();
					}
					while (code >= '0' && code <= '9');
					line = nNumber - 1;
				}

				// 文字列があればファイル名として認識する
				while (readCode() == ' ')
				{
					nextCode();
				}
				if (readCode() == '"')
				{
					nextCode();
					auto	nLastIndex	= m_nIndex;
					nextCode();
					while (readCode() != '"')
					{
						nextCode();
					}
					nLastIndex	= m_nIndex;
					name = sourcedata().substr(nStartIndex, nLastIndex-nStartIndex);
				}
				*static_cast<source_location*>(this)	= { sourcedata(), name, (int)line };
			}
			auto		nLastIndex	= m_nIndex;
			while (readCode() != '\n')
			{
				nLastIndex	= m_nIndex;
				nextCode();
				if (m_nIndex >= sourcedata().size())
				{
					break;
				}
			}
			if (m_bEnableComment)
			{
				return	Token(Token::COMMENT, sourcedata().substr(nStartIndex, nLastIndex - nStartIndex).c_str(), location);
			}
		}
		else if (code == '.')
		{
			// ３つ続く場合はひとまとまりにする
			auto	nStartIndex	= m_nIndex;
			nextCode();
			auto	backIndex	= m_nIndex;
			if (readCode() == code)
			{
				nextCode();
				if (readCode() == code)
				{
					nextCode();
					return	Token(Token::CODE, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
				}
			}
			m_nIndex	= backIndex;
			return	Token(Token::CODE, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
		}
		else if (code == ':')
		{
			//
			// 記号を発見。これらは続いて同じものが来る場合は結合する。
			//
			auto	nStartIndex	= m_nIndex;
			nextCode();
			if (readCode() == code)
			{
				nextCode();
			}
			return	Token(Token::CODE, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
		}
		else if (code == '=' || code == '!' || code == '^' || code == '*' || code == '/' || code == '%')
		{
			//
			// 記号を発見。これらは続いて = が来る場合は結合する。
			//
			auto	nStartIndex	= m_nIndex;
			nextCode();
			if (readCode() == '=')
			{
				nextCode();
			}
			return	Token(Token::CODE, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
		}
		else if (code == '<' || code == '>' || code == '|' || code == '&' || code == '+' || code == '-')
		{
			//
			// 記号を発見。これらは続いて = が来るか同じ文字が続く場合は結合する。
			//
			auto	nStartIndex	= m_nIndex;
			nextCode();
			if (readCode() == '=' || readCode() == code)
			{
				nextCode();
			}
			// アロー演算子のチェック
			if (code == '-' && readCode() == '>')
			{
				nextCode();
			}
			return	Token(Token::CODE, sourcedata().substr(nStartIndex, m_nIndex - nStartIndex).c_str(), location);
		}
		else if ((code >= 0x21 && code <= 0x2F) || (code >= 0x3A && code <= 0x40) || (code >= 0x5B && code <= 0x60) || (code >= 0x7B && code <= 0x7E))
		{
			//
			// 記号を発見
			//
			nextCode();
			return	Token(Token::CODE, std::string(1, static_cast<char>(code)), location);
		}
		else
		{
			std::string	msg	= (" unkown character '") + std::string(1, static_cast<char>(code)) + ("' .");
			return	Token(Token::Error, msg, location);
		}
	}
}


inline unsigned int TextReader::readCode()
{
	if (m_nIndex >= sourcedata().size())
	{
		return	0;
	}
	unsigned int	code = sourcedata()[m_nIndex];
	if (code == '\r')
	{
		return	'\n';
	}
	return	code;
}


inline std::size_t TextReader::nextCode()
{
	if (m_nIndex >= sourcedata().size())
	{
		return	m_nIndex;
	}
	unsigned int	code = sourcedata()[m_nIndex];
	if (code == '\n' || code == '\r')
	{
		set_line(line() + 1);
		++m_nIndex;
		unsigned int	next = sourcedata()[m_nIndex];
		if (next != code && (next == '\n' || next == '\r'))
		{
			++m_nIndex;
		}
	}
	else
	{
		++m_nIndex;
	}
	return	m_nIndex;
}


PASSION_NAMESPACE_END
#endif
