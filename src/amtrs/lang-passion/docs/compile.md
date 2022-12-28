# compile

psnvm には大きく分けて４つのプログラム言語が存在します。フロントエンドを作成する場合はこれら４つの言語のうちどれかを出力すれば psnvm で実行できるようになります。

1. psn (passion language)
2. pfl (passion functional language)
3. pvl (passion validated language)
4. pal (passion assembly language)
5. pvm (passion virtual machine code)

## 1. passion (passion language)

c++ によく似たプログラム言語です。人が書くことを想定した言語です。

## 2. pfl (passion functional language)

lang passion を構文解析した結果を示すノードです。関数型言語のようなイメージです。型のチェックなどは行われません。

```lisp
(class `Cls` (function `Cls::Cls` `()` (compound (return))))
```

## 3. pvl (passion validated language)

syntax node から型の整合性など調整されたノード情報です。syntax node に似た関数型言語のような構造になっています。

```lisp
(class `Cls`)
(function `Cls::Cls` `()` [(return)])
```

## 4. pal (passion assembly language)

pvmと一対一のアセンブリ言語です。

## 5. pvm (passion virtual machine code)

仮想マシンが実行可能な仮想マシン語でバイナリ形式です。アセンブリ言語のような構造です。

## psnvmをバックエンドに使用する

プログラマーは任意のプログラム言語からsyntax node や validated node を生成することができれば、psmvmをバックエンドとして実行可能可能です。
