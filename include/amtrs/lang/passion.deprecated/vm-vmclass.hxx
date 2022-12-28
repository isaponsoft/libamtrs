/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__vm_vmclass__hpp
#define	__libamtrs__lang__passion__vm_vmclass__hpp
PSNVM_NAMESPACE_BEGIN

struct	vmclass
{
	static constexpr size_t	none	= static_cast<size_t>(-1);

	// メンバ変数
	//	メンバ変数の初期化式はコンパイラ側の情報であり、
	//	仮想マシンのクラス情報には含まれない
	struct	mvariable
	{
		unsigned int	access;
		sstring			type;
		sstring			name;
	};

	// メンバ関数
	struct	mfunction
	{
		unsigned int	access;
		function*		func		= nullptr;
	};

	struct	extends
	{
		sstring			type;
		unsigned int	access;
	};

	sstring					name;
	simplearray<extends>	base;			// ベースクラス
	simplearray<mvariable>	member_var;		// メンバ変数
	simplearray<mfunction>	member_func;	// メンバ関数

	/*
	 * メンバ変数を検索します。
	 */
	mvariable* find_var(text _name) noexcept;

	/*
	 * メンバ関数を検索します。最初に一致する関数を返します。
	 * 主に同名の関数があるかどうかを調べるために使用します。
	 */
	mfunction* find_func(text _name) noexcept;
};

class	vmclassmgr
{
public:
	static constexpr size_t	none	= static_cast<size_t>(-1);

	vmclassmgr(vmclassmgr* _parent);
	~vmclassmgr();

	vmclassmgr() = default;
	vmclassmgr(vmclassmgr const&) = delete;
	vmclassmgr(vmclassmgr&& r) noexcept
		: mParent(r.mParent)
		, mClasses(std::move(r.mClasses))
	{
		r.mParent	= nullptr;
	}
	vmclassmgr& operator =  (vmclassmgr const&) = delete;
	vmclassmgr& operator =  (vmclassmgr&& r) noexcept
	{
		mParent		= r.mParent;
		mClasses	= std::move(r.mClasses);
		r.mParent	= nullptr;
		return	*this;
	}

	/*
	 * クラスを登録する
	 */
	void add(vmclass _cls);

	/*
	 * 破棄する
	 */
	void clear();

	/*
	 * クラス名を指定して検索する
	 */
	vmclass* find(text _fullname) noexcept;

	/*
	 * 親のマネージャへ移動します。
	 */
	void move_to_parent();

	/*
	 * クラス名または型名から必要なメモリサイズを計算します。
	 * 名前が不正だったり見つからない場合は none を返します。
	 */
	size_t datasize(text _fullname);

	/*
	 * 指定するメンバ変数のオフセット値を計算します。
	 * 名前が不正だったり見つからない場合は none を返します。
	 * _fullname
	 *		正式な名前 example) AA::BB:CC::DD::a
	 */
	size_t offset(text _fullname);

	/*
	 * 指定するメンバ変数のオフセット値を計算します。
	 * 名前が不正だったり見つからない場合は none を返します。
	 * _name
	 *		メンバの名前
	 */
	size_t offset(vmclass* _cls, text _name);


protected:
	vmclassmgr*				mParent	= nullptr;
	simplearray<vmclass>	mClasses;
};

PSNVM_NAMESPACE_END
#endif
