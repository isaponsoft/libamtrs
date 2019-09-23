#ifndef	__passion__assembler__code_buffer__hpp
#define	__passion__assembler__code_buffer__hpp
#include "../psnvm.hpp"
#include "../vm/vmopecode.hpp"
#include "def.hpp"
#include "log.hpp"
PASSION_ASSEMBLER_NAMESPACE_BEGIN


struct	code_buffer
{
	//! アセンブラコードの状態
	struct	asmcode
	{
		//! ラベルを持つオペコードを作成する
		static asmcode make_labelable(opecode_id _code, brunch_point _to)
		{
			asmcode	retval;
			retval.opecode	= _code;
			retval.label	= _to;
			retval.minSize	= 9999;
			retval.maxSize	= -9999;
			return	retval;
		}

		opecode_id			opecode		= opecode_id::None;
		std::intmax_t		operand1	= 0;
		std::intmax_t		operand2	= 0;
		vmstring			operandStr;

		brunch_point		label;			//!< ラベルデータの場合はラベル名
		vmstring			call;			//!< invokeで呼び出す関数の識別名
		std::intmax_t		vtbl	= 0;	//!< vtblのインデックス
		vmvariable_entry	variable;		//!< メンバ変数の時のメンバ変数のエントリ
		int					minSize	= 1;
		int					maxSize	= 1;
	};



	auto begin() const -> std::vector<asmcode>::const_iterator { return m_aCodes.begin(); }
	auto end() const -> std::vector<asmcode>::const_iterator { return m_aCodes.end(); }


	// ========================================================================
	//!	バッファに蓄積されているバイトコードのオペランドサイズを計算します。
	// ------------------------------------------------------------------------
	//!	分岐命令などの分岐先のコードによって命令サイズが変わる命令があります。
	//!	そうした命令のコードサイズを計算します。
	// ------------------------------------------------------------------------
	void compute_jmp_opecode_size();

	code_buffer() = default;;
	code_buffer(const code_buffer& ) = default;;
	code_buffer(      code_buffer&&) = default;;
	code_buffer& operator = (const code_buffer& ) = default;;
	code_buffer& operator = (      code_buffer&&) = default;;
	~code_buffer() = default;

	bool empty() const noexcept { return m_aCodes.empty(); }
	std::size_t size() const noexcept { return m_aCodes.size(); }


