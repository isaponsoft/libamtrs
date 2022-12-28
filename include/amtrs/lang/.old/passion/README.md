

```
int main(int argc, char** argv)
{
	int		n	= 0;

	return	0;
}
```

# psnfl

psnflは関数型プログラミング言語風の中間コードになります。人が書くことは想定しておらず基本的にはプログラムでパースしやすい構造を優先してデザインしています。式や文は必ず ```()``` で囲む必要があり、```()```を切り抜くことで比較的簡単にパース出来ます。

式自体はアセンブラに近くpsnfl上では型のチェックなどは最低限に留められます。従って、値のキャストや ```operator 関数``` への呼び出しの変換などは行われません。

## 基本書式

基本的な書式形式は以下の５つだけです。

### (1) C = ```(COMMAND C|N C|N ...)```

COMMANDで指定するノードを生成します。複数の式を持ちます。

### (2) N = ``` `name` ```

名前またはキーワードです。

### (3) NT = ``` `name`:`C|N` ```

名前と式の組み合わせです。 ```:``` の前後に他の文字をはさんではなりません。

### (4) NUM = ```$number```

数値リテラルです。

### (5) STR = ```"string"```

文字列リテラルです。エスケープ記号としてバックスラッシュが使用できます。

## ノード

* I = \`identify\`
* N = $number
* S = "string"
* T = N|(decltype)
* P = I:T



```
(function
	`main`:`int`
	(parameters `argc`:`int` `argv`:`char**`)
	(local	`n`:`int`)
	(store	`n` $0)
	(return $0)
)
```
## シンボル

関数、変数、クラス、エイリアス、名前空間が存在する。名前空間の区切り記号は ```/``` になる。passion lang 上では ```::``` で示されるため異なることに注意すること。



## COMMAND

### class

```
(class `class-name` [(extends)] [member-expr...])
```

### extends

```
(extends `class-name`:(extendsattr)...)
```

### extendsattr

```
(extendsattr `private`|`protected`|`public`)
```


### template

```
(template (parameters) (function|class))
```

### function, functionm

関数の定義を行います。```functionm```はthisポインタを持つ関数です。

```
(function|functionm `function-symbol`
	[(parameters)]
	[statement-expr...]
)
```

```function-symbol``` と ```parameters``` の引数パラメータの型は一致していないる必要があります。異なる場合は正しく実行することができないコードが生成される場合があります。


```function-symbol``` は以下のような組み合わせでも構いません。

```lisp
(function ```main```:```int(int:char***)```)
(function ```main```:(signature ```int``` ```int``` ```char***```))
(function ```main```:(signature (decltype $1) (decltype $1) ```char***```))
```

### signature

関数のシグネチャを生成します。正規化すると```identify``` に置き換えられます。

```
(signature result-type parameter-type...)
```

|signature|identify|
|--|--|
|```(signature `int` `short` `MyClass*`)```|``` `int(short:MyClass*)` ```|
|```(signature `void`)```|``` `void()` ```|



### call, callm

```call``` thisポインタを持たない関数の呼び出しを行います。```callm``` thisポインタを持つ関数の呼び出しを行います。

```
(call `function-name` (signature)
 	[arg-expr...]
)
```


thisポインタを持つ関数（メンバ関数）の呼び出しを行います。

```
(callm `function-name` (signature)
	this-expr
	[arg-expr...]
)
```


example) ```(call `main`:`int?int:char**` $1 $0)```


### add,sub,div,mul...

２項目の数を演算します。それぞれが即値かスカラまたは参照である必要があります。オブジェクトへの参照の場合、```operator +```などの関数呼び出しに置き換えられることがあります。またそれぞれの式の結果は整数データにキャストできる必要があります。

```
(add left-expr right-expr)
```

即値とスカラ以外の演算の場合は以下のように置き換えられます。

```
(call `operator +`:`int?int:int`
	left-expr			# this-expr
	right-expr			# arguments
)
```

### load

`address-expr` で指定するメモリからスカラをロードします。

```
(load address-expr)
```

### signature

関数のパラメータ型を示します。

```
(signature `type`...)
```

### store

`address-expr` で指定するメモリへ `expr` の値を書き込みます。`expr` はスカラである必要があります。

```
(store address-expr expr)
```

## toint

値を浮動小数から整数に変換します。

```
(toint exp)
```

## tofloat

値を整数から浮動小数に変換します。

```
(tofloat exp)
```

