# amtrs::io::read()

1. ```read(char_type* dest, STREAM& in, size_t size)```
2. ```read(char_type (&dest)[N], STREAM& in)```
3. ```read<DEST>(STREAM& in, size_t size)```

in からデータ読み取ります。

1. sizeで指定される量のデータを読み取ります。
2. ```read(&dest[0], in, N)``` と同等。配列に対するエイリアスです。
3. DESTで示すクラスのオブジェクトを生成し、その中にデータを読み込んで返します。DESTで示す型はresize()関数とdata()関数をサポートする必要があります。

## Parameters

|name|node|
|--|--|
|dest|読み取ったデータを格納するバッファ。|
|size| 読み取るデータの要素数。|
|in|読み取り元のストリームオブジェクト。|


## Return value

size_type 実際に読み込みを行った要素数を返します。

## Complexity

## Exceptions

none

## Notes

## Example

```c++
(1)
char buf[100];
io::read(buf, in, 100);

(2)
char buf[100];
io::read(buf, in);

(3)
std::string s = io::read<std::string>(in, size);
std::vector<char> s = io::read<std::vector<char>>(in, size);
```


## write

```c++
char buf[100];
io::write(out, buf, 100);

char buf[100];
io::write(out, buf);

io::write(out, std::string_view("Test"));
```

## size

```c++
size_t s = io::size(in);
```
