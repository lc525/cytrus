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
** WebCamLib.cpp : Low-level camera operations.
**                 Configures DirectShow filters for grabbing camera
**                 data.
**
*/

#include "stdafx.h"
#include "WebCamLib.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

IGraphBuilder* g_pGraphBuilder = NULL;
IMediaControl* g_pMediaControl = NULL;
ICaptureGraphBuilder2* g_pCaptureGraphBuilder = NULL;
IBaseFilter* g_pIBaseFilterCam = NULL;
IBaseFilter* g_pIBaseFilterSampleGrabber = NULL;
IBaseFilter* g_pIBaseFilterNullRenderer = NULL;
CameraInfo g_aCameraInfo[MAX_CAMERAS] = {0};

PFN_CaptureCallback g_pfnCaptureCallback = NULL;

///// Forward declarations
void MyFreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        // Unecessary because pUnk should not be used, but safest.
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

////// Exported functions
extern "C"
{

__declspec(dllexport) DWORD APIENTRY RefreshCameraList(int *nCount)
{
	IEnumMoniker* pclassEnum = NULL;
	ICreateDevEnum* pdevEnum = NULL;

	if (NULL == nCount)
	{
		return E_INVALIDARG;
	}

	*nCount = 0;

	CleanupCameraInfo();

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
			NULL, 
			CLSCTX_INPROC, 
			IID_ICreateDevEnum, 
			(LPVOID*)&pdevEnum);

	if (SUCCEEDED(hr))
	{
        hr = pdevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pclassEnum, 0);
	}

	if (pdevEnum != NULL)
	{
		pdevEnum->Release();
		pdevEnum = NULL;
	}

	if (pclassEnum != NULL)
	{ 
	    IMoniker* apIMoniker[1];
		ULONG ulCount = 0;

        while (SUCCEEDED(hr) && (*nCount) < MAX_CAMERAS && pclassEnum->Next(1, apIMoniker, &ulCount) == S_OK)
        {
			g_aCameraInfo[*nCount].pMoniker = apIMoniker[0];

			IPropertyBag *pPropBag;
			hr = apIMoniker[0]->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
	        if (SUCCEEDED(hr))
	        {
	            // To retrieve the filter's friendly name, do the following:
	            VARIANT varName;
	            VariantInit(&varName);
	            hr = pPropBag->Read(L"FriendlyName", &varName, 0);
	            if (SUCCEEDED(hr) && varName.vt == VT_BSTR)
	            {
					g_aCameraInfo[*nCount].bstrName = SysAllocString(varName.bstrVal);
	            }
	            VariantClear(&varName);
	            
	            pPropBag->Release();
	        }

			(*nCount)++;
        }

		pclassEnum->Release();
	}

	return hr;
}

__declspec(dllexport) DWORD APIENTRY GetCameraDetails(int index, IUnknown **ppUnk, BSTR* pbstrName)
{
	if (index >= MAX_CAMERAS || NULL == g_aCameraInfo[index].pMoniker || NULL == pbstrName)
	{
		return E_INVALIDARG;
	}

	g_aCameraInfo[index].pMoniker->AddRef();
	*ppUnk = g_aCameraInfo[index].pMoniker;
	*pbstrName = SysAllocString(g_aCameraInfo[index].bstrName);

	return S_OK;
}

__declspec(dllexport) DWORD APIENTRY DisplayCameraPropertiesDialog(IUnknown *pUnk, HWND hwnd)
{
	if (NULL == pUnk)
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;
	IMoniker *pMoniker = NULL;
	IBaseFilter *pFilter = NULL;
	ISpecifyPropertyPages *pProp = NULL;

	// Grab the moniker interface
	hr = pUnk->QueryInterface(IID_IMoniker, (LPVOID*)&pMoniker);

	// Create a filter graph for the moniker
	if (SUCCEEDED(hr))
	{ 
		hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (LPVOID*)&pFilter);
	}

	// See if it implements a property page
	if (SUCCEEDED(hr))
	{
		hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (LPVOID*)&pProp);
	}

	// Show the property page
	if (SUCCEEDED(hr))
	{
		FILTER_INFO filterinfo;
		hr = pFilter->QueryFilterInfo(&filterinfo);

		IUnknown *pFilterUnk = NULL;
		if (SUCCEEDED(hr))
		{
			hr = pFilter->QueryInterface(IID_IUnknown, (LPVOID*)&pFilterUnk);
		}

		if (SUCCEEDED(hr))
		{
			CAUUID caGUID;
			pProp->GetPages(&caGUID);

			OleCreatePropertyFrame(
		        hwnd,                   // Parent window
		        0, 0,                   // Reserved
		        filterinfo.achName,     // Caption for the dialog box
		        1,                      // Number of objects (just the filter)
		        &pFilterUnk,            // Array of object pointers. 
		        caGUID.cElems,          // Number of property pages
		        caGUID.pElems,          // Array of property page CLSIDs
		        0,                      // Locale identifier
		        0, NULL                 // Reserved
				);
		}

		if (pFilterUnk != NULL)
		{
			pFilterUnk->Release();
			pFilterUnk = NULL;
		}
	}

	if (pProp != NULL)
	{
		pProp->Release();
		pProp = NULL;
	}

	if (pMoniker != NULL)
	{
		pMoniker->Release();
		pMoniker = NULL;
	}

	if (pFilter != NULL)
	{
		pFilter->Release();
		pFilter = NULL;
	}

	return hr;
}

