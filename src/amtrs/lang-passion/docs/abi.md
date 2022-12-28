# ABI

## managed pointer

managed pointer は vnew を行った際にvmmheadを付与した状態でメモリが確保される。例えば ```vnew int()``` を行った場合、```sizeof(vmmhead) + sizeof(int)``` が実際に確保されるメモリになる。

```c++
struct	vmmhead
{
	size_t	refcount;	// 参照カウント
	void*	cur;		// メモリの参照位置
	size_t	length;		// length of without vmmhead.
};
```

### cast

```c++
struct	Base1;
struct	Base2;
struct	Extends : Base1, Base2;
```

この場合

```vmmhead Base1 Base2 Extends``` というようにメモリが確保される。```cur``` には ```Extends(Base1と同等)``` のアドレスが格納される。
 
```Base2``` ヘキャストすると、```+sizeof(Base1)```されるが、このアドレスが```cur```へ格納される。