	void add_ref2int()
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::REFINT;
	}


	void add_load_this()
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LDTHIS;
		++mStackPoint;
	}

	void add_load_int(std::intmax_t _num)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->operand1	= _num;
		++mStackPoint;
		if (_num >= -128 && _num < 128)
		{
			ac->opecode		= opecode_id::LDINT8;
			ac->minSize	= 2;
			ac->maxSize	= 2;
		}
		else
		if (_num >= -32768 && _num < 32768)
		{
			ac->opecode		= opecode_id::LDINT16;
			ac->minSize	= 3;
			ac->maxSize	= 3;
		}
		else
		{
			ac->opecode		= opecode_id::LDINT32;
			ac->minSize	= 5;
			ac->maxSize	= 5;
		}
	}


	void add_load_string(vmstring _string)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LDSTR;
		ac->operandStr	= std::string(_string.c_str());
		ac->minSize		= 3;
		ac->maxSize		= 3;
		++mStackPoint;
	}


	bool add_load(const vmstring& _type)
	{
		opecode_id				opecode	= opecode_id::LOAD_REF;
		int						minSize	= 1;
		int						maxSize	= 1;
		if (passion_is_gcpointerable(_type))
		{
			opecode 	= opecode_id::LOAD_REF;
			minSize		= 3;
			maxSize		= 3;
		}
		else if (passion_size_is_addressable(_type))
		{
			opecode = opecode_id::LOAD_INT;
		}
		else if (auto size = is_floating_point(_type); size)
		{
			switch (size)
			{
				case 4 :	opecode = opecode_id::LOAD_FLOAT;	break;
				case 8 :	opecode = opecode_id::LOAD_DOUBLE;	break;
			}
		}
		else if (passion_is_integer(_type))
		{
			switch (passion_size_of_primitive(_type))
			{
				case 1 :	opecode = opecode_id::LOAD_I8;		break;
				case 2 :	opecode = opecode_id::LOAD_I16;		break;
				case 4 :	opecode = opecode_id::LOAD_I32;		break;
				case 8 :	opecode = opecode_id::LOAD_I64;		break;
			}
		}
		else
		{
AMTRS_TRACE_LOG("Unkown type %s", vmstring(_type).c_str());
			return	false;
		}
		++mStackPoint;
		asmcode*	ac	= this->createBlankCode();
		ac->opecode	= opecode;
		ac->minSize	= minSize;
		ac->maxSize	= maxSize;
		return	true;
	}



	// ==============================================================
	//!	
	// --------------------------------------------------------------
	bool add_store(const vmstring& _type, bool _nopop = false)
	{
		opecode_id				opecode	= _nopop ? opecode_id::STORENP_REF : opecode_id::STORE_REF;
		if (passion_is_gcpointerable(_type))
		{
			opecode = _nopop ? opecode_id::STORENP_REF : opecode_id::STORE_REF;
		}
		else if (passion_size_is_addressable(_type))
		{
			opecode = _nopop ? opecode_id::STORENP_INT : opecode_id::STORE_INT;
		}
		else if (auto size = is_floating_point(_type); size)
		{
			switch (size)
			{
				case 4 :	opecode = _nopop ? opecode_id::STORENP_FLOAT  : opecode_id::STORE_FLOAT;	break;
				case 8 :	opecode = _nopop ? opecode_id::STORENP_DOUBLE : opecode_id::STORE_DOUBLE;	break;
			}
		}
		else if (passion_is_integer(_type))
		{
			switch (passion_size_of_primitive(_type))
			{
				case 1 :	opecode = _nopop ? opecode_id::STORENP_I8  : opecode_id::STORE_I8;	break;
				case 2 :	opecode = _nopop ? opecode_id::STORENP_I16 : opecode_id::STORE_I16;	break;
				case 4 :	opecode = _nopop ? opecode_id::STORENP_I32 : opecode_id::STORE_I32;	break;
				case 8 :	opecode = _nopop ? opecode_id::STORENP_I64 : opecode_id::STORE_I64;	break;
			}
		}
		else
		{
AMTRS_TRACE_LOG("Unkown type %s", vmstring(_type).c_str());
			return	false;
		}
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode;
		++mStackPoint;
		return	true;
	}



	// 廃止予定
	bool add_load(vmclass* _clz)
	{
		amtrs_string_view		type	= amtrs_string_view(_clz->name());
		opecode_id				opecode	= opecode_id::LOAD_REF;
		int						minSize	= 1;
		int						maxSize	= 1;
		if (passion_is_gcpointerable(type))
		{
			opecode = opecode_id::LOAD_REF;
			minSize	= 3;
			maxSize	= 3;
		}
		else if (passion_size_is_addressable(type))
		{
			opecode = opecode_id::LOAD_INT;
		}
		else if (auto size = is_floating_point(type); size)
		{
			switch (size)
			{
				case 4 :	opecode = opecode_id::LOAD_FLOAT;	break;
				case 8 :	opecode = opecode_id::LOAD_DOUBLE;	break;
			}
		}
		else if (passion_is_integer(type))
		{
			switch (passion_size_of_primitive(type))
			{
				case 1 :	opecode = opecode_id::LOAD_I8;		break;
				case 2 :	opecode = opecode_id::LOAD_I16;		break;
				case 4 :	opecode = opecode_id::LOAD_I32;		break;
				case 8 :	opecode = opecode_id::LOAD_I64;		break;
			}
		}
		else
		{
AMTRS_TRACE_LOG("Unkown type %s", vmstring(type).c_str());
			return	false;
		}
		++mStackPoint;
		asmcode*	ac	= this->createBlankCode();
		ac->opecode	= opecode;
		ac->minSize	= minSize;
		ac->maxSize	= maxSize;
		return	true;
	}


	bool add_load(vmclass* _clz, vmclass* _cast_to)
	{
		amtrs_string_view		type	= amtrs_string_view(_clz->name());
		opecode_id				opecode	= opecode_id::STORE_REF;
		int						minSize	= 1;
		int						maxSize	= 1;
		if (passion_is_gcpointerable(type))
		{
			opecode = opecode_id::LOAD_REF;
			minSize	= 3;
			maxSize	= 3;
		}
		else if (passion_size_is_addressable(type))
		{
			opecode = opecode_id::LOAD_INT;
		}
		else if (auto size = is_floating_point(type); size)
		{
			switch (size)
			{
				case 4 :	opecode = opecode_id::LOAD_FLOAT;	break;
				case 8 :	opecode = opecode_id::LOAD_DOUBLE;	break;
			}
		}
		else if (passion_is_integer(type))
		{
			switch (passion_size_of_primitive(type))
			{
				case 1 :	opecode = opecode_id::LOAD_I8;		break;
				case 2 :	opecode = opecode_id::LOAD_I16;		break;
				case 4 :	opecode = opecode_id::LOAD_I32;		break;
				case 8 :	opecode = opecode_id::LOAD_I64;		break;
			}
		}
		else
		{
			opecode = opecode_id::LOAD_REF;
			minSize	= 3;
			maxSize	= 3;
		}
		++mStackPoint;
		asmcode*	ac	= this->createBlankCode();
		ac->opecode	= opecode;
		ac->minSize	= minSize;
		ac->maxSize	= maxSize;
		if (_cast_to)
		{
			vmvariable_entry	entry;
			entry.clazz		= _clz;
			entry.name		= vmstring(".vtable<") + _cast_to->name() + ">*";
			ac->variable	= std::move(entry);
		}
		return	true;
	}



	// ==============================================================
	//!	
	// --------------------------------------------------------------
	bool add_store(vmclass* _clz)
	{
		amtrs_string_view		type	= amtrs_string_view(_clz->name());
		opecode_id				opecode	= opecode_id::STORE_REF;
		if (_clz)
		{
			if (passion_is_gcpointerable(type))
			{
				opecode = opecode_id::STORE_REF;
			}
			else if (passion_size_is_addressable(type))
			{
				opecode = opecode_id::STORE_INT;
			}
			else if (auto size = is_floating_point(type); size)
			{
				switch (size)
				{
					case 4 :	opecode = opecode_id::STORE_FLOAT;	break;
					case 8 :	opecode = opecode_id::STORE_DOUBLE;	break;
				}
			}
			else if (passion_is_integer(type))
			{
				switch (passion_size_of_primitive(type))
				{
					case 1 :	opecode = opecode_id::STORE_I8;		break;
					case 2 :	opecode = opecode_id::STORE_I16;	break;
					case 4 :	opecode = opecode_id::STORE_I32;	break;
					case 8 :	opecode = opecode_id::STORE_I64;	break;
				}
			}
			else
			{
				opecode = opecode_id::STORE_REF;
			}
		}

		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode;
		++mStackPoint;
		return	true;
	}



	// ==============================================================
	//!	
	// --------------------------------------------------------------
	bool add_storenp(vmclass* _clz)
	{
		amtrs_string_view		type	= amtrs_string_view(_clz->name());
		opecode_id				opecode	= opecode_id::STORENP_REF;
		if (_clz)
		{
			if (passion_is_gcpointerable(type))
			{
				opecode = opecode_id::STORENP_REF;
			}
			else if (passion_size_is_addressable(type))
			{
				opecode = opecode_id::STORENP_INT;
			}
			else if (auto size = is_floating_point(type); size)
			{
				switch (size)
				{
					case 4 :	opecode = opecode_id::STORENP_FLOAT;	break;
					case 8 :	opecode = opecode_id::STORENP_DOUBLE;	break;
				}
			}
			else if (passion_is_integer(type))
			{
				switch (passion_size_of_primitive(type))
				{
					case 1 :	opecode = opecode_id::STORENP_I8;	break;
					case 2 :	opecode = opecode_id::STORENP_I16;	break;
					case 4 :	opecode = opecode_id::STORENP_I32;	break;
					case 8 :	opecode = opecode_id::STORENP_I64;	break;
				}
			}
			else
			{
				opecode = opecode_id::STORENP_REF;
			}
		}

		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode;
		++mStackPoint;
		return	true;
	}


	void add_load_adr(vmclass* _clz, vmclass* _cast_to)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LOAD_ADR;
		if (_cast_to)
		{
			vmvariable_entry	entry;
			entry.clazz		= _clz;
			entry.name		= vmstring(".vtable<") + _cast_to->name() + ">*";
			ac->variable	= std::move(entry);
		}
		ac->minSize		= 3;
		ac->maxSize		= 3;
		++mStackPoint;
	}

	void add_load_adr()
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LOAD_ADR;
		ac->minSize		= 3;
		ac->maxSize		= 3;
		++mStackPoint;
	}

	void add_load_ref()
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LOAD_REF;
		ac->minSize		= 3;
		ac->maxSize		= 3;
		++mStackPoint;
	}

	void add_ret()
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::RET;
		ac->minSize		= 1;
		ac->maxSize		= 1;
	}



	// ==============================================================
	//!	ラベルを挿入します。
	// --------------------------------------------------------------
	//!	@return	追加したラベルのポインタ。
	//!			ポインタはコードを追加すると変化することがあるため
	//!			保持してはならない。
	//!			ラベルが重複している場合、nullptr を返す。
	// --------------------------------------------------------------
	bool insert_label(const brunch_point& _label)
	{
		// ラベルが重複して定義されていないか調べる。
		if (this->find_label(_label) >= 0)
		{
			return	false;
		}
		//PSNASM_TRACE_LOG("_%p:", _label.get());

		// ラベルを追加
		asmcode*	pCode	= this->createBlankCode();
		pCode->opecode		= opecode_id::None;
		pCode->label		= _label;
		pCode->minSize		= 0;
		pCode->maxSize		= 0;
		return	true;
	}


	//
	// 指定する名前のラベルを検索し、ラベルが示す位置を返す。
	// ラベルが見つからない場合は -1 になる。
	//
	int find_label(const brunch_point& _label)
	{
		int		index = 0;
		for (auto& def : m_aCodes)
		{
			if (def.opecode == opecode_id::None && def.label == _label)
			{
				return	index;
			}
			++index;
		}
		return	-1;
	}


	// ==============================================================
	//!	alloc 命令を追加します。
	// --------------------------------------------------------------
	void add_alloc(int _size)
	{
		if (_size > 0)
		{
			asmcode*	pCode	= this->createBlankCode();
			pCode->operand1		= _size;
			if (_size >= -128 && _size < 128)
			{
				pCode->opecode		= opecode_id::ALLOC8;
				pCode->minSize	= 2;
				pCode->maxSize	= 2;
			}
			else
			{
				pCode->opecode		= opecode_id::ALLOC16;
				pCode->minSize	= 3;
				pCode->maxSize	= 3;
			}
			mStackPoint += _size;
		}
	}

	// ==============================================================
	//!	free 命令を追加します。
	// --------------------------------------------------------------
	void add_free(int _size)
	{
		if (_size > 0)
		{
			asmcode*	pCode	= this->createBlankCode();
			pCode->operand1		= _size;
			if (_size >= -128 && _size < 128)
			{
				pCode->opecode		= opecode_id::FREE8;
				pCode->minSize	= 2;
				pCode->maxSize	= 2;
			}
			else
			{
				pCode->opecode		= opecode_id::FREE16;
				pCode->minSize	= 3;
				pCode->maxSize	= 3;
			}
			mStackPoint -= _size;
		}
	}

	// ==============================================================
	//!	jfalse 命令を追加します。
	// --------------------------------------------------------------
	void add_jfalse(brunch_point _to_label)
	{
		m_aCodes.push_back(asmcode::make_labelable(opecode_id::JFALSE32, _to_label));
		--mStackPoint;
	}


	// ==============================================================
	//!	jtrue 命令を追加します。
	// --------------------------------------------------------------
	void add_jtrue(brunch_point _to_label)
	{
		m_aCodes.push_back(asmcode::make_labelable(opecode_id::JTRUE32, _to_label));
		--mStackPoint;
	}


	// ==============================================================
	//!	jmp 命令を追加します。
	// --------------------------------------------------------------
	void add_jmp(brunch_point _to_label)
	{
		m_aCodes.push_back(asmcode::make_labelable(opecode_id::JMP32, _to_label));
	}


	// ==============================================================
	//!	load_func 命令を追加します。
	// --------------------------------------------------------------
	void add_load_function(vmfunction* _entry)
	{
		asmcode*	pCode	= this->createBlankCode();
		pCode->opecode		= opecode_id::LOAD_FUNC;
		pCode->call			= _entry->identify();
		pCode->minSize		= 3;
		pCode->maxSize		= 3;
		mStackPoint			-= 1;
	}


	// ==============================================================
	//!	call 命令を追加します。
	// --------------------------------------------------------------
	void add_call(std::size_t _argsize)
	{
		asmcode*	pCode	= this->createBlankCode();
		pCode->opecode		= opecode_id::CALLPTR;
		pCode->operand1		= (int)_argsize;
		pCode->minSize		= 3;
		pCode->maxSize		= 3;
		mStackPoint			-= _argsize;
	}

	// ==============================================================
	//!	call 命令を追加します。
	// --------------------------------------------------------------
	void add_call(vmfunction* _entry, std::size_t _argsize)
	{
		asmcode*	pCode	= this->createBlankCode();
		pCode->opecode		= opecode_id::CALL;
		pCode->operand1		= (int)_argsize;
		pCode->call			= _entry->identify();
		pCode->minSize		= 5;
		pCode->maxSize		= 5;
		mStackPoint			-= _argsize;
	}


	// ==============================================================
	//!	vcall 命令を追加します。
	// --------------------------------------------------------------
	void add_vcall(int _vtbl, std::size_t _argsize)
	{
		asmcode*	pCode	= this->createBlankCode();
		pCode->opecode		= opecode_id::VCALL;
		pCode->operand1		= (int)_argsize;
		pCode->vtbl			= _vtbl;
		pCode->minSize		= 5;
		pCode->maxSize		= 5;
		mStackPoint			-= _argsize;
	}


	void add_dup_adr()
	{
		asmcode*	pCode	= this->createBlankCode();
		pCode->opecode		= opecode_id::DUP_ADR;
		pCode->minSize		= 1;
		pCode->maxSize		= 1;
		mStackPoint += 1;
	}


	void add_inc(vmclass* _clz)
	{
		opecode_id		opecode;
		if (passion_is_gcpointerable(_clz->name()))
		{
			AMTRS_TRACE_LOG("inc can't not pointer NG");
			opecode = opecode_id::LOAD_REF;
		}
		else if (passion_size_is_addressable(_clz->name()))
		{
			opecode = opecode_id::INC_INT;
		}
		else if (auto size = is_floating_point(_clz->name()); size)
		{
			AMTRS_TRACE_LOG("inc can't not float NG");
			switch (size)
			{
				case 4 :	opecode = opecode_id::LOAD_FLOAT;	break;
				case 8 :	opecode = opecode_id::LOAD_DOUBLE;	break;
			}
		}
		else if (passion_is_integer(_clz->name()))
		{
			switch (passion_size_of_primitive(_clz->name()))
			{
				case 1 :	opecode = opecode_id::INC_I8;		break;
				case 2 :	opecode = opecode_id::INC_I16;		break;
				case 4 :	opecode = opecode_id::INC_I32;		break;
				case 8 :	opecode = opecode_id::INC_I64;		break;
			}
		}
		else
		{
			AMTRS_TRACE_LOG("inc can't unkown type %s NG", _clz->name().c_str());
			return;
		}

		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode;
		mStackPoint -= 2;
	}


	void add_dec(vmclass* _clz)
	{
		opecode_id		opecode;
		if (passion_is_gcpointerable(_clz->name()))
		{
			AMTRS_TRACE_LOG("dec can't not pointer NG");
			opecode = opecode_id::LOAD_REF;
		}
		else if (passion_size_is_addressable(_clz->name()))
		{
			opecode = opecode_id::DEC_INT;
		}
		else if (auto size = is_floating_point(_clz->name()); size)
		{
			AMTRS_TRACE_LOG("dec can't not float NG");
			switch (size)
			{
				case 4 :	opecode = opecode_id::LOAD_FLOAT;	break;
				case 8 :	opecode = opecode_id::LOAD_DOUBLE;	break;
			}
		}
		else if (passion_is_integer(_clz->name()))
		{
			switch (passion_size_of_primitive(_clz->name()))
			{
				case 1 :	opecode = opecode_id::DEC_I8;		break;
				case 2 :	opecode = opecode_id::DEC_I16;		break;
				case 4 :	opecode = opecode_id::DEC_I32;		break;
				case 8 :	opecode = opecode_id::DEC_I64;		break;
			}
		}
		else
		{
			AMTRS_TRACE_LOG("dec can't unkown type %s NG", _clz->name().c_str());
			return;
		}

		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode;
		mStackPoint -= 2;
	}


	void add_add() { _add_command(opecode_id::ADD); }
	void add_sub() { _add_command(opecode_id::SUB); }
	void add_mul() { _add_command(opecode_id::MUL); }
	void add_div() { _add_command(opecode_id::DIV); }
	void add_mod() { _add_command(opecode_id::MOD); }
	void add_and() { _add_command(opecode_id::AND); }
	void add_or () { _add_command(opecode_id::OR ); }
	void add_xor() { _add_command(opecode_id::XOR); }
	void add_shiftl() { _add_command(opecode_id::SHIFTL); }
	void add_shiftr() { _add_command(opecode_id::SHIFTR); }
	void add_land() { _add_command(opecode_id::LAND); }
	void add_lor() { _add_command(opecode_id::LOR); }

	void add_cmpe()  { _add_command(opecode_id::CMPE); }
	void add_cmpn()  { _add_command(opecode_id::CMPN); }
	void add_cmps()  { _add_command(opecode_id::CMPS); }
	void add_cmpl()  { _add_command(opecode_id::CMPL); }
	void add_cmpse() { _add_command(opecode_id::CMPSE); }
	void add_cmple() { _add_command(opecode_id::CMPLE); }

	void add_not() { _add_command(opecode_id::NOT, 1); }

	void add_new_array()    { _add_command(opecode_id::NEW_A,    2, 5); }
	void add_new_arrayref() { _add_command(opecode_id::NEW_AREF, 1); }


	void add_new(vmstring _classname)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::NEW;
		ac->operandStr	= std::string(_classname.c_str());
		ac->operand1	= 0;
		ac->minSize		= 3;
		ac->maxSize		= 3;
		mStackPoint += 1;
	}

	void add_lea_arg(std::intmax_t _arg)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LEA_A;
		ac->operand1	= _arg;
		ac->minSize		= 3;
		ac->maxSize		= 3;
		mStackPoint += 1;
	}

	void add_lea_local(std::intmax_t _local)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LEA_L;
		ac->operand1	= _local;
		ac->minSize		= 3;
		ac->maxSize		= 3;
		mStackPoint += 1;
	}

	void add_lea_stack(std::intmax_t _sp)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LEA_S;
		ac->operand1	= _sp;
		ac->minSize		= 3;
		ac->maxSize		= 3;
		mStackPoint += 1;
	}


	void add_larray()
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LARRAY;
		ac->minSize		= 1;
		ac->maxSize		= 1;
		mStackPoint -= 1;
		mStackPoint += 1;
	}


	void add_lea(intmax_t _scale, size_t _offset)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LEA;
		ac->operand1	= _scale;
		ac->operand2	= _offset;
		ac->minSize		= 4;
		ac->maxSize		= 4;
		mStackPoint -= 2;
		mStackPoint += 1;
	}

	void add_lea_aref()
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::LEA_AREF;
		ac->minSize		= 1;
		ac->maxSize		= 1;
		mStackPoint -= 2;
		mStackPoint += 1;
	}

	void add_lea_array(vmclass* _clz)
	{
		opecode_id		opecode;
		if (passion_is_gcpointerable(_clz->name()))
		{
			opecode = opecode_id::LEA_AREF;
		}
		else if (passion_size_is_addressable(_clz->name()))
		{
			AMTRS_TRACE_LOG("LEA array can't not addressib;e NG");
			opecode = opecode_id::LOAD_INT;
		}
		else if (auto size = is_floating_point(_clz->name()); size)
		{
			AMTRS_TRACE_LOG("LEA array can't not float NG");
			switch (size)
			{
				case 4 :	opecode = opecode_id::LOAD_FLOAT;	break;
				case 8 :	opecode = opecode_id::LOAD_DOUBLE;	break;
			}
		}
		else if (passion_is_integer(_clz->name()))
		{
			switch (passion_size_of_primitive(_clz->name()))
			{
				case 1 :	opecode = opecode_id::LEA_AI8;		break;
				case 2 :	opecode = opecode_id::LEA_AI16;		break;
				case 4 :	opecode = opecode_id::LEA_AI32;		break;
				case 8 :	opecode = opecode_id::LEA_AI64;		break;
			}
		}
		else
		{
			return;
		}
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode;
		ac->minSize		= 1;
		ac->maxSize		= 1;
		mStackPoint -= 2;
		mStackPoint += 1;
		++mStackPoint;
	}


	// ==============================================================
	//!	lea_o 命令を追加します。
	// --------------------------------------------------------------
	void add_lea_object(vmvariable_entry&& _entry)
	{
		asmcode*	pCode	= this->createBlankCode();
		pCode->opecode		= opecode_id::LEA_O;
		pCode->variable		= std::move(_entry);
		pCode->minSize		= 3;
		pCode->maxSize		= 3;
		mStackPoint -= 2;
		mStackPoint += 1;
	}


	// ==============================================================
	//!	lea_g 命令を追加します。
	// --------------------------------------------------------------
	void add_lea_global(vmvariable_entry&& _entry)
	{
		asmcode*	pCode	= this->createBlankCode();
		pCode->opecode		= opecode_id::LEA_G;
		pCode->variable		= std::move(_entry);
		pCode->minSize		= 3;
		pCode->maxSize		= 3;
		mStackPoint -= 2;
		mStackPoint += 1;
	}


	void add_delete_ref()
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= opecode_id::DELETE_REF;
		ac->minSize		= 1;
		ac->maxSize		= 1;
		mStackPoint -= 1;
	}