__declspec(dllexport) DWORD APIENTRY StartCamera(IUnknown *pUnk, PFN_CaptureCallback pfnCallback, int* pnWidth, int* pnHeight)
{
	if (NULL == pUnk || NULL == pfnCallback || NULL == pnWidth || NULL == pnHeight)
	{
		return E_INVALIDARG;
	}

	if (g_pGraphBuilder != NULL)
	{
		return E_UNEXPECTED;
	}

	g_pfnCaptureCallback = pfnCallback;
	
	IMoniker *pMoniker = NULL;
	HRESULT hr = S_OK;

	// Grab the moniker interface
	hr = pUnk->QueryInterface(IID_IMoniker, (LPVOID*)&pMoniker);

	// Build all the necessary interfaces to start the capture
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_FilterGraph, 
			NULL, 
			CLSCTX_INPROC, 
			IID_IGraphBuilder, 
			(LPVOID*)&g_pGraphBuilder);

	}

	if (SUCCEEDED(hr))
	{
		hr = g_pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&g_pMediaControl);
	}

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, 
			NULL, 
			CLSCTX_INPROC, 
			IID_ICaptureGraphBuilder2, 
			(LPVOID*)&g_pCaptureGraphBuilder);
	}

	// Setup the filter graph
	if (SUCCEEDED(hr))
	{
		 hr = g_pCaptureGraphBuilder->SetFiltergraph(g_pGraphBuilder);
	}

	// Build the camera from the moniker
	if (SUCCEEDED(hr))
	{ 
		hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (LPVOID*)&g_pIBaseFilterCam);
	}

	// Add the camera to the filter graph
	if (SUCCEEDED(hr))
	{
		hr = g_pGraphBuilder->AddFilter(g_pIBaseFilterCam, L"WebCam");
	}

	// Create a SampleGrabber
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&g_pIBaseFilterSampleGrabber);
	}

	// Configure the Sample Grabber
	if (SUCCEEDED(hr))
	{
		hr = ConfigureSampleGrabber(g_pIBaseFilterSampleGrabber);
	}

	// Add Sample Grabber to the filter graph
	if (SUCCEEDED(hr))
	{
		hr = g_pGraphBuilder->AddFilter(g_pIBaseFilterSampleGrabber, L"SampleGrabber");
	}

	// Create the NullRender
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&g_pIBaseFilterNullRenderer);
	}

	// Add the Null Render to the filter graph
	if (SUCCEEDED(hr))
	{
		hr = g_pGraphBuilder->AddFilter(g_pIBaseFilterNullRenderer, L"NullRenderer");
	}

	// Configure the render stream
	if (SUCCEEDED(hr))
	{
		hr = g_pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, g_pIBaseFilterCam, g_pIBaseFilterSampleGrabber, g_pIBaseFilterNullRenderer);
	}

	// Grab the capture width and height
	if (SUCCEEDED(hr))
	{
		ISampleGrabber* pGrabber = NULL;
		hr = g_pIBaseFilterSampleGrabber->QueryInterface(IID_ISampleGrabber, (LPVOID*)&pGrabber);
		if (SUCCEEDED(hr))
		{
			AM_MEDIA_TYPE mt;
			hr = pGrabber->GetConnectedMediaType(&mt);
			if (SUCCEEDED(hr))
			{
				VIDEOINFOHEADER *pVih;
				if ((mt.formattype == FORMAT_VideoInfo) && 
				    (mt.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
				    (mt.pbFormat != NULL) ) 
				{
				    pVih = (VIDEOINFOHEADER*)mt.pbFormat;
					*pnWidth = pVih->bmiHeader.biWidth;
					*pnHeight = pVih->bmiHeader.biHeight;
				}
				else
				{
					hr = E_FAIL;  // Wrong format
				}

				MyFreeMediaType(mt);  //TODO: Fix this as its leaking - DONE(lucian carata)

			}
		}

		if (pGrabber != NULL)
		{
			pGrabber->Release();
			pGrabber = NULL;
		}
	}

	// Start the capture
	if (SUCCEEDED(hr))
	{
		hr = g_pMediaControl->Run();
	}

	// If init fails then ensure that you cleanup
	if (FAILED(hr))
	{
		StopCamera();
	}
	else
	{
		hr = S_OK;  // Make sure we return S_OK for success
	}

	// Cleanup
	if (pMoniker != NULL)
	{
		pMoniker->Release();
		pMoniker = NULL;
	}

	return hr;
}

