#ifndef	__passion__vmclass__hpp
#define	__passion__vmclass__hpp
#include "def.hpp"
#include "vmfunction.hpp"
#include "vmobject.hpp"
#include "../inc/struct_info.h"
PASSION_EXTERN_C_BEGIN

/*
 * Class 定義
 */
struct	passion_class
{
	passion_struct_variables*	member_variables;
};



PASSION_EXTERN_C_END
PASSION_NAMESPACE_BEGIN
class	vmclass_manager;
struct	vmvtable;





// ****************************************************************************
//! クラス定義
// ----------------------------------------------------------------------------
//! コンパイル時に使用される。実行時には参照されない。
// ----------------------------------------------------------------------------
struct	vmclass
		: public ref_object
		, public passion_class
{
	static constexpr std::size_t	ninit	= static_cast<std::size_t>(-1);

	using	argument_type	= vmvariant;


	struct	native_conf
	{
		using	sizer_type		= std::size_t(*)(argument_type*, std::size_t);
		using	init_type		= vmobject::vmnative*(*)(void* native, void* extra, argument_type*, std::size_t);

		sizer_type		sizer	= nullptr;		//!< ネイティブオブジェクトの拡張領域のサイズを返す
		init_type		init	= nullptr;		//!< ネイティブ初期化関数
	};


	struct	declere_value
	{
		declere_value() = default;
		declere_value(const declere_value&) = default;
		declere_value(declere_value&&) = default;
		declere_value& operator = (const declere_value&) = default;
		declere_value& operator = (declere_value&&) = default;

		declere_value(vmstring _type, vmstring _name, uint32_t _attribute = 0, std::size_t _position = 0)
			: type(std::move(_type))
			, name(std::move(_name))
			, attribute(_attribute)
			, position(_position)
		{}

		vmstring		type;
		vmstring		name;
		uint32_t		attribute	= 0;		//!< vmmember_attributes
		std::size_t		position	= 0;		//!< 変数の位置
	};

	struct	vtable_info
	{
		passion_vtable*	vtbl	= nullptr;
		size_t			size	= 0;
	};


	bool							prototype	= false;			//!< true の時はプロトタイプのみ
	attribute_flag					access		= attribute_flag::public_attribute;

	// ========================================================================
	//! クラス名を指定してクラスを作成します。
	// ------------------------------------------------------------------------
	vmclass(vmstring _classname)
		: mClassName(_classname)
		, mStructSize(ninit)
	{
		this->member_variables	= nullptr;
	}

	~vmclass()
	{
		passion_struct_variables_destroy(this->member_variables);
		for (auto& v : mVtableList)
		{
			if (v.vtbl)
			{
				passion_vtable_destroy(v.vtbl);
			}
		}
	}



	const vmstring& name() const noexcept { return mClassName; }
	const vmstring& identify() const noexcept { return mClassName; }

	const auto& functions() const noexcept { return mFunctions; }

	std::vector<passion_function*> function_list() const
	{
		std::vector<passion_function*>	retval;
		for (auto f : functions())
		{
			retval.push_back(f->function());
		}
		return	retval;
	}


	//! 構造体の大きさ（vmobjectなどのヘッダ部分のサイズを含まない）
	passion_variable_size struct_size() const noexcept { return { (uint16_t)mStructSize, 0 }; }

	auto& vtables() const noexcept { return mVtableList; }			// 主にダンプで使用する


	template<class T>
	void set_native_class(vmclass_manager* _vm)
	{
		declere_value	ntv[2];
		ntv[0].type		= vmstring((make_constring("native<") + T::signature() + make_constring(">*")).c_str());
		ntv[0].name		= (make_constring(".native-") + T::signature()).c_str();
		set_variables(ntv, 1);
		mNativeConf	= native_conf{ T::native_size, T::native_new };
		T::native_functions(_vm, this);
	}



	vmfunction* add_function(vmfunction* _f)
	{
		mFunctions.push_back(_f);
		return _f;
	}



	vmfunction* find_function(const vmstring& _name, std::size_t _argSize) const noexcept;


	//! 一致する関数名を持つ関数があるかどうかを検索を行う。
	vmfunction* find_function(const vmstring& _name) const noexcept
	{
		auto	it	= std::find_if(mFunctions.begin(), mFunctions.end(), [&](vmfunction* _func) -> bool { return _name == _func->name(); });
		if (it != mFunctions.end())
		{
			return	*it;
		}
		for (const auto& base : mBaseClasses)
		{
			auto	f = base->find_function(_name);
			if (f)
			{
				return	f;
			}
		}
		return	nullptr;
	}



	//! 一致する関数名を持つ関数がいくつあるかを返す
	size_t count_function(const vmstring& _name) const noexcept
	{
		auto	count	= std::count_if(mFunctions.begin(), mFunctions.end(), [&](vmfunction* _func) -> bool { return _name == _func->name(); });
		for (const auto& base : mBaseClasses)
		{
			count	+=  base->count_function(_name);
		}
		return	count;
	}



	void add_base_class(vmclass* _clazz) { mBaseClasses.push_back(_clazz); }


	bool is_base_of(const vmstring& _name) const noexcept
	{
		if (mClassName == _name)
		{
			return	true;
		}
		for (auto& bc : mBaseClasses)
		{
			if (bc->is_base_of(_name))
			{
				return	true;
			}
		}
		return	false;
	}


	// ========================================================================
	//! キャスト時に必要なオフセット値を計算します。
	// ------------------------------------------------------------------------
	//! キャスト不可能な場合の値は不定です。
	// ------------------------------------------------------------------------
	bool cast_offset(vmvariable_entry& _offset, vmclass* _to)
	{
		if (this == _to)
		{
			return	true;
		}
		vmstring	memberName	= vmstring(".") + _to->name();
		auto*		tbl			= variable_info_table();
		for (auto& vi : make_bucket(tbl->variables, tbl->variables + tbl->variable_size))
		{
			if (amtrs_string_to_string_view(vi.name) == memberName)
			{
				_offset.clazz		= this;
				_offset.name		= amtrs_string_to_string_view(vi.name);
				_offset.position	= (int)vi.get_offset();
				return	true;
			}
		}
		return	false;
	}


	//! 指定する変数の vmobject の先頭からのオフセット座標を取得する。
	intmax_t variable_offset(const vmstring& _name, const vmstring& _thiz) const noexcept
	{
		intmax_t		offset	= 0;
		if (!_thiz.empty())
		{
			// override した関数は this が base class を指しているのでオフセットを計算する。
			offset	= -variable_offset(vmstring(".vtable<") + _thiz + ">*", {});
		}
		auto*		tbl			= variable_info_table();
		for (auto& v : make_bucket(tbl->variables, tbl->variables + tbl->variable_size))
		{
			if (amtrs_string_to_string_view(v.name) == _name)
			{
				return	v.get_offset() + offset;
			}
		}
		for (auto& v : mStaticVariables)
		{
			if (v.name == _name)
			{
				return	v.position + offset;
			}
		}
		return	0;
	}

	bool created_destractor() const noexcept { return mDestractor != nullptr; }

	void set_destractor(vmfunction* _f)
	{
		mDestractor	= _f;
	}

	vmfunction* get_destractor();

	const auto& base_classes() const noexcept { return mBaseClasses; }

	auto variables() const noexcept { return mVariables; }

	const passion_struct_variables* variable_info_table() const noexcept { return member_variables; }

	//! オーバーライドしている場合も含めて関数が所属するベースクラスを取得する。
	vmclass* find_base_class(vmfunction* _func) noexcept
	{
		for (const auto& base : mBaseClasses)
		{
			if (auto c = base->find_base_class(_func); c)
			{
				return	c;
			}
		}
		auto	it	= std::find_if(mFunctions.begin(), mFunctions.end(), [&](vmfunction* _f) -> bool
					{
						return	_f == _func || _f->can_override(_func);
					});
		if (it != mFunctions.end())
		{
			return	this;
		}
		return	nullptr;
	}

	std::pair<vmstring, const declere_value*> get_variable_info(const vmstring& _variablename)
	{
		for (const declere_value& dec : mVariables)
		{
			if ((dec.name == _variablename) && !(dec.attribute & (uint32_t)attribute_flag::static_attribute))
			{
				return	{ name(), &dec };
			}
		}
		for (vmclass* c : mBaseClasses)
		{
			if (auto info = c->get_variable_info(_variablename); info.second)
			{
				return	info;
			}
		}
		return	{ {}, nullptr };
	}


	const declere_value* get_static_variable_info(const vmstring& _variablename)
	{
		for (const declere_value& dec : mVariables)
		{
			if ((dec.name == _variablename) && (dec.attribute & (uint32_t)attribute_flag::static_attribute))
			{
				return	&dec;
			}
		}
		return	nullptr;
	}

	const shared_buffer<declere_value>& static_variables() const noexcept { return mStaticVariables; }


	void build()
	{
		mStaticVariables	= make_static_variable_info();
		mVtableList.clear();
		struct	_
		{
			static std::pair<std::size_t, std::size_t> make(std::vector<declere_value>& _table, std::vector<vtable_info>& _vtable, vmclass* _clz, std::size_t _index, std::size_t _position, vmfunction* _destractor)
			{
				std::size_t		index		= _index;
				std::size_t		position	= _position;
				if (_clz->mBaseClasses.empty())
				{

					// ベースクラスなら、vmobj* へのポインタを暗黙的に挿入する
					std::size_t		ns		= sizeof(vmobject*);
					if ((position & (ns-1)) != 0)
					{
						// アライメント
						position = (position + ns - 1) & ~(ns-1);
					}

					auto*	v		= passion_vtable_create(1);
					v->functions[0]	= _destractor->function();
					v->base_offset	= vmobject::top_offset() + position;
					_vtable.push_back({v, 1});

					_table.push_back({
						vmstring(".vtable<") + _clz->name() + ">*",
						vmstring(".vtable<") + _clz->name() + ">*",
						0,
						position
					});
					++index;
					position += ns;
				}
				else
				{
					for (vmclass* c : _clz->mBaseClasses)
					{
						std::vector<vtable_info>	vti;
						auto [idx, pos] = make(_table, vti, c, index, position, _destractor);
						index		= idx;
						position	= pos;
						for (auto& v : vti)
						{
							_vtable.push_back(v);
						}
					}
				}

				// 変数テーブル
				for (auto& dec : _clz->mVariables)
				{
					if (dec.attribute & (uint32_t)attribute_flag::static_attribute)
					{
						continue;
					}
					std::size_t	ns	= passion_size_of_without_structure(dec.type);
					if ((position & (ns-1)) != 0)
					{
						// アライメント
						position = (position + ns - 1) & ~(ns-1);
					}
					_table.push_back({
						dec.type,
						dec.name,
						dec.attribute,
						position,
					});
					++index;
					position += ns;
				}


				// 仮想関数テーブル
				for (passion_function* f : _clz->function_list())
				{
					if (!(f->attributes & virtual_attribute))
					{
						continue;
					}

					bool			have	= false;
					for (auto& vt : _vtable)
					{
						for (int i = 1; (std::size_t)i < vt.size; ++i)
						{
							// 同じ関数が基底クラスで宣言されているならテーブルには追加しない。
							auto*	tf	= vt.vtbl->functions[i];
							if (tf->function_name == f->function_name && tf->signature == f->signature)
							{
								have					= true;
								vt.vtbl->functions[i]	= f;
								break;
							}
						}
					}
					if (!have)
					{
						auto&		vti			= _vtable.front();
						auto*		v			= passion_vtable_create(vti.size + 1);
						v->base_offset			= vti.vtbl->base_offset;
						std::copy(vti.vtbl->functions, vti.vtbl->functions + vti.size, v->functions);
						v->functions[(int)vti.size]	= f;
						vti.vtbl	= v;
						vti.size	= vti.size + 1;
					}
				}

				return	{index, position};
			}

		};


		std::vector<declere_value>	structVariables;
		mStructSize		= _::make(structVariables, mVtableList, this, 0, 0, get_destractor()).second;


		passion_struct_variables*	vars	= passion_struct_variables_create(structVariables.size());
		passion_variable_info*		cur		= vars->variables;
		for (auto& v : structVariables)
		{
			cur->type					= amtrs_string_create_from_string_view(v.type);
			cur->name					= amtrs_string_create_from_string_view(v.name);
			cur->offset.static_size		= static_cast<uint16_t>(v.position);
			cur->offset.int_count		= 0;
			++cur;
		}

		this->member_variables	= vars;
	}


	int get_vtbl_id(vmfunction* _function)
	{
		for (auto& vti : mVtableList)
		{
			int	i	= 0;
			for (auto* f : make_bucket(vti.vtbl->functions, vti.vtbl->functions + vti.size))
			{
				if ((f->function_name == _function->function()->function_name)
				 && (f->signature == _function->function()->signature))
				{
					return	i;
				}
				++i;
			}
		}
		return	-1;
	}


	const native_conf& native() const noexcept { return mNativeConf; }

	void set_variables(const declere_value* _first, std::size_t _size)
	{
		mVariables.assign(_first, _first + _size);
	}

	void initvtable(void* _vmobject)
	{
		for (auto& vd : mVtableList)
		{
			*reinterpret_cast<passion_vtable**>((reinterpret_cast<std::intptr_t>(_vmobject) + vd.vtbl->base_offset))	= vd.vtbl;
		}
	}

protected:

	shared_buffer<declere_value> make_static_variable_info()
	{
		std::size_t					index		= 0;
		std::size_t					position	= 0;
		std::vector<declere_value>	retval;
		for (auto& dec : mVariables)
		{
			if (!(dec.attribute & (uint32_t)attribute_flag::static_attribute))
			{
				continue;
			}
			std::size_t	ns	= sizeof(vmobject*);
			if ((position & (ns-1)) != 0)
			{
				// アライメント
				position = (position + ns - 1) & ~(ns-1);
			}
			retval.push_back({
				dec.type,
				dec.name,
				dec.attribute,
				position,
			});
			++index;
			position += ns;
		}
		return	{ retval.begin(), retval.end() };
	}




	vmstring										mClassName;
	std::vector<vmclass*>							mBaseClasses;		//!< 継承しているクラス
	std::size_t										mStructSize;		//!< インスタンス化されたオブジェクトの大きさ(refcounter, mVtable* を含む)

	std::vector<ref<vmfunction>>					mFunctions;
	native_conf										mNativeConf;		//!< ネイティブクラスに関する情報
	ref<vmfunction>									mDestractor;


	shared_buffer<declere_value>					mVariables;			//!< メンバ変数の定義
	shared_buffer<declere_value>					mStaticVariables;	//!< ビルド済みの静的変数リスト

	std::vector<vtable_info>						mVtableList;

private:
	friend	vmclass_manager;
	friend	vmvtable;
};



inline vmfunction* vmclass::find_function(const vmstring& _name, std::size_t _argSize) const noexcept
{
	auto	it	= std::find_if(mFunctions.begin(), mFunctions.end(), [&](vmfunction* _func) -> bool
				{
					if (_name != _func->name())
					{
						return	false;
					}
					if (_argSize == _func->parameter_size())
					{
						return	true;
					}
					if (!_func->is_variadic() || _argSize < _func->parameter_size())
					{
						return	false;
					}
					return	true;
				});
	if (it != mFunctions.end())
	{
		return	*it;
	}

	for (const auto& base : mBaseClasses)
	{
		auto	f = base->find_function(_name, _argSize);
		if (f)
		{
			return	f;
		}
	}

	return	nullptr;
}


PASSION_NAMESPACE_END
#endif
