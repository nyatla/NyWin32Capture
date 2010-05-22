// MultiCaptureTestDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include <assert.h>
#include "NyWin32Capture.h"
#include "MultiCaptureTest.h"
#include "MultiCaptureTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace NyWin32Capture;

class CaptureItem
{
public:
	int tag;
	CaptureDevice* dev;
	BITMAPINFOHEADER dibheader;
	HWND hwnd;

	CaptureItem(int i_tag,CaptureDevice* i_dev,const BITMAPINFOHEADER& bmi,HWND i_hwnd)
	{
		this->tag=i_tag;
		this->dev=i_dev;
		this->dibheader=bmi;
		this->hwnd=i_hwnd;
	}
};
/*	アプリケーションコントローラ
*/
class AppCtrl
{
public:
	CWnd* _wnd;
	CaptureDeviceList* devlist;
	std::vector<CaptureItem*> _item;
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
		//全部のカメラをあける
		for(int i=0;i<nod;i++){
			CaptureDevice* d=devlist->getDevice(i);
			d->openDevice();
			//キャプチャフォーマットを設定
			BITMAPINFOHEADER b;
			SetupCaptureFormat(d,b);
			//コールバックモードの時に使うユーザ定義値を設定
			CaptureItem* item=new CaptureItem(i,d,b,i_app_window->m_hWnd);
			this->_item.push_back(item);
			d->setUserValue(item);
		}
	}
	virtual ~AppCtrl()
	{
		for(int i=0;i<this->_item.size();i++){
			delete _item[i];
		}
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
		//DIB作るためにヘッダを保存しておく
		return true;
	}
	//dataの内容をウインドウに描画
	void DrawBitmap(void* data,int i)
	{
		//DIB→BITMAPに変換して描画
		CDC* dc=this->_wnd->GetDC();
		CBitmap bmp;
		BITMAPINFO bmi;
		bmi.bmiHeader=this->_item[i]->dibheader;
		int x,y;
		x=(i%3)*320;
		y=(i/3)*240+32;
		int l=SetDIBitsToDevice(
			dc->m_hDC,x,y,320,240,
			0,0,0,240,data,&bmi,DIB_RGB_COLORS);
		this->_wnd->ReleaseDC(dc);
	}
	void Start(bool is_async)
	{
		for(int i=0;i<this->_item.size();i++){
			if(!is_async){
				this->_item[i]->dev->startCapture();	//同期取得の実験ならこっち
			}else{
				this->_item[i]->dev->startCaptureCallback(AppCtrl::OnCaptureImage);		//非同期取得の実験ならこっち
			}
		}
	}
	void Stop()
	{
		for(int i=0;i<this->_item.size();i++){
			this->_item[i]->dev->stopCapture();
		}
	}
	static void OnCaptureImage(const CaptureDevice* i_sender,BYTE *pBuffer, long BufferLen)
	{
		//SendMessageで飛ばす
		CaptureItem* inst=(CaptureItem*)(i_sender->getUserValue());
		DWORD ret;
		//GDIと同期をとるときにデットロックする可能性があるから。
		if(SendMessageTimeout(inst->hwnd,WM_USER+39,inst->tag,(LPARAM)pBuffer,SMTO_ABORTIFHUNG,1000,&ret)==0){
			OutputDebugString(_T("Error on SendMessageTimeout"));
		}
	}
};


//クラスを公開したくないのでグローバル変数にしてあるけど、実際は
//親クラスのメンバにした方がよいです。
AppCtrl* appctrl;




// CMultiCaptureTestDlg ダイアログ




CMultiCaptureTestDlg::CMultiCaptureTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMultiCaptureTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMultiCaptureTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMultiCaptureTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_SWITCH, &CMultiCaptureTestDlg::OnBnClickedSwitch)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
    ON_MESSAGE((WM_USER+39),CMultiCaptureTestDlg::OnASyncCapture)//キャプチャ
END_MESSAGE_MAP()


// CMultiCaptureTestDlg メッセージ ハンドラ

BOOL CMultiCaptureTestDlg::OnInitDialog()
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

void CMultiCaptureTestDlg::OnPaint()
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
HCURSOR CMultiCaptureTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//ボタンが押されたとき
void CMultiCaptureTestDlg::OnBnClickedSwitch()
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

void CMultiCaptureTestDlg::OnClose()
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	CDialog::OnClose();
}

void CMultiCaptureTestDlg::OnDestroy()
{
	delete appctrl;
	CDialog::OnDestroy();
	// TODO: ここにメッセージ ハンドラ コードを追加します。
}

void CMultiCaptureTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}
//SendMessageで飛ばしたメッセージの受け口
LRESULT CMultiCaptureTestDlg::OnASyncCapture(WPARAM wParam, LPARAM lParam )
{
	//あーこれ64bitポインタだとまずくない？
	appctrl->DrawBitmap((void*)lParam,wParam);
	return 0;
}