__declspec(dllexport) DWORD APIENTRY StopCamera()
{
	HRESULT hr = S_OK;
	
	if (g_pMediaControl != NULL)
	{
		g_pMediaControl->Stop();
		g_pMediaControl->Release();
		g_pMediaControl = NULL;
	}

	g_pfnCaptureCallback = NULL;

	if (g_pIBaseFilterNullRenderer != NULL)
	{
		g_pIBaseFilterNullRenderer->Release();
		g_pIBaseFilterNullRenderer = NULL;
	}

	if (g_pIBaseFilterSampleGrabber != NULL)
	{
		g_pIBaseFilterSampleGrabber->Release();
		g_pIBaseFilterSampleGrabber = NULL;
	}

	if (g_pIBaseFilterCam != NULL)
	{
		g_pIBaseFilterCam->Release();
		g_pIBaseFilterCam = NULL;
	}

	if (g_pGraphBuilder != NULL)
	{
		g_pGraphBuilder->Release();
		g_pGraphBuilder = NULL;
	}

	if (g_pCaptureGraphBuilder != NULL)
	{
		g_pCaptureGraphBuilder->Release();
		g_pCaptureGraphBuilder = NULL;
	}

	return hr;
}

__declspec(dllexport) DWORD APIENTRY Initialize()
{
	HRESULT hr = S_OK;

	// Nothing to really do yet

	return hr;
}

__declspec(dllexport) DWORD APIENTRY Cleanup()
{
	StopCamera();	
	CleanupCameraInfo();

	return S_OK;
}

}

__declspec(dllexport) void CleanupCameraInfo()
{
	for (int n = 0; n < MAX_CAMERAS; n++)
	{
		SysFreeString(g_aCameraInfo[n].bstrName);
        g_aCameraInfo[n].bstrName = NULL;

		if (g_aCameraInfo[n].pMoniker != NULL)
		{
			g_aCameraInfo[n].pMoniker->Release();
			g_aCameraInfo[n].pMoniker = NULL;
		}
	}
}

// SampleGrabber callback interface
	MySampleGrabberCB::MySampleGrabberCB()
	{
		m_nRefCount = 0;
	}

	HRESULT STDMETHODCALLTYPE MySampleGrabberCB::SampleCB( 
            double SampleTime,
            IMediaSample *pSample)
	 {
		 return E_FAIL;
	 }
        
     HRESULT STDMETHODCALLTYPE MySampleGrabberCB::BufferCB( 
            double SampleTime,
            BYTE *pBuffer,
            long BufferLen)
	 {
		if (g_pfnCaptureCallback != NULL)
		{
			g_pfnCaptureCallback(BufferLen, pBuffer);
		}

		return S_OK;
	 }

	HRESULT STDMETHODCALLTYPE MySampleGrabberCB::QueryInterface( 
            REFIID riid,
            void **ppvObject)
	 {
		 return E_FAIL;  // Not a very accurate implementation
	 }
        
	ULONG STDMETHODCALLTYPE MySampleGrabberCB::AddRef()
	{
		return ++m_nRefCount;
	}
        
	ULONG STDMETHODCALLTYPE MySampleGrabberCB::Release()
	{
		int n = --m_nRefCount;
		if (n <= 0)
		{
			delete this;
		}
		return n;
	}

HRESULT ConfigureSampleGrabber(IBaseFilter *pIBaseFilter)
{
	HRESULT hr = S_OK;

	ISampleGrabber *pGrabber = NULL;

	hr = pIBaseFilter->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
	if (SUCCEEDED(hr))
	{
		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		mt.subtype = MEDIASUBTYPE_RGB24;
		mt.formattype = FORMAT_VideoInfo;
		hr = pGrabber->SetMediaType(&mt);
	}

	if (SUCCEEDED(hr))
	{
		hr = pGrabber->SetCallback(new MySampleGrabberCB(), 1);
	}

	if (pGrabber != NULL)
	{
		pGrabber->Release();
		pGrabber = NULL;
	}

	return hr;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif

