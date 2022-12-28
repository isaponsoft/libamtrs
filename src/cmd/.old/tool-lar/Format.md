```c++
Header	header;
Entry	entries[header.file_num];
Value	values[header.file_num];

Header{
	Signature[4]	"LAR1"
	U31LE			header_size
	U1				zero_reserved
	LAR_SIZE_T		entry_size
	LAR_SIZE_T		value_size
	LAR_SIZE_T		file_num
}

Entry{
	LAR_SIZE_T		compress_size
	LAR_SIZE_T		original_size
	LAR_SIZE_T		name_size
	U8				flags
	CHAR			name[name_size]
}

Value{
	bytes[Entry.compress_size]
}


LAR_SIZE_T{
	U8	num[]
}

U31LE		31bit little endian unsigned int.
U8			8bit unsigned int.
U1			1bit.

FILESIZE	Header.header_size + Header.entry_size + Header.value_size
```

### LAR_SIZE_T


```c++
do
{
	tmp = readU8();
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
lar [OPTIONS] (create|append|extract) LARFILE FILES...
```


larは空のディレクトリは保存しません。
