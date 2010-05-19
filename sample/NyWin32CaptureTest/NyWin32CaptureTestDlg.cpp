// NyWin32CaptureTestDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "NyWin32Capture.h"
#include "NyWin32CaptureTest.h"
#include "NyWin32CaptureTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace NyWin32Capture;


class AppCtrl
{
public:
	CWnd* _wnd;
	bool is_start;
	CaptureDeviceList* devlist;
public:
	AppCtrl(CWnd* i_app_window)
	{
		CoInitialize(NULL);					// COMの初期化		
		this->_wnd=i_app_window;
		this->is_start=false;
		CButton* bn=(CButton*)_wnd->GetDlgItem(ID_SWITCH);
		bn->SetWindowText(_T("start capture"));
		//カメラキャプチャデバイスのリストを作る。
		devlist=new CaptureDeviceList();
		SetupCamera1();

	}
	virtual ~AppCtrl()
	{
		//カメラキャプチャデバイスのリストを開放
		delete devlist;

		CoUninitialize();
	}
	//	キャプチャデバイスをオープンして、１番目のカメラをQVGAでキャプチャできるように
	//	設定する。
	//
	bool SetupCamera1()
	{
		//キャプチャデバイスのリストを読む
		int nod=this->devlist->getNumberOfDevice();
		if(nod<1){
			return "This computer has not Capture device.";
		}
		//0番目のカメラをあける。
		CaptureDevice* d=devlist->getDevice(0);
		d->openDevice();
		//フォーマットリストを得る。
		VideoFormatList lt;
		d->getVideoFormatList(lt);
		int nof=lt.getNumberOfFormat();
		if(nof<1){
			return "The device has not Video format.";
		}
		//イメージのフォーマットを設定。このタイプはuuids.hに書いてある。
		const VideoFormat* vf=lt.getFormat(320,240,MEDIASUBTYPE_RGB24);
		if(vf==NULL){
			return "The device has not 320,240,MEDIASUBTYPE_RGB24,30.0 format.";
		}
		//イメージフォーマットを設定
		d->setVideoFormat(*vf,30.0);
		return NULL;
	}

	void OnPushSwitch()
	{/*
		CButton* bn=(CButton*)this->_wnd->GetDlgItem(ID_SWITCH);
		bn->SetWindowText(this->is_start?_T("stop capture"):_T("start capture"));
		this->is_start=(!this->is_start);
	*/	
	}
};

AppCtrl* appctrl;




// CNyWin32CaptureTestDlg ダイアログ




CNyWin32CaptureTestDlg::CNyWin32CaptureTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNyWin32CaptureTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNyWin32CaptureTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNyWin32CaptureTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_SWITCH, &CNyWin32CaptureTestDlg::OnBnClickedSwitch)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CNyWin32CaptureTestDlg メッセージ ハンドラ

BOOL CNyWin32CaptureTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	appctrl=new AppCtrl(this);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CNyWin32CaptureTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CNyWin32CaptureTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CNyWin32CaptureTestDlg::OnBnClickedSwitch()
{
	//開始・停止スイッチを押した
//	appctrl->OnPushSwitch();
}

void CNyWin32CaptureTestDlg::OnClose()
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	CDialog::OnClose();
}

void CNyWin32CaptureTestDlg::OnDestroy()
{
	delete appctrl;
	CDialog::OnDestroy();
	// TODO: ここにメッセージ ハンドラ コードを追加します。
}
