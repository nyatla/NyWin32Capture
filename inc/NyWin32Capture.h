/* 
 * PROJECT: NyWin32Capture
 * --------------------------------------------------------------------------------
 * The MIT License
 * Copyright (c) 2010 R.Iizuka NyARToolkit project
 * airmail(at)ebony.plala.or.jp
 * http://nyatla.jp/nyartoolkit/
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */
#pragma once
#include <exception>
#include <vector>
#include <windows.h>
#include <strmif.h>
#include <amvideo.h>
#include <dshow.h>
#include <uuids.h>


namespace NyWin32Capture
{
	/**	このクラスは、NyWin32CaptureExceptionの例外を定義します。
	*/
	class NyWin32CaptureException :public std::exception
	{
	public:
		NyWin32CaptureException();
		NyWin32CaptureException(exception e);
		NyWin32CaptureException(const char* m);
	};
}

namespace NyWin32Capture
{
	/**	このクラスは、1個のビデオフォーマットを格納し、各値へのアクセス手を提供します。
	*/
	class VideoFormat
	{
	private:
		//コピーコンストラクタ削除
		VideoFormat(const VideoFormat& );
		VideoFormat& operator=(const VideoFormat&);
	private:
		AM_MEDIA_TYPE *_pmt;
		VIDEO_STREAM_CONFIG_CAPS* _scc;
	public:
		VideoFormat(AM_MEDIA_TYPE* i_pmt,const VIDEO_STREAM_CONFIG_CAPS& i_scc);
		virtual ~VideoFormat();
		int getWidth()const;
		int getHeight()const;
		double getRate()const;
		const GUID& getMediaSubType()const;
		const AM_MEDIA_TYPE* VideoFormat::getMediaType()const;
		const VIDEOINFOHEADER* getVideoInfoHeader()const;
		/**	フォーマットのBITMAPINFOHEADERを返します。
		*/
		const BITMAPINFOHEADER* getBitmapInfoHeader()const;
	public:
		/**	指定したパラメータで、BITMAPINFOHEADER構造体を初期化します。
		*/
		static void initBITMAPINFOHEADER(int i_width,int i_height,const GUID& i_media_subtype,BITMAPINFOHEADER& o_bmih);
	};
}

namespace NyWin32Capture
{
	/**	このクラスは、Videofirmatのリストを定義します。
	*/
	class VideoFormatList
	{
	private:
		//コピーコンストラクタ削除
		VideoFormatList(const VideoFormatList& );
		VideoFormatList& operator=(const VideoFormatList&);
	private:
		std::vector<VideoFormat*> *_list;
		void clear();
	public:
		VideoFormatList();
		void update(IAMStreamConfig* i_config);
		virtual ~VideoFormatList();
		/**	この関数は、指定したフォーマット(幅、高さ、メディアサブタイプ)に合致するVideoFormatをリストから検索します。
			見つかったれば、そのフォーマットへのポインタを返します。
			見つからなければ、NULLを返します。
		*/
		const VideoFormat* getFormat(int i_width,int i_height,const GUID& i_media_type)const;
		/**	この関数は、指定したフォーマット(幅、高さ)に合致するVideoFormatをリストから検索します。
			見つかったれば、そのフォーマットへのポインタを返します。
			見つからなければ、NULLを返します。
		*/
		const VideoFormat* getFormat(int i_width,int i_height)const;
		/**	この関数は、i_index番目のVideoFormatを返します。
		*/
		const VideoFormat* getFormat(int i_index)const;
		/**	リストが所有するVideoFormatの数を返します。
		*/
		int getNumberOfFormat()const;
	};
}


struct ISampleGrabber;


namespace NyWin32Capture
{

	//class IMediaControl;

	class CaptureImageCallback;
	class CaptureDevice;
	/**	startCaptureCallbackが呼び出すコールバック関数です。
		この関数の中からStart,Stop等の関数を呼び出さないでください。
	*/
	typedef void (*OnCaptureImage)(const CaptureDevice* i_sender,BYTE *pBuffer, long BufferLen);



