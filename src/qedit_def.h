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

/*		このファイルは、qedit.hが無くてもコンパイルできるようにするためのヘッダーです。
		3つのオプションのうち、コンパイルできるものを１つを選んでください。

		NyWin32Capture_QEDIT_DEF_INCLUDE
			普通にqedit.hをインクルードします。

		NyWin32Capture_QEDIT_DEF_TYPELIB
			タイプライブラリからqedit.ithを生成します。
			VC2008の場合、comutils.hにバグがあり、INTSAFE_E_ARITHMETIC_OVERFLOWマクロの多重定義が起きます。
			そのため、C4005を無視する指定があります。

		NyWin32Capture_QEDIT_DEF_COPY
			宣言をそのまま使います。
*/

//
//#define NyWin32Capture_QEDIT_DEF_INCLUDE 1
//#define NyWin32Capture_QEDIT_DEF_TYPELIB 2
#define NyWin32Capture_QEDIT_DEF_COPY    3



#if defined(NyWin32Capture_QEDIT_DEF_INCLUDE)
	#include <qedit.h>
#elif defined(NyWin32Capture_QEDIT_DEF_TYPELIB)
	//qedit.dllのタイプライブラリを使う
	#pragma warning(disable : 4005)
	#define IMediaSamplePtr HRESULT	//宣言ミス？の回避用
	#import "libid:78530B68-61F9-11D2-8CAD-00A024580902"\
	 no_namespace\
	 named_guids\
	 raw_interfaces_only\
	 no_implementation\
	 exclude("_AMMediaType","_FilterState","IReferenceClock","IMediaFilter","_PinDirection",\
	"IEnumMediaTypes","IFilterGraph","_FilterInfo","IGraphBuilder","IBaseFilter",\
	"_PinInfo","IPin","IEnumPins","IEnumFilters","IEnumMediaTypes","IAMSetErrorLog","IAMTimelineObj",\
	"IMediaDet","IMediaSample",\
	"IPersistStream","IPersist","IStream","ISequentialStream","_LARGE_INTEGER","_ULARGE_INTEGER","tagSTATSTG","_FILETIME","IPropertyBag","IErrorLog")
#elif defined(NyWin32Capture_QEDIT_DEF_COPY)

	const CLSID CLSID_SampleGrabber = { 0xC1F400A0, 0x3F08, 0x11d3, { 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37 } };
	static const IID IID_ISampleGrabberCB = { 0x0579154A, 0x2B53, 0x4994, { 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85 } };
	static const IID IID_ISampleGrabber = { 0x6B652FFF, 0x11FE, 0x4fce, { 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F } };

	interface ISampleGrabberCB :public IUnknown
	{
		virtual STDMETHODIMP SampleCB( double SampleTime, IMediaSample *pSample ) = 0;
		virtual STDMETHODIMP BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen ) = 0;
	};


	interface ISampleGrabber:public IUnknown
	{
		virtual HRESULT STDMETHODCALLTYPE SetOneShot( BOOL OneShot ) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetMediaType( const AM_MEDIA_TYPE *pType ) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( AM_MEDIA_TYPE *pType ) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( BOOL BufferThem ) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( long *pBufferSize, long *pBuffer ) = 0;
		virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( IMediaSample **ppSample ) = 0;
		virtual HRESULT STDMETHODCALLTYPE SetCallback( ISampleGrabberCB *pCallback, long WhichMethodToCallback ) = 0;
	};



#else
#error 
#endif

