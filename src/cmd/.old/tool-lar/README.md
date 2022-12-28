# Lite ARchive

主にモバイルアプリや組み込み、ゲームなどでの仕様を想定した軽量なアーカイブファイルフォーマットです。ファイルのヘッダ情報がアーカイブの先頭に集中するようになっています。

## 特徴

### 小さなヘッダ

ヘッダにはファイルサイズとファイル名だけしかありません。とてもコンパクトです。半面、各OSのファイルシステムにおける所有ユーザーやパーミッションといった情報を含めることはできません。

### 先頭部分だけ読めばよい

どのようなファイルが含まれるかはアーカイブの先頭部分を読み込むだけで完了します。ファイルの最後尾まで読み取る必要はありません。

## 構造

ファイルは ```Header```, ```Entries```, ```Values``` の３つのブロックに分かれます。

ざっくり説明すると以下のような構造です。

```c++
Header	header;
Entry	entries[header.header_size];
[padding 16bytes]
Value	values[header.filenum];

Header{
	Signature[4]	"LAR1"
	U32LE			header_size
	LAR_SIZE_T		entry_size
	LAR_SIZE_T		value_size
	LAR_SIZE_T		file_num
}

Entry{
}

Value{
	bytes[Entry.compress_size]
	[padding 16bytes]
}


LAR_SIZE_T{
	One{
		U1BIT		continue;
		U7BIT		size
	} []
}

```

### LAR_SIZE_T

ファイルサイズなどの数値を保存するための基本型です。理論上は無限の数値を扱うことが出来ます。数値を読み込むときは1バイトずつ読み取ります。そのうちの7bitが有効な数値で、1bitは継続して値を読み込むかどうかを示すフラグになります。

つまり127以下の数値であれば1byteで保存できます。

```c++
do
{
	tmp = read8bit();
	num = (num << 7) + (tmp & 0x7f);
} while (tmp&0x80)
```


### Header

header はシグネチャ4バイト、ヘッダサイズ4バイトの合計8バイト。
それから、ファイルエントリのサイズ、データブロックのサイズ、ファイルの数が可変長（最低3バイト）からなります。

最も小さいときは11バイトになります。

```c++
header.signature = "LAR1";
header.header_size =
	sizeof(header.entry_size) +
	sizeof(header.value_size) +
	sizeof(header.file_num);
header.entry_size	= LAR_SIZE_T(entry block size);
header.value_size	= LAR_SIZE_T(value block size);
header.file_num		= LAR_SIZE_T(file count);
```

### Entry

ファイル名と各ファイルの大きさを格納します。header.file_numの数だけ存在し、その合計はentry_sizeバイトに一致します。

```c++
entry.compress_size	= LAR_SIZE_T(compressed size);
entry.original_size	= LAR_SIZE_T(de-compressed size);
entry.name_size	= LAR_SIZE_T(name size);
entry.flags		= compresstype & 0x3f;
entry.name		= CHAR[name_size];
```



## コマンドラインツール

```shell
lar [OPTIONS] SOURCEFILES...
```


larは空のディレクトリは保存しません。

### 展開

カレントディレクトリにファイルを展開します。

```shell
lar --extract archive.lar
```

アーカイブファイルのみを指定した場合、アーカイブのあるディレクトリにサブディレクトリを作成し、その中にファイルを展開します。

```shell
lar archive.lar archive2.lar archive3.lar
```

### 圧縮

```shell
lar --create archive.lar file1 file2...
```

ソースファイルのあるディレクトリ名と同じ名前の lar が作られます。

```shell
lar file1 file2...
```

