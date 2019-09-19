package jp.libamtrs;
import	android.content.Intent;


public class AmtrsActivityResult
{
	private long	mHandler;

	AmtrsActivityResult()
	{}

	AmtrsActivityResult(long _handler)
	{
		mHandler	= _handler;
	}

	static AmtrsActivityResult makeNativeBridge(long _handler)
	{
		return	new AmtrsActivityResult(_handler);
	}


	// ネイティブコードをそのまま呼び出す
	// Javaで作られた処理を行う場合はオーバーライドして処理してください。
	void onActivityResult(int _requestCode, int _resultCode, Intent _data)
	{
		onActivityResult(mHandler, _requestCode, _resultCode, _data);
	}

	native private void onActivityResult(long _handler, int _requestCode, int _resultCode, Intent _data);
}
