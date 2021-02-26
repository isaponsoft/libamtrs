/*
 * passion には c/c++ の「プリプロセッサ」によく似た機能を持っています。
 * c/c++ のプロセッサは行単位で処理しますが、passionのプリプロセッサは単語単位で
 * 処理を行います。そのため、c/c++よりも柔軟かつ複雑なマクロ機能を提供します。
 *
 * Passionプリプロセッサ(PPP = Passion PreProcessor)の命令は
 * ディレクトィブ呼び # から始まります。
 */


#define	M_1		"Hello"

int main()
{
	printf(M_1);		// M_1 は "Hello" に置き換えられます。
	printf("\n");


	printf("${M_1}\n");	// 文字列の展開にも対応しています。
	return	0;
}
