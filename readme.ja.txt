Windows用キャプチャCOM DLL.
Copyright (C)2008 R.Iizuka

version 1.0.0

Website:	http://nyatla.jp/
E-mail :	nyatla39<at>gmail.com
--------------------------------------------------


・NyWin32Capture

	NyWin32Captureは、Windows用のカメラキャプチャ支援ライブラリです。
	WindowsOSで向けの、カメラキャプチャアプリケーションに使用できます。


・特徴
	NyWin32Captureには、以下の特徴があります。
	
	+同期/非同期取得（コールバック関数）対応
		同期・非同期両方のAPIで、キャプチャ画像の取得ができます。
	+複数のキャプチャデバイスの同時制御
		複数のデバイスから、同時にキャプチャ処理を実行できます。
	+WindowsSDK 7.1対応
		WindowsSDK 7.1に対応しています。

・開発環境
 Visual Stadio 2008 Professional 以上


・ディレクトリ構成
	./build
	  プロジェクトファイルがあります。
	./inc
	  アプリケーションから参照するヘッダファイルです。
	./sample
	  キャプチャのサンプルプログラムがあります。
	./src
	  ソースファイルがあります。


・サンプル
	NyWin32CaptureTest
	  GUIにMFCを使った、１台のカメラを制御するテストプログラムです。
	MultiCapture
	  GUIにMFCを使った、複数台のカメラを制御するテストプログラムです。
	  PCに接続された全てのカメラから映像を取得し、表示します。


・コンパイル方法

	1.
	NyWin32Captureには、Windows SDK 7.1が必要です。下記URLから
	ダウンロードして下さい。
	http://www.microsoft.com/downloads/details.aspx?displaylang=en&FamilyID=35aeda01-421d-4ba5-b44b-543dc8c33a20

	2.
	NyWin32Capture.slnを開いて、コンパイルしてください。コンパイルには、VisualStadio2008 Express Edition
	以上が必要です。
	

・ライセンス
	NyWin32CaptureはMITライセンスで配布しています。


・おまけ

	このライブラリは、qedit.dllを使用していますが、qedit.hを参照していません。
	これは、Windows SDK 7.0以降にqedit.hが含まれていないためです。
	
	この問題を回避するため、NyWin32Captureでは、qedit_def.hにSampleGrabberの
	定義を実装しています。
	
	方法は、1.qedit.DLLからithを作る。2.SampleGrabberを直接定義。3.過去のqedit
	をインクルードの3種類ありますが、デフォルトでは2を選択しています。

	もし、うまくコンパイルできない場合には、1,3の方法を試してみてください。



	