# Passion Validated Binary

validate が完了したプログラムをバイナリ化したもの。
実行したりモジュールとしてロードすることができる。

## Format

ファイル内の構造は 32bit リトルエンディアンを基準にしています。

### 基本型

|Name|Size|Note|
|--|--|--|
|CHAR|1|文字型|
|WORD|2|整数|
|DWORD|4|整数|
|STRING|>4|文字列<br />DWORD size; CHAR[size+1]; |

### pvb::fileheader

|Addr|Type|Name|Note|
|--|--|--|--|
|+0000H|CHAR[4]|identify|PVB1 固定|
|+0004H|DWORD|file_size|ファイルの大きさ|
