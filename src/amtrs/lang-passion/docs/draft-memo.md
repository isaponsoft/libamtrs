# a

## vnew : 寿命管理されたオブジェクト

newの代わりに vnew を使うと参照カウンタによって管理されたメモリが生成される。ポインタは ```*``` の代わりに ```+``` を使用する。

```c++
class	Cls{ ...略... };
(1) Cls+   o = vnew Cls();
(2) Cls*   o = new Cls();
(3) Cls[]+ o = vnew Cls[size];
(4) Cls*   o = new Cls[size];
```

vptr から ptr へのキャストは出来るが、その逆は不可能。

```c++
Cls+ vptr;
Cls* ptr;

ptr = vptr; // OK
vptr = ptr; // Syntax error
```

### メモリの構造

|ADDR|TYPE|NOTE|
|--|--|--|
|-03|function**|関数テーブルのポインタ|
|-02|uintmax_t|参照カウント|
|-01|void*|メモリの終端位置|
|+00||オブジェクトの先頭|

### vptrの構造

vptrは２つのポインタを内包する。つまり ```sizeof(Class+) == 2 * sizeof(Class*)``` になる。以下のような構造になる。

```c++
template<class Class>
struct	vptr
{
	Class*	obj;		// キャストされたオブジェクトの先頭
	void*	mem;		// メモリブロックの先頭

	Class* operator -> () { return obj; }
	size_t size() { return ((mem - sizeof(Head))->end - mem) / sizeof(Class);
	Class& operator ++ () { ++obj;  return *this; }
};
```

また vptr はイテレータのように使用することもできる。特に指定されない場合は範囲チェックが行われ、範囲外になった場合は null になります。

```c++
Cls[]+	ary;
++ary;
```



## new を文字列で指定して実行する

```c++
class	Hello : public BaseClass
{
public:
	// コンストラクタを refrection 属性で有効にしておくと new で
	// 動的呼び出しが可能になる。refrectionに渡すパラメータは
	// new が返すオブジェクトの型になる。
	[[psn::refrection(BaseClass)]]
	Hello(int _a, int _b)
	{
	}
};

string		classname	= "Hello";

(1) BaseClass* base = new <BaseClass(int, int)> (classname, 1, 2);
(2) BaseClass+ base = vnew <BaseClass(int, int)> (classname, 1, 2);
(3) void*      place = malloc(sizeof(Hello));
    BaseClass* base  = new (place) <BaseClass(int, int)> (classname, 1, 2);
```


## new 演算子の使用を強制する

```c++
[[psn::place(new, vnew)]]
struct	new_onlyclass
{};

// OK
new_onlyclass*	n	= new new_onlyclass();
new_onlyclass+	n	= vnew new_onlyclass();

// Syntax error
new_onlyclass	o;

// Syntax error
struct	myclass
{
	new_onlyclass	o;
};
```
