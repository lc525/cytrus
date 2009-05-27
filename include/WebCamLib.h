/*~External (not created by Carata Lucian as part of the B.Sc Thesis)
** 
** Original Author: Mike Wasserman
** Project: Touchless; http://touchless.codeplex.com/
** License notice: OpenSource(Ms-PL); 
**				   please read the License.txt file included.
** Modifications (by Carata Lucian): small, concerning memory leaks
** and project structure.
**
** -----------------------------------------------------------------
** WebCamLib.h : Dll function declarations. The dll is used for 
** low-level camera operations.
**
*/
#ifndef _WEBCAMLIB_H_
#define _WEBCAMLIB_H_
#include <dshow.h>
#include <qedit.h>
#include <strsafe.h>


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 );

////// Private variables
#define MAX_CAMERAS		10

struct CameraInfo
{
	BSTR bstrName;
	IMoniker* pMoniker;
};

IGraphBuilder* g_pGraphBuilder = NULL;
IMediaControl* g_pMediaControl = NULL;
ICaptureGraphBuilder2* g_pCaptureGraphBuilder = NULL;
IBaseFilter* g_pIBaseFilterCam = NULL;
IBaseFilter* g_pIBaseFilterSampleGrabber = NULL;
IBaseFilter* g_pIBaseFilterNullRenderer = NULL;
CameraInfo g_aCameraInfo[MAX_CAMERAS] = {0};

typedef void (__stdcall *PFN_CaptureCallback)(DWORD dwSize, BYTE* pbData);
PFN_CaptureCallback g_pfnCaptureCallback = NULL;

///// Forward declarations
HRESULT ConfigureSampleGrabber(IBaseFilter *pIBaseFilter);
void CleanupCameraInfo();
extern "C" __declspec(dllexport) DWORD APIENTRY Cleanup();
extern "C" __declspec(dllexport) DWORD APIENTRY StopCamera();

void MyFreeMediaType(AM_MEDIA_TYPE& mt);

////// Exported functions
extern "C"
{
__declspec(dllexport) DWORD APIENTRY RefreshCameraList(int *nCount);
__declspec(dllexport) DWORD APIENTRY GetCameraDetails(int index, IUnknown **ppUnk, BSTR* pbstrName);
__declspec(dllexport) DWORD APIENTRY DisplayCameraPropertiesDialog(IUnknown *pUnk, HWND hwnd);
__declspec(dllexport) DWORD APIENTRY StartCamera(IUnknown *pUnk, PFN_CaptureCallback pfnCallback, int* pnWidth, int* pnHeight);
__declspec(dllexport) DWORD APIENTRY StopCamera();
__declspec(dllexport) DWORD APIENTRY Initialize();
__declspec(dllexport) DWORD APIENTRY Cleanup();
}

// SampleGrabber callback interface
class MySampleGrabberCB : public ISampleGrabberCB
{
public:

	MySampleGrabberCB();

	 virtual HRESULT STDMETHODCALLTYPE SampleCB( 
            double SampleTime,
            IMediaSample *pSample);
        
     virtual HRESULT STDMETHODCALLTYPE BufferCB( 
            double SampleTime,
            BYTE *pBuffer,
            long BufferLen);

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
            REFIID riid,
            void **ppvObject);
        
	virtual ULONG STDMETHODCALLTYPE AddRef();
        
	virtual ULONG STDMETHODCALLTYPE Release();

private:
	int m_nRefCount;
};

HRESULT ConfigureSampleGrabber(IBaseFilter *pIBaseFilter);


#endif