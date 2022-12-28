# Android Gradle

ビルドタイプ Debug/Releaseが使用できます。

## Global parameter

| Value                       | Default                                                      |
| --------------------------- | ------------------------------------------------------------ |
| GRADLE_URL                  | https://services.gradle.org/distributions/gradle-6.1-bin.zip |
| Android.AndroidManifest.xml | AndroidManifestを生成するためのテンプレートファイルを指定できます。 |

## AndroidManifest.xml

Android.template.AndroidManifest.xml にファイル名を指定するとカスタマイズした AndroidManifest.xml のテンプレートを使用できます。

| Name                                | Value                                                 | Default                                                      |
| ----------------------------------- | ----------------------------------------------------- | ------------------------------------------------------------ |
| パッケージ名                        | Android.Manifest.Package PACKAGE APPLICATIONID        |                                                              |
| アクティビティ                      | Android.Manifest.Activity MAINACTIVITY                | .MainActivity                                                |
| Activity android:screenOrientation  | Android.Manifest.ScreenOrientation SCREEN_ORIENTATION | portrait                                                     |
| android:name="android.app.lib_name" | Android.Manifest.AppLibName                           | appmain                                                      |
| uses-permission                     | Android.Manifest.Permission                           | ACCESS_NETWORK_STATE ACCESS_WIFI_STATE WAKE_LOCK DISABLE_KEYGUARD READ_EXTERNAL_STORAGE WRITE_EXTERNAL_STORAGE com.android.vending.BILLING |
|                                     |                                                       |                                                              |

### build.gradle

Template: Android.template.build.gradle

| Name                                                         | Value                                          | Default                    |
| ------------------------------------------------------------ | :--------------------------------------------- | -------------------------- |
| android.defaultConfig.applicationId                          | Android.Manifest.Package PACKAGE APPLICATIONID |                            |
| android.defaultConfig.externalNativeBuild.cmake.arguments    | Android.BuildGradle.CmakeArguments             |                            |
| android.sourceSets.main.assets.srcDirs                       | Android.BuildGradle.Assets.SrcDirs             | ${APGEN_STARTUP_DIR}/asset |
| android.sourceSets.main.res.srcDirs                          | Android.BuildGradle.Res.SrcDirs                | [ "res/android" ]          |
| android.sourceSets.debug.res.srcDirs                         | Android.BuildGradle.Debug.Res.SrcDirs          |                            |
| android.sourceSets.main.java.srcDirs                         | Android.BuildGradle.Java.SrcDirs               | ["src-android"]            |
| android.externalNativeBuild.cmake.path                       | Android.BuildGradle.CMakeLists                 |                            |
| android.signingConfigs.release[storeFile\|storePassword\|keyAlias\|keyPassword] | Android.BuildGradle.signingConfigs             |                            |
|                                                              |                                                |                            |



