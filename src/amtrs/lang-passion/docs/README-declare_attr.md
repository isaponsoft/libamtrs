# template arguments

以下はc++では許可されるがpsnvmではエラーとなる

```c++
class	Cls
{
public:
	// syntax error psnvm
	char static const inline f();
};
```

psnvmでは以下のようにする。

```c++
class	Cls
{
public:
	static inline char const f();
};
```
