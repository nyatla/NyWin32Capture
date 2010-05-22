// MultiCaptureTest.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CMultiCaptureTestApp:
// このクラスの実装については、MultiCaptureTest.cpp を参照してください。
//

class CMultiCaptureTestApp : public CWinApp
{
public:
	CMultiCaptureTestApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMultiCaptureTestApp theApp;