# Application Project Generator

Android, iOSのクロスプラットフォームビルドを行うためのコマンドです。cmakeの補助的なツールという位置づけです。

## コマンドのビルド

Windowsを例に、 apgen コマンドをビルドします。完成したコマンドは適当な場所にコピーしてください。またzlibとlibresselが必要になります。

```shell
DIR %LIBAMTRS%
libamtrs libressl zlib
MD build
CD build
cmake.exe %LIBAMTRS%\tools\apgen
msbuild.exe apgen.vcxproj
```

## プロジェクトの生成まで

## プロジェクト設定ファイルの作成

プロジェクトフォルダ（通常はCMakeLists.txtのあるフォルダ）にAPGenConf.txtを作成します。APGenConf.txtは英数字のキーと値で構成されます。先頭が#の場合はコメント扱いになります。また = の前後には空白とタブを任意に含めることができます。

```shell
FORMAT = 1
# Packagename
ANDROID:PACKAGENAME	= com.foo.bar.package.name
# Keystore
ANDROID:storeFile = android.keystore
ANDROID:storePassword = password
ANDROID:keyAlias = alias
ANDROID:keyPassword = password
```

### apgenによるAndroid Gradleプロジェクトの生成

```shell
mkdir build-android
cd build-android
%BIN%\apgen.exe -G "android gradle" %PROJECTDIR%
gradlew assembleDebug
```

## apgenの使い方

内臓テンプレートを使用してプロジェクトを生成します。

```shell
apgen -G "Generator" APGENCONF-DIRECTORY
```

## モジュールにパラメータを渡す

"-Dパラメータ" のように書くことで、APGenConf.txtの内容を上書きできます。

```shell
apgen -M "android-gradle" APGENCONF-DIRECTORY \
	-DGRADLE_URL=https://localhost/mygradle.zip
```

"-android-gradle:パラメータ" のように書いても構いません。

```shell
apgen -M "android-gradle" APGENCONF-DIRECTORY \
	-android-gradle:GRADLE_URL=https://localhost/mygradle.zip
```

後者は誤ったパラメータを渡したくない場合などに使用するとよいでしょう。



## 各種モジュールの個別説明

* [Android Gradle](AndroidGradle/Readme.md)

## 簡易機能

apgenの持つ機能のいくつかは限定的ではありますが単体で呼び出すことができます。

### ファイルをダウンロードする

指定したURLからファイルをダウンロードし、ファイルに保存します。参照先のURLがディレクトリの場合はホスト名をファイル名として使用します。

```shell
apgen -G download - -DURL=http://foo.bar.com/
```

### zipを展開する

```shell
apgen -G unzip - -DZIP=FILENAME
```

## 文法

### 代入

```shell
NAME = string
NAME = ${NAME1}
NAME = ${NAME1|NAME2}
NAME = ${NAME1|NAME2|NAME3:DefaultValue}
```

１行目: NAME には string が格納されます

２行目：NAMEには NAME1 の内容が代入されます。

３行目：NAMEには NAME1 の値が代入されます。ただし、NAME1が空っぽの場合はNAME2の内容が代入されます。

４行目：NAME1,NAME2,NAME3の値の順に調べ、空でないものを代入します。すべての値が空の場合、DefaultValueを代入します。

## 制御文

### if ([NOT] 値)

```shell
.if(${V})
.else
.end

.if(NOT ${V})
.else
.end
```

### for(イテレータ 値)

for文中では次の特殊なマクロを定義します。

| Name        | Value                       |
| ----------- | --------------------------- |
| __for_count | 0から始まる繰り返しカウント |

```shell
VALUES = A B C
.for(I ${VALUES})
	@message(${__for_count} ${I})
.end
```



## 組み込み済みマクロ

| Name              | Value                                         |
| ----------------- | --------------------------------------------- |
| APGEN_STARTUP_DIR | コマンド開始時の apgen のディレクトリ。       |
| APGEN_SELF_DIR    | 自身のディレクトリ。include時でも変更される。 |

## 関数

関数は @関数名(パラメータ) のように書くことで呼び出すことができます。

### @dump()

現在定義済みのマクロとその中身の一覧をダンプします。

```shell
@dump()
```



### @include(ファイル名)

指定したファイルをincludeします。

```shell
@include(../lib/xyz.apgen)
```

### @message(メッセージ)

メッセージを表示します。

```shell
@message(MESSAGE ${V})
```

### @find(名前 パス[ パス[ パス]...])

第１引数で指定するファイル名またはディレクトリが第２引数以降のパスから検索し、一致したパスを返します。検索位置は相対パスを指定する場合は APGEN_STARTUP_DIR からの相対パスになります。

```shell
LIBAMTRS_PATH = @find(libamtrs .. ../.. ../../.. ../libs ../../libs)
```

### @normalize_path(パス [TRUE|FALSE])

パスを正規化します。第２引数をTRUEにした場合、Windows環境ではバックスラッシュでエスケープします。

```shell
PATH =  @normalize_path(path/to/../path2 TRUE)
path\\path2
```

### @update(DEST SOURCE)

SOURCEで指定するファイルを読み取りマクロを置換、DESTに書き込みます。内容に変更がなければスキップします。モジュールに関係なく実行されます。