	/**	このクラスは、キャプチャデバイス１つを制御します。
		このクラスは３つの状態ST_RUN,ST_IDLE,ST_CLOSEDを持ちます。
		ステータスと、これを変化させる関数名の関係は以下の通りです。

		
		|function name      | status transition
		|-------------------+---------------------
		|openDevice         | ST_CLOSED -> ST_IDLE
		|(setting functions)| -
		|startDevice        | ST_IDLE   -> ST_RUN
		|(capture funstion )| -
		|stopDevice         | ST_RUN    -> ST_IDLE
		|closeDevice        | ST_IDLE   -> ST_CLOSED

	*/
	class CaptureDevice
	{
	private:
		const static int ST_CLOSED=0;
		const static int ST_IDLE  =1;
		const static int ST_RUN=2;
	private:
		IMoniker*    _moniker;
		AM_MEDIA_TYPE _capture_mediatype;
		struct{
			struct{
				IBaseFilter* filter;
				IAMStreamConfig* config;
				IPin* pin;
			}sorce;
			struct{
				IMediaControl* mc;
				IGraphBuilder* graph;
			}graph_builder;
			struct{
				IBaseFilter* filter;
				ISampleGrabber* grab;
			}render;
		}ds_res;
		struct{
			WCHAR* name;
		}_allocated_res;
		int _status;
		CaptureImageCallback* _image_cb;
		void* _user_value;
		void mStartCapture(BOOL i_set_bffer_samples);
	public:
		CaptureDevice(IMoniker* i_moniker);
		virtual ~CaptureDevice();
		/**	同期モードでキャプチャ処理をスタートします。
			この関数で開始したキャプチャは、captureImage関数でイメージを取得することができます。
		*/
		void startCapture();
		/**	非同期モードでキャプチャ処理をスタートします。
			この関数で開始したキャプチャは、イメージをi_callback関数に非同期で通知します。
			captureImage関数を使うことはできません。
		*/
		void startCaptureCallback(OnCaptureImage i_callback);
		/**	キャプチャを停止しします。
		*/
		void stopCapture();
		/**	キャプチャデバイスをオープンします。
		*/
		void openDevice();
		/**	キャプチャデバイスをクローズします。
		*/
		void closeDevice();
		/** デバイスの提供できるビデオフォーマットの一覧を、o_list引数に返します。
			この関数は、ST_IDLE,ST_RUNステータスのときだけ使用可能です。
		*/
		void getVideoFormatList(VideoFormatList& o_list)const;
		/** キャプチャイメージを取得します。
			この関数は、startCaptureからしばらくの間失敗することがあります。
			この関数は、startCaptureで開始し、かつST_RUNステータスのときだけ使用可能です。
		*/
		bool captureImage(void* i_buf,long i_buf_size=0);
		/** キャプチャイメージのフォーマットを指定します。
			この関数は、ST_IDLEステータスのときだけ使用可能です。
		*/
		bool setVideoFormat(int i_width,int i_height,const GUID& i_media_subtype,double i_rate);
		/** キャプチャイメージのフォーマットを指定します。
			この関数は、ST_IDLEステータスのときだけ使用可能です。
		*/
		bool setVideoFormat(const VideoFormat& i_format,double i_rate);
		/** i_formatのmedia_subtypeフィールドを強制してキャプチャイメージのフォーマットを指定します。
			この関数は、ST_IDLEステータスのときだけ使用可能です。
		*/
		bool setVideoFormat(const VideoFormat& i_format,const GUID& i_media_subtype,double i_rate);

		/**	キャプチャ画像メディアタイプを取得します。
		*/
		const AM_MEDIA_TYPE& getMediaType()const;
		/**	ユーザ定義値を取得します。
		*/
		void* getUserValue()const;
		/**	ユーザ定義値を設定します。この値は、getUserValueで取得できます。
			非同期関数に任意の値を渡したいときに使用します。
		*/
		void setUserValue(void* i_user_value);
		/**	このデバイスの値を取得します。
		*/
		const WCHAR* getName()const;
	};
}

namespace NyWin32Capture
{
	/**	このクラスは、キャプチャデバイスのリストを定義し、キャプチャデバイスを所有します。
	*/
	class CaptureDeviceList
	{
	private:
		void createDeviceList();
		void releaseDeviceList();
		std::vector<CaptureDevice*> *_list;
	public:
		CaptureDeviceList();
		virtual ~CaptureDeviceList();
		//	i_index番目のキャプチャデバイスを取得します。
		CaptureDevice* getDevice(int i_index)const;
		//	デバイスの数を取得します。
		int getNumberOfDevice()const;
	};
}