# template arguments

c++ と異なりプロトタイプが無いためテンプレートの実引数の解釈が異なる部分があります。
テンプレート実引数は左の式を最長一致になるように優先的に解釈されます。

例えば以下の式を例にとります。

## patturn A

```A<B<C>```

この式はパターンの一致条件を無視するなら以下の２パターンの解釈が可能です。

(1) AがテンプレートでBCが比較式

```c++
template<class> class A;

x	= B < C;
y	= template A<x>
```

(2) Bがテンプレートで AとBの比較式

```c++
template<class> class B;

x	= template B<C>;
y	= A < x;
```

しかし lang passion では左にある項目である ```A``` がテンプレートであることを優先します。従って先の例では (1) が採用されます。

## prototype

c++では前方宣言されたクラスや関数の実体を書くとき、次のように template 文を連ねて書く必要があります。

```c++
template<class A>
struct	foo
{
	template<class B>
	struct	bar
	{
		template<class C>
		int f();
	};
};

template<class A>
template<class B>
template<class C>
int foo<A>::bar<B>::f() { ... }
```

lang passion では省略します。

```c++
template<class A>
struct	foo
{
	template<class B>
	struct	bar
	{
		template<class C>
		int f();
	};
};

int foo<A>::bar<B>::f() { ... }
```
