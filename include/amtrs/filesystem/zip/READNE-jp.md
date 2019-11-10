# filesystem::zip

## 概要

[zipファイル](https://ja.wikipedia.org/wiki/ZIP_(%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB%E3%83%95%E3%82%A9%E3%83%BC%E3%83%9E%E3%83%83%E3%83%88))を疑似的なファイルシステムとして扱うためのライブラリ群で、std::filesystemに似たような作りにしています。

## 圧縮・伸長

filesystem::zipはあくまでも疑似ファイルシステムであり圧縮されたデータについては一切関与しません。圧縮されたデータを扱う場合はcompress::deflate を併用します。

## 関数

### zip::open

```c++
template<class IN>
zip_archive<IN> open(IN _in);
```

第１引数で指定される入力のファイルを開きzip_archiveを返します。ファイルが開けなかった場合は空のアーカイブを返します。





## サンプル

```
#include <amtrs/filesystem/zip/@>

for (auto e : amtrs::filesystem::zip::open(std::fstream("test.zip")))
{
    std::cout << e.filename() << std::endl;
}

```

