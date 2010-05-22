// NyWin32CaptureTestDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include <assert.h>
#include "NyWin32Capture.h"
#include "NyWin32CaptureTest.h"
#include "NyWin32CaptureTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace NyWin32Capture;

/*	アプリケーションコントローラ
*/
class AppCtrl
{
public:
	CWnd* _wnd;
	CaptureDeviceList* devlist;
	CaptureDevice* dev;
	BITMAPINFOHEADER dibheader;
public:
	AppCtrl(CWnd* i_app_window)
	{
		CoInitialize(NULL);

		this->_wnd=i_app_window;
		CButton* bn=(CButton*)_wnd->GetDlgItem(ID_SWITCH);
		bn->SetWindowText(_T("start capture"));
		//カメラキャプチャデバイスのリストを作る。
		devlist=new CaptureDeviceList();
		//キャプチャデバイスのリストから1番目のカメラを得る。
		int nod=this->devlist->getNumberOfDevice();
		if(nod<1){
			throw std::exception("This computer has not Capture device.");
		}
		//0番目のカメラをあける。
		CaptureDevice* d=devlist->getDevice(0);
		d->openDevice();
		//キャプチャフォーマットを設定
		SetupCaptureFormat(d,this->dibheader);
		//コールバックモードの時に使うユーザ定義値を設定
		d->setUserValue(this);
		this->dev=d;

	}
	virtual ~AppCtrl()
	{
		delete devlist;
		//カメラキャプチャデバイスのリストを開放
		CoUninitialize();


	}
	//１番目のカメラをQVGAでキャプチャできるように設定する。
	bool SetupCaptureFormat(CaptureDevice* d,BITMAPINFOHEADER& bmiheader)
	{
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
			//カメラがフォーマットを持ってない場合はインテリジェント接続を試す。
			d->setVideoFormat(320,240,MEDIASUBTYPE_RGB24,30.0);
			VideoFormat::initBITMAPINFOHEADER(320,240,MEDIASUBTYPE_RGB24,bmiheader);
		}else{
			//カメラがフォーマットを持ってる場合はそのまま使う
			d->setVideoFormat(*vf,30.0);
			bmiheader=*(vf->getBitmapInfoHeader());
		}
		return true;
	}
	//同期してイメージを取得して、ウインドウに描画
	void DrawBitmap()
	{
		//イメージを同期取得
		const AM_MEDIA_TYPE& mt=dev->getMediaType();
		BYTE* buf;
		buf=new BYTE[mt.lSampleSize];
		this->dev->captureImage(buf);
		//描画
		DrawBitmap(buf);
		delete[] buf;
	}
	//dataの内容をウインドウに描画
	void DrawBitmap(void* data)
	{
		//DIB→BITMAPに変換して描画
		CDC* dc=this->_wnd->GetDC();
		CBitmap bmp;
		BITMAPINFO bmi;
		bmi.bmiHeader=this->dibheader;
		int l=SetDIBitsToDevice(
			dc->m_hDC,0,0,this->dibheader.biWidth,this->dibheader.biHeight,
			0,0,0,this->dibheader.biHeight,data,&bmi,DIB_RGB_COLORS);
		this->_wnd->ReleaseDC(dc);
	}
	void Start(bool is_async)
	{
		if(!is_async){
			this->dev->startCapture();	//同期取得の実験ならこっち
		}else{
			this->dev->startCaptureCallback(AppCtrl::OnCaptureImage);		//非同期取得の実験ならこっち
		}
	}
	void Stop()
	{
		this->dev->stopCapture();
	}
	static void OnCaptureImage(const CaptureDevice* i_sender,BYTE *pBuffer, long BufferLen)
	{
		//SendMessageで飛ばす
		AppCtrl* inst=(AppCtrl*)(i_sender->getUserValue());
		DWORD ret;
		//GDIと同期をとるときにデットロックする可能性があるから。
		if(SendMessageTimeout(inst->_wnd->m_hWnd,WM_USER+39,0,(LPARAM)pBuffer,SMTO_ABORTIFHUNG,1000,&ret)==0){
			OutputDebugString(_T("Error on SendMessageTimeout"));
		}
	}
};


//クラスを公開したくないのでグローバル変数にしてあるけど、実際は
//親クラスのメンバにした方がよいです。
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
	ON_WM_TIMER()
    ON_MESSAGE((WM_USER+39),CNyWin32CaptureTestDlg::OnASyncCapture)//キャプチャ
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
	this->is_start=false;

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



//ボタンが押されたとき
void CNyWin32CaptureTestDlg::OnBnClickedSwitch()
{
	//キャプチャON→OFFの切り替え
	CButton* bn=(CButton*)this->GetDlgItem(ID_SWITCH);
	this->is_start=(!this->is_start);
	bn->SetWindowText(this->is_start?_T("stop capture"):_T("start capture"));
/*	//同期実行テスト
	if(this->is_start){
		appctrl->Start(false);
		this->SetTimer(123,100,NULL);
	}else{
		this->KillTimer(123);
		appctrl->Stop();
	}*/
	//非同期試験ならこっち
	if(this->is_start){
		appctrl->Start(true);
	}else{
		appctrl->Stop();
	}
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

void CNyWin32CaptureTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	appctrl->DrawBitmap();
	CDialog::OnTimer(nIDEvent);
}
//SendMessageで飛ばしたメッセージの受け口
LRESULT CNyWin32CaptureTestDlg::OnASyncCapture(WPARAM wParam, LPARAM lParam )
{
	//あーこれ64bitポインタだとまずくない？
	appctrl->DrawBitmap((void*)lParam);
	return 0;
}