private:
	void _add_command(opecode_id _o, int _stk = 2, int _size = 1)
	{
		asmcode*	ac	= this->createBlankCode();
		ac->opecode		= _o;
		ac->minSize		= _size;
		ac->maxSize		= _size;
		mStackPoint -= _stk;
	}


	//
	// 指定する範囲のコードが取りうる最小サイズを計算する。
	// \return int 最小バイト数。一切計算不可能なコードがあれば -1 を返す。
	//
	int minmamCodeLength(int nBegin, int nLast)
	{
		int			nLength	= 0;
		const auto*	pCode	= m_aCodes.data();
		for (int index = nBegin; index < nLast; ++index)
		{
			if (pCode[index].minSize > pCode[index].maxSize)
			{
				// コードサイズが全くの不明のものがあったので、エラー。
				return	-1;
			}
			nLength += pCode[index].minSize;
		}
		return	nLength;
	}


	//
	// 指定する範囲のコードが取りうる最大サイズを計算する。
	// \return int 最大バイト数。一切計算不可能なコードがあれば -1 を返す。
	//
	int maximamCodeLength(int nBegin, int nLast)
	{
		int			nLength	= 0;
		const auto*	pCode	= m_aCodes.data();
		for (int index = nBegin; index < nLast; ++index)
		{
			if (pCode[index].minSize > pCode[index].maxSize)
			{
				// コードサイズが全くの不明のものがあったので、エラー。
				return	-1;
			}
			nLength += pCode[index].maxSize;
		}
		return	nLength;
	}

	//
	// 新しくコードを記録するバッファを生成する。
	//
	asmcode* createBlankCode()
	{
		m_aCodes.push_back({});
		asmcode&	block	= m_aCodes.back();
		return	&block;
	}


	std::vector<asmcode>		m_aCodes;			//!< コード
	std::size_t					mStackPoint	= 0;	//!< スタックポイント
};




