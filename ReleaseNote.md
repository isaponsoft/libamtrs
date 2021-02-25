
# 2021/02/25

addlicense公開に合わせて大幅アップデート。


# 2019/11/11

プログラムの整理と拡張。

## libamtrs

* STL互換ライブラリを compati-stl へ移動(主に xcode など c++17対応が不完全なプラットフォームで使用)
* ioライブラリにいくつかの機能を追加、iostreamを廃止予定
* cryptoにcrc32を追加
* filesystem/zip/zipfs を追加
* filesystem にファイルパス関数を追加
* memory/shared_bufferをallocatorに対応
* net/http/parser を追加
* net/socket/socket, socket_stream, ssl_stream を追加
* net/uri を追加
* opt/io/zlib-stream_in を追加(need zlib)

# 2019/9/27

xcode11対応と細かい不具合の修正。

## libamtrs

* compati/varint, compati/filesystem が xcode11 で問題が出ていたいのを修正
* sound create時にフォーマットを指定するように変更
* 不要な include を一部解消

# 2019/9/23

- ReleaseNoteをMarkdownへに変更。

## libamtrs

* フォルダ構成を整理。

* docsにドキュメントを追加。

# 2019/9/19

* とりあえずプロジェクトのバックアップも兼ねて公開。
## libamtrs
* libamtrsを使ったアプリをリリースしたので
* 一応アプリを作れるくらいにはなった。
	
## passion
* いろいろ想定している仕様が入っていないが
* まぁなんとなくこんな言語というのを触れるようにはなった。
	
## inarikvs
* プロジェクトには含まれているが最近メンテしていないので
* コンパイルできるかも微妙。バックアップ目的での公開です。

