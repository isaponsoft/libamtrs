## Configuration macro

|MACRO|Type|comment|
|--|--|--|
|AMTRS_DEBUG_LOG_LEVEL|User's setting|デバッグログのレベル 0:NONE 1:ERROR 2:WARNING ... 7:ALL|


## Macro

|MACRO|Type|comment|
|--|--|--|
|AMTRS_CURRENT_PLATFORM|Auto|AMTRS_PLATFOEM_xxxx|
|AMTRS_PLATFORM_ANDROID|Auto|Android is 1|
|AMTRS_PLATFORM_FREEBSD|Auto|FreeBSD is 1|
|AMTRS_PLATFORM_IOS|Auto|iOS is 1|
|AMTRS_PLATFORM_LINUX|Auto|Linux is 1|
|AMTRS_PLATFORM_WIN32|Auto|Win32 is 1|


## Libraries

|file|comment|
|--|--|
|amtrs.hpp|基本ヘッダ|
|application.hpp|アプリケーションのイベントを管理する|
|camera.hpp|カメラを扱う|
|chrono.hpp|std::chrono utility|
|crypto.hpp|暗号化などで使用される機能群|
|compati-stl/*|STL互換簡易クラス。プラットフォームによって使用できない機能を補助するためのものです|
|filesystenm.hpp|OSのファイルシステムにアクセスする|
|font.hpp|フォントを扱う|
|g3d.hpp|3D Graphics API|
|geometry.hpp|幾何学。空間や座標を扱う|
|graphics.hpp|Bitmapや画像形式|
|iostream.hpp|Input Output(DEPRECATED)|
|java/*|Java class virtual interface with JNI|
|jni.hpp|Java Native Interface Utility|
|main.hpp|main() 関数のラッパー|
|media_manager.hpp|画像ファイルや音声ファイルなどOSからメディア情報を取得する|
|node.hpp|Node System|
|node/*.cpp|Node System extra nodes|
|shared_bufeer.hpp|共有バッファ|
|string.hpp|文字列に関する便利関数|
|text-json.hpp|JSONパーサー|
|text-xml.hpp|XML便利関数|
|vstream.hpp|iostreamの仮想化|
|window.hpp|GUIプラットフォームにおけるアプリケーション領域|

