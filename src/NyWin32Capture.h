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
		const GUID& getFormat()const;
		const AM_MEDIA_TYPE* VideoFormat::getMediaType()const;
		const VIDEOINFOHEADER* getVideoInfoHeader()const;
		const BITMAPINFOHEADER* getBitmapInfoHeader()const;
	};
}

namespace NyWin32Capture
{
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
		/**	この関数は、指定したフォーマットに合致するVideoFormatをリストから検索します。
			見つかったれば、そのフォーマットへのポインタを返します。
			見つからなければ、NULLを返します。
		*/
		const VideoFormat* getFormat(int i_width,int i_height,const GUID& i_format)const;
		const VideoFormat* getFormat(int i_index)const;
		int getNumberOfFormat()const;
	};
}


struct ISampleGrabber;


namespace NyWin32Capture
{

	class CaptureImageCallback;
	class CaptureDevice;
	/**	コールバック関数
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
//				GUID pin_category;
				IPin* pin;
			}sorce;
			struct{
				IMediaControl* mc;
				IFilterGraph2* graph;
			}graph_builder;
			ICaptureGraphBuilder2* cap_builder;
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
	public:
		CaptureDevice(IMoniker* i_moniker);
		virtual ~CaptureDevice();
		void startCapture();
		void startCaptureCallback(OnCaptureImage i_callback);
		void stopCapture();
		void openDevice();
		void closeDevice();
		/** デバイスの提供できるビデオフォーマットの一覧を、o_list引数に返します。
			この関数は、ST_IDLE,ST_RUNステータスのときだけ使用可能です。
		*/
		void getVideoFormatList(VideoFormatList& o_list)const;
		/** キャプチャイメージを取得します。
			この関数は、startCaptureからしばらくの間失敗することがあります。
			この関数は、ST_RUNステータスのときだけ使用可能です。
		*/
		bool captureImage(void* i_buf,long i_buf_size=0);
		/** キャプチャイメージのフォーマットを指定します。
			この関数は、ST_IDLEステータスのときだけ使用可能です。
		*/
		bool setVideoFormat(int i_width,int i_height,GUID i_format,double i_rate);
		/** キャプチャイメージのフォーマットを指定します。
			この関数は、ST_IDLEステータスのときだけ使用可能です。
		*/
		bool setVideoFormat(const VideoFormat& i_format,double i_rate);
		/**	キャプチャ画像メディアタイプを取得します。
		*/
		const AM_MEDIA_TYPE& getMediaType()const;

		void* getUserValue()const;
		void setUserValue(void* i_user_value);
		const WCHAR* getName()const;
	};
}

namespace NyWin32Capture
{
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