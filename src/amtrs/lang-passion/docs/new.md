# new, delete

psnvmでは new 以外にも vnew が使用できます。vnewはメモリアクセスを安全に行うことができる管理されたポインタです。c++ではvmpテンプレートで記載できます。

```c++
C^			o	= vnew C();
vmp<C>		o	= vmp<C>::vnw();
```

```c++
C[]^		a	= vnew C[size]();
vmp<C[]>	a	= vmp<C[]>::vnw(size);
```
管理されたポインタは範囲外アクセスを行うとすると例外を発行します。
