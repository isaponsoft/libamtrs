# Virtual machine

PSNVMとは特定のアーキテクチャに依存しない仮想マシンフォーマットです。

* 3個の専用レジスタと13個の汎用レジスタを持っています。
* REG[0] は常に 0 を指します。
* REG[1] はスタックポインタです。
* REG[2] はベースポインタです。

## OPECODE FORMAT

仮想マシンはワード単位(2bytes)の命令セットを持っています。

||D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|+00H|SF|ZF|>|FMT|M|>|>|CODE|
|+01H|>|>|>|>|>|>|>|OPERAND|

* SF : 符号フラグ。この値と符号フラグが一致する場合のみ命令を実行する。
* ZF : ゼロフラグ。この値とゼロフラグが一致する場合のみ命令を実行する。
* FMT : 命令長。
* M : モード。命令によって異なる。
* CODE : 命令番号。
* OPERAND : 命令によって異なるパラメータ。

|FMT|FORMAT|
|--|--|
|0|なし。16bitの命令である。|
|1|1ワードの即値が続く32bit命令である。|
|2|6ワードの即値が続く64bit命令である。|
|3|14ワードの即値が続く128bit命令である。|

|CODE|CATEGORY|COMMAND|
|--|--|--|
|0|IMMIDIATE|INTEGER|
|1|ACCESS|LOAD INTEGER, STORE INTEGER|
|2|OPERATION|ADD,SUB,MUL,IMUL,DIV,IDIV,MOD,IMOD|
|^|^|AND,OR,XOR,SHL,SHR,SAR|
|^|^|ABS,NEG,NOT,L_OR,L_AND|
|3|BRANCH<br />STACK|JMP,JC,JNC,JE,JNE,JCE,JNCE,JNCE,JNCNE|
|^|^|CALL,CALLREG,RET,EXCEPTION|
|^|^|PUSH,POP,PUSHEXCEPTION,POPEXCEPTION|
|4,5|未定義|
|6|EXTRA|FETCH,YIELD,INTRPT|
|7|MODE|PADDING,BITSIZE,FLOAT,SIMD|

## 共通項目

### SIZE

通常 3bit の値で表される。

|SIZE|LENGTH|
|:---:|:---|
|0|8bit|
|1|16bit|
|2|32bit|
|3|64bit|
|4-5|未定義|
|6|実行環境に依存する int|
|7|実行環境に依存する intmax_t|

浮動小数モードでは次のように扱われる

|SIZE|LENGTH|
|:---:|:---|
|0|32bit|
|1|64bit|
|3-7|未定義|

SIMD(64, 128)モードでは次のように扱われる

|SIZE|LENGTH|
|:---:|:---|
|0|8bit x N|
|1|16bit x N|
|2|32bit x N|
|3|64bit x N|
|4|128bit x N|
|5-7|未定義|


## OPECODE 0 : IMMIDIATE

```
IF M == 1 THEN LENGTH = 1 ^ SIZE
IF M == 1 THEN REG1 = SIZE DATA[ IMM ]
IF M == 0 THEN REG1 = IMM
```

||D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|+00H|SF|ZF|M|>|FMT|0|0|0|
|+01H|>|>|>|REG1|>|>|>|IMM|

|M|Details|
|:--|:--|
|0|IMMの値は即値である|
|1|IMMの値は SIZE として扱う|


## OPECODE 1 : ACCESS

REG1 に対してオペランドで指定されるアドレスの値を読み取ります。読み込む値のサイズは SIZE によって決定されます。M = 1 の場合は読み込みではなく書き込みになります。この命令は FMT = 0 の動作は未定義。

```
LENGTH  = 1 ^ SIZE
IF SCL == 0 THEN ADDRESS = REG1 + (REG2 + OFFSET) * LENGTH
IF SCL == 1 THEN ADDRESS = REG1 + REG2 + OFFSET
IF M == 0 THEN REG1 = [ ADDRESS ]
IF M == 1 THEN [ ADDRESS ] = REG1
```

||D7|D6|D5|D4|D3|D2|D1|D0|NOTE|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|+00H|SF|ZF|>|FMT|M|0|0|1|-|
|+01H|>|>|SIZE|SCL|>|>|>|REG1|-|
|+02H|>|>|>|REG2|>|>|>|REG3|FMT >= 1|
|+03H|>|>|>|>|>|>|>|OFFSET|FMT >= 1|
|+04H|>|>|>|>|>|>|>|OFFSET|FMT >= 2|
|+05H|>|>|>|>|>|>|>|OFFSET|FMT >= 2|
|+06H|>|>|>|>|>|>|>|OFFSET|FMT >= 2|
|+07H|>|>|>|>|>|>|>|OFFSET|FMT >= 2|

|SIZE|LENGTH|
|:---:|:---:|
|0|8bit|
|1|16bit|
|2|32bit|
|3|64bit|
|4-7|未定義|

|SCL|計算|
|:---:|:---|
|0|アドレス計算時に REG3 + OFFSET に LENGTH を掛ける|
|1|アドレス計算時に REG3 + OFFSET に LENGTH の値をそのまま使う|


### OPECODE 2 : COMPUTE

REG2 と REG3 を指定された計算方法で計算し、結果をREG1に格納します。また CF, ZF を変更します。

||D7|D6|D5|D4|D3|D2|D1|D0|NOTE|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|+00H|SF|ZF|0|1|M|0|1|0|-|
|+01H|>|>|>|>|>|>|>|OPECODE|-|
|+02H|>|>|>|REG1|>|>|>|REG2|-|
|+03H|>|>|>|>|>|>|>|-|-|


### OPT:0 INTEGER

|OPERATION|OPERATION|EXPR|
|--|--|--|
|0|ADD|REG1 = REG2 + REG3|
|1|SUB|REG1 = REG2 - REG3|
|2|MUL|REG1 = REG2 * REG3|
|3|IMUL|REG1 = REG2 * REG3|
|4|DIV|REG1 = REG2 / REG3|
|5|IDIV|REG1 = REG2 / REG3|
|6|MOD|REG1 = REG2 % REG3|
|7|IMOD|REG1 = REG2 % REG3|
|8|BIT AND|REG1 = REG2 & REG3|
|9|BIT OR|REG1 = REG2 \| REG3|
|10|BIT XOR|REG1 = REG2 ^ REG3|
|11|LOGIC AND|REG1 = REG2 && REG3|
|12|LOGIC OR|REG1 = REG2 \|\| REG3|
|13|LEFT SHIFT|REG1 = REG2 << REG3|
|14|RIGHT SHIFT|REG1 = REG2 >> REG3|
|15|RIGHT SHIFT SIGNED|REG1 = REG2 >> REG3|









