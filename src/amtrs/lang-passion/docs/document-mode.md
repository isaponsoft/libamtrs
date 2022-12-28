# document mode

パーサーの document_mode フラグを true にするとドキュメントモードを有効にします。
ドキュメントモードでは ```<% %>``` 以外のコードは標準出力へ出力されます。

例えば html にコードを埋め込むような使い方などが出来ます。

```html
<html>
<body>
<%
	for (int i = 0; i < 10; ++i)
	{
		echo("${i}, ");
	}
%>
</body>
</html>
```

これは以下のように出力されます。

```html
<html>
<body>
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
</body>
</html>
```
