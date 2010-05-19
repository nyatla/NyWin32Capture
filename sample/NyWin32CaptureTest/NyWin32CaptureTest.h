// NyWin32CaptureTest.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CNyWin32CaptureTestApp:
// このクラスの実装については、NyWin32CaptureTest.cpp を参照してください。
//

class CNyWin32CaptureTestApp : public CWinApp
{
public:
	CNyWin32CaptureTestApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CNyWin32CaptureTestApp theApp;