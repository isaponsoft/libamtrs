#ifndef	__passion__assembler__assembler__hpp
#define	__passion__assembler__assembler__hpp
#include "../psnvm.hpp"
#include "../token/escape.hpp"
#include "../vm/def.hpp"
#include "def.hpp"
#include "code_buffer.hpp"
#include "log.hpp"
PASSION_ASSEMBLER_NAMESPACE_BEGIN

// ****************************************************************************
//! 関数のアセンブルを行います。
// ----------------------------------------------------------------------------
class	assembler
		: public code_buffer
{
public:
	using	stringpool_type		= std::vector<vmstring>;
	using	functiontable_type	= std::vector<vmstring>;
	using	variableable_type	= std::vector<vmvariable_entry>;

	const stringpool_type& string_pool() const noexcept { return mStringPool; }
	const stringpool_type& classes_pool() const noexcept { return mClassesPool; }
	const functiontable_type& function_table() const noexcept { return mFunctionTable; }
	const variableable_type& variable_table() const noexcept { return mVariableTable; }

	assembler()
	{
		mVariableTable.push_back({nullptr, ""});		// 常に 0 を示す
	}

	//
	// コードを変換する
	//
	shared_buffer<uint8_t> assemble()
	{
		PSNASM_TRACE_LOG("assemble");

		code_buffer& _ir	= *this;

		// ジャンプ命令のサイズを決定する
		_ir.compute_jmp_opecode_size();

		// ニーモニックを出力する
		std::vector<uint8_t>	bc;
		for (const auto& code : _ir)
		{
			switch (code.opecode)
			{
				case opecode_id::None:
					if (!code.label.empty())
					{
						PSNASM_TRACE_LOG("%p:", code.label.get());
					}
					break;

				case opecode_id::LDINT8:
					PSNASM_TRACE_LOG("[%04zx:%d] LDINT8 %d", bc.size(), (int)code.minSize, (int)code.operand1);
					push_int8(bc, opecode_id::LDINT8);
					push_int8(bc, static_cast<uint8_t>(code.operand1));
					break;

				case opecode_id::LDINT16:
					PSNASM_TRACE_LOG("[%04zx:%d] LDINT16 %d", bc.size(), (int)code.minSize, (int)code.operand1);
					push_int8(bc, opecode_id::LDINT16);
					push_int16_le(bc, static_cast<uint16_t>(code.operand1));
					break;

				case opecode_id::LDINT32:
					PSNASM_TRACE_LOG("[%04zx:%d] LDINT32 %d", bc.size(), (int)code.minSize, (int)code.operand1);
					push_int8(bc, opecode_id::LDINT32);
					push_int32_le(bc, static_cast<uint32_t>(code.operand1));
					break;

				case opecode_id::LDSTR:
					PSNASM_TRACE_LOG("[%04zx:%d] LDSTR %s", bc.size(), (int)code.minSize, code.operandStr.c_str());
					push_int8(bc, opecode_id::LDSTR);
					push_int16_le(bc, add_string(mStringPool, code.operandStr));
					break;

				case opecode_id::ALLOC8:
					PSNASM_TRACE_LOG("[%04zx:%d] ALLOC8 %d", bc.size(), (int)code.minSize, code.operand1);
					push_int8(bc, opecode_id::ALLOC8);
					push_int8(bc, static_cast<uint8_t>(code.operand1));
					break;

				case opecode_id::ALLOC16:
					PSNASM_TRACE_LOG("[%04zx:%d] ALLOC16 %d", bc.size(), (int)code.minSize, code.operand1);
					push_int8(bc, opecode_id::ALLOC16);
					push_int16_le(bc, static_cast<uint16_t>(code.operand1));
					break;

				case opecode_id::FREE8:
					PSNASM_TRACE_LOG("[%04zx:%d] FREE8 %d", bc.size(), (int)code.minSize, code.operand1);
					push_int8(bc, opecode_id::FREE8);
					push_int8(bc, static_cast<uint8_t>(code.operand1));
					break;

				case opecode_id::FREE16:
					PSNASM_TRACE_LOG("[%04zx:%d] FREE16 %d", bc.size(), (int)code.minSize, code.operand1);
					push_int8(bc, opecode_id::FREE16);
					push_int16_le(bc, static_cast<uint16_t>(code.operand1));
					break;

				case opecode_id::LOAD_REF :
				{
					int	id	= add_variable(code.variable);
					PSNASM_TRACE_LOG("[%04zx:%d] LOAD_REF %d ; %s", bc.size(), (int)code.minSize, id, code.variable.name.c_str());
					push_int8(bc, opecode_id::LOAD_REF);
					push_int16_le(bc, (int16_t)id);
					break;
				}

				case opecode_id::LOAD_ADR :
				{
					int	id	= add_variable(code.variable);
					PSNASM_TRACE_LOG("[%04zx:%d] LOAD_ADR %d ; %s", bc.size(), (int)code.minSize, id, code.variable.name.c_str());
					push_int8(bc, opecode_id::LOAD_ADR);
					push_int16_le(bc, (int16_t)id);
					break;
				}

				case opecode_id::LEA:
					PSNASM_TRACE_LOG("[%04zx:%d] LEA [%d * %d + %d]", bc.size(), (int)code.minSize, (int)code.operand1, (int)code.operand2);
					push_int8		(bc, opecode_id::LEA);
					push_int8		(bc, static_cast<uint8_t>(code.operand1));
					push_int16_le	(bc, static_cast<int16_t>(code.operand2));
					break;

				case opecode_id::LEA_A:
					PSNASM_TRACE_LOG("[%04zx:%d] LEA_A %d", bc.size(), (int)code.minSize, (int)code.operand1);
					push_int8(bc, opecode_id::LEA_A);
					push_int16_le(bc, static_cast<int16_t>(code.operand1));
					break;

				case opecode_id::LEA_L:
					PSNASM_TRACE_LOG("[%04zx:%d] LEA_L %d", bc.size(), (int)code.minSize, (int)code.operand1);
					push_int8(bc, opecode_id::LEA_L);
					push_int16_le(bc, static_cast<int16_t>(code.operand1));
					break;

				case opecode_id::LEA_S:
					PSNASM_TRACE_LOG("[%04zx:%d] LEA_S %d", bc.size(), (int)code.minSize, (int)code.operand1);
					push_int8(bc, opecode_id::LEA_S);
					push_int16_le(bc, static_cast<int16_t>(code.operand1));
					break;

				case opecode_id::LEA_O:
				{
					int	id	= add_variable(code.variable);
					PSNASM_TRACE_LOG("[%04zx:%d] LAE_O %d ; %s.%s", bc.size(), (int)code.minSize, id, code.variable.clazz->identify().c_str(), code.variable.name.c_str());
					push_int8(bc, opecode_id::LEA_O);
					push_int16_le(bc, (int16_t)id);
					break;
				}

				case opecode_id::LEA_G:
				{
					int	id	= add_variable(code.variable);
					PSNASM_TRACE_LOG("[%04zx:%d] LAE_G %d ; %s.%s", bc.size(), (int)code.minSize, id, code.variable.clazz->identify().c_str(), code.variable.name.c_str());
					push_int8(bc, opecode_id::LEA_G);
					push_int16_le(bc, (int16_t)id);
					break;
				}

				case opecode_id::JMP8:
					PSNASM_TRACE_LOG("[%04zx:%d] JMP8 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JMP8);
					push_int8(bc, static_cast<uint8_t>(code.operand1));
					break;

				case opecode_id::JMP16:
					PSNASM_TRACE_LOG("[%04zx:%d] JMP16 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JMP16);
					push_int16_le(bc, static_cast<uint16_t>(code.operand1));
					break;

				case opecode_id::JMP32:
					PSNASM_TRACE_LOG("[%04zx:%d] JMP32 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JMP32);
					push_int32_le(bc, static_cast<uint32_t>(code.operand1));
					break;

				case opecode_id::JFALSE8:
					PSNASM_TRACE_LOG("[%04zx:%d] JFALSE8 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JFALSE8);
					push_int8(bc, static_cast<uint8_t>(code.operand1));
					break;

				case opecode_id::JFALSE16:
					PSNASM_TRACE_LOG("[%04zx:%d] JFALSE16 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JFALSE16);
					push_int16_le(bc, static_cast<uint16_t>(code.operand1));
					break;

				case opecode_id::JFALSE32:
					PSNASM_TRACE_LOG("[%04zx:%d] JFALSE32 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JFALSE32);
					push_int32_le(bc, static_cast<uint32_t>(code.operand1));
					break;

				case opecode_id::JTRUE8:
					PSNASM_TRACE_LOG("[%04zx:%d] JTRUE8 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JTRUE8);
					push_int8(bc, static_cast<uint8_t>(code.operand1));
					break;

				case opecode_id::JTRUE16:
					PSNASM_TRACE_LOG("[%04zx:%d] JTRUE16 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JTRUE16);
					push_int16_le(bc, static_cast<uint16_t>(code.operand1));
					break;

				case opecode_id::JTRUE32:
					PSNASM_TRACE_LOG("[%04zx:%d] JTRUE32 %d   ; _%p ... [%x]", bc.size(), (int)code.minSize, code.operand1, code.label.get(), bc.size()+2+code.operand1);
					push_int8(bc, opecode_id::JTRUE32);
					push_int32_le(bc, static_cast<uint32_t>(code.operand1));
					break;

				case opecode_id::NEW:
					PSNASM_TRACE_LOG("[%04zx:%d] NEW %s", bc.size(), (int)code.minSize, code.operandStr.c_str());
					push_int8(bc, opecode_id::NEW);
					push_int16_le(bc, add_identify(mClassesPool, code.operandStr));
					break;

				case opecode_id::LOAD_FUNC:
				{
					int	id	= add_function(code.call);
					PSNASM_TRACE_LOG("[%04zx:%d] LOAD_FUNC %d:'%s'", bc.size(), (int)code.minSize, id, code.call.c_str());
					push_int8(bc, opecode_id::LOAD_FUNC);
					push_int16_le(bc, static_cast<int16_t>(id));
					break;
				}

				case opecode_id::CALLPTR:
				{
					PSNASM_TRACE_LOG("[%04zx] CALLPTR %d", bc.size(), (int)code.minSize, code.operand1);
					push_int8(bc, opecode_id::CALLPTR);
					push_int16_le(bc, static_cast<int16_t>(code.operand1));
					break;
				}

				case opecode_id::CALL:
				{
					int	id	= add_function(code.call);
					PSNASM_TRACE_LOG("[%04zx:%d] CALL %d:'%s', arg=%d", bc.size(), (int)code.minSize, id, code.call.c_str(), code.operand1);
					push_int8(bc, opecode_id::CALL);
					push_int16_le(bc, static_cast<int16_t>(id));
					push_int16_le(bc, static_cast<int16_t>(code.operand1));
					break;
				}

				case opecode_id::VCALL:
				{
					PSNASM_TRACE_LOG("[%04zx:%d] VCALL vtbl=%d, arg=%d", bc.size(), (int)code.minSize, (int)code.vtbl, code.call.c_str(), code.operand1);
					push_int8(bc, opecode_id::VCALL);
					push_int16_le(bc, static_cast<int16_t>(code.vtbl));
					push_int16_le(bc, static_cast<int16_t>(code.operand1));
					break;
				}

				default:
					PSNASM_TRACE_LOG("[%04zx:%d] %s", bc.size(), (int)code.minSize, get_opecodename(code.opecode));
					push_int8(bc, code.opecode);
					break;
			}
		}

		shared_buffer<uint8_t>	retval;
		retval		= shared_buffer<uint8_t>(bc.size());
		std::memcpy(retval.data(), bc.data(), bc.size());
		return	retval;
	}

private:
	static void push_int8(std::vector<uint8_t>& _buff, opecode_id _id)
	{
		_buff.push_back(static_cast<uint8_t>(_id));
	}

	static void push_int8(std::vector<uint8_t>& _buff, uint8_t code)
	{
		_buff.push_back(code);
	}



	static void push_int16_le(std::vector<uint8_t>& _buff, unsigned short code)
	{
		_buff.push_back((uint8_t)code&0xFF);
		_buff.push_back((uint8_t)(code>>8)&0xFF);
	}


	static void push_int32_le(std::vector<uint8_t>& _buff, unsigned int code)
	{
		_buff.push_back((uint8_t)code&0xFF);
		_buff.push_back((uint8_t)(code>>8)&0xFF);
		_buff.push_back((uint8_t)(code>>16)&0xFF);
		_buff.push_back((uint8_t)(code>>24)&0xFF);
	}


	static uint16_t add_identify(stringpool_type& _pool, const vmstring& _string)
	{
		// 既に登録済みなら登録しない
		uint16_t	index	= 0;
		for (; index < _pool.size(); ++index)
		{
			if (_pool[index] == _string)
			{
				return	index;
			}
		}
		_pool.push_back(_string);
		return	index;
	}

	static uint16_t add_string(stringpool_type& _pool, const vmstring& _string)
	{
		// 文字列をエスケープ
		std::vector<char>	buff(_string.size() + 1);
		char*				dest	= buff.data();
		unsigned int		uFirst	= _string[0];		// " のはず。
		std::size_t			nIndex	= 1;
		while (nIndex < _string.size())
		{
			unsigned int	code = _string[nIndex];
			++nIndex;
			if (code == uFirst)
			{
				if (nIndex < _string.size())
				{
					*dest++ = static_cast<char>(code);
					++nIndex;
				}
				else
				{
					break;
				}
			}
			else if (code == '\\')
			{
				if (_string[nIndex] == 'n')
				{
					*dest++ = '\n';
				}
				else
				{
					*dest++ = _string[nIndex];
				}
				++nIndex;
			}
			else
			{
				*dest++ = static_cast<char>(code);
			}
		}

		return	add_identify(_pool, {std::string_view(buff.data(), (std::size_t)(dest - buff.data()))});
	}



	int16_t add_function(const vmstring& _function)
	{
		int16_t		i	= 0;
		for (auto& f : mFunctionTable)
		{
			if (f == _function)
			{
				return	i;
			}
			++i;
		}
		mFunctionTable.push_back(_function);
		return	i;
	}


	int16_t add_variable(const vmvariable_entry& _entry)
	{
		int16_t		i	= 0;
		for (auto& f : mVariableTable)
		{
			if (f == _entry)
			{
				return	i;
			}
			++i;
		}
		mVariableTable.push_back(_entry);
		return	i;
	}



	stringpool_type		mStringPool;
	stringpool_type		mClassesPool;
	functiontable_type	mFunctionTable;
	variableable_type	mVariableTable;
};


PASSION_ASSEMBLER_NAMESPACE_END
#endif