inline void code_buffer::compute_jmp_opecode_size()
{
	for (;;)
	{
		bool	bNonSizing	= false;
		int		nIndex		= -1;
		for (auto& code : m_aCodes)
		{
			++nIndex;
			if (code.opecode == opecode_id::JMP8    || code.opecode == opecode_id::JMP16    || code.opecode == opecode_id::JMP32
			 || code.opecode == opecode_id::JFALSE8 || code.opecode == opecode_id::JFALSE16 || code.opecode == opecode_id::JFALSE32
			 || code.opecode == opecode_id::JTRUE8  || code.opecode == opecode_id::JTRUE16  || code.opecode == opecode_id::JTRUE32
			)
			{
				int	nLabel	= find_label(code.label);
				int	minOffset	= 0;
				int	maxOffset	= 0;
				if (nIndex < nLabel)
				{
					// 後方参照
					minOffset	= minmamCodeLength(nIndex+1, nLabel);
					maxOffset	= maximamCodeLength(nIndex+1, nLabel);
				}
				else
				{
					// 前方参照
					minOffset	= minmamCodeLength(nLabel, nIndex);
					maxOffset	= maximamCodeLength(nLabel, nIndex);
				}

				if (minOffset == -1 || maxOffset == -1)
				{
					code.minSize = 2;
					code.maxSize = 5;
					bNonSizing = true;
				}
				else
				{
					if (minOffset >= -128 && minOffset < 128)
					{
						if (nIndex > nLabel)
						{
							// 前方にジャンプするので、自身の命令サイズ分も加算しておく
							minOffset += 2;
						}
						if (minOffset >= -128 && minOffset < 128)
						{
							code.minSize = 2;
						}
						else
						{
							minOffset += 1;
							code.minSize = 3;
						}
					}
					else
					if (minOffset >= -32768 && minOffset < 32768)
					{
						if (nIndex > nLabel)
						{
							// 前方にジャンプするので、自身の命令サイズ分も加算しておく
							minOffset += 3;
						}
						if (minOffset >= -32768 && minOffset < 32768)
						{
							code.minSize = 3;
						}
						else
						{
							minOffset += 2;
							code.minSize = 5;
						}
					}
					else
					{
						code.minSize = 5;
					}

					if (maxOffset >= -128 && maxOffset < 128)
					{
						if (nIndex > nLabel)
						{
							// 前方にジャンプするので、自身の命令サイズ分も加算しておく
							maxOffset += 2;
						}
						if (maxOffset >= -128 && maxOffset < 128)
						{
							code.maxSize = 2;
						}
						else
						{
							maxOffset += 1;
							code.maxSize = 3;
						}
					}
					else
					if (maxOffset >= -32768 && maxOffset < 32768)
					{
						if (nIndex > nLabel)
						{
							// 前方にジャンプするので、自身の命令サイズ分も加算しておく
							maxOffset += 3;
						}
						if (maxOffset >= -32768 && maxOffset < 32768)
						{
							code.maxSize = 3;
						}
						else
						{
							maxOffset += 2;
							code.maxSize = 5;
						}
					}
					else
					{
						code.maxSize = 5;
					}

					if (nIndex < nLabel)
							code.operand1	= maxOffset;
					else	code.operand1	= -maxOffset;
					if (minOffset != maxOffset)
					{
						bNonSizing = true;
					}
					else
					{
						if (minOffset >= -128 && minOffset < 128)
						{
							if (code.opecode == opecode_id::JMP8    || code.opecode == opecode_id::JMP16    || code.opecode == opecode_id::JMP32)
							{
								code.opecode	= opecode_id::JMP8;
							}
							if (code.opecode == opecode_id::JFALSE8 || code.opecode == opecode_id::JFALSE16 || code.opecode == opecode_id::JFALSE32)
							{
								code.opecode	= opecode_id::JFALSE8;
							}
							if (code.opecode == opecode_id::JTRUE8  || code.opecode == opecode_id::JTRUE16  || code.opecode == opecode_id::JTRUE32)
							{
								code.opecode	= opecode_id::JTRUE8;
							}
						}
						else if (minOffset >= -32768 && maxOffset < 32768)
						{
							if (code.opecode == opecode_id::JMP8    || code.opecode == opecode_id::JMP16    || code.opecode == opecode_id::JMP32)
							{
								code.opecode	= opecode_id::JMP16;
							}
							if (code.opecode == opecode_id::JFALSE8 || code.opecode == opecode_id::JFALSE16 || code.opecode == opecode_id::JFALSE32)
							{
								code.opecode	= opecode_id::JFALSE16;
							}
							if (code.opecode == opecode_id::JTRUE8  || code.opecode == opecode_id::JTRUE16  || code.opecode == opecode_id::JTRUE32)
							{
								code.opecode	= opecode_id::JTRUE16;
							}
						}
						else
						{
							if (code.opecode == opecode_id::JMP8    || code.opecode == opecode_id::JMP16    || code.opecode == opecode_id::JMP32)
							{
								code.opecode	= opecode_id::JMP32;
							}
							if (code.opecode == opecode_id::JFALSE8 || code.opecode == opecode_id::JFALSE16 || code.opecode == opecode_id::JFALSE32)
							{
								code.opecode	= opecode_id::JFALSE32;
							}
							if (code.opecode == opecode_id::JTRUE8  || code.opecode == opecode_id::JTRUE16  || code.opecode == opecode_id::JTRUE32)
							{
								code.opecode	= opecode_id::JTRUE32;
							}
						}
					}
				}
			}
		}
		// すべてのコードのサイズが決定した
		if (!bNonSizing)
		{
			break;
		}
	}
}

PASSION_ASSEMBLER_NAMESPACE_END
#endif
