# compati

STL(C++17)の互換ライブラリです。一部プラットフォームで実装に不具合があったり未実装だったりする場合に代用するためのものです。（推奨はしませんが）c++14環境でc++17ライブラリを使用したい場合などにも代用できます。

amtrs/filesystem/@ を include した場合はSTLのものを使うか互換ライブラリを使うか環境に合わせて自動的に選択します。意図的に互換ライブラリを使用したい場合はコンパイラオプションのINCLUDE_PATHに libamtrs/include/compati-stl/対象のクラス を追加してください。

例)互換ライブラリを使用する

```shell
${CC} -std=c++17 -I libamtrs/include -I libamtrs/include/compati-stl/filesystem source-code
```

