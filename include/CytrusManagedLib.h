// CytrusManagedLib.h

#pragma once
#include "CytrusAlgLib.h"
#include <list>
#include <iostream>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Runtime::InteropServices;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

using namespace cytrus::cameraHAL;
using namespace cytrus::alg;


namespace CytrusManagedLib {

	delegate void CaptureCallbackProc(int dwSize, array<byte>^ pbData);
	delegate void CaptureCallbackProcU(int dwSize, unsigned char* pbData);
	public delegate void NewImageCallback();

	public ref class Class1
	{	
	private:
		static GCHandle gch;
		SurfAlg* alg;
		
		CaptureCallbackProcU^ fPtr;

		void callEventWrapperU(int dwSize, unsigned char* pbData){
			array<byte>^ byteArray = gcnew array< byte >(dwSize);
			Marshal::Copy((IntPtr)pbData,byteArray, 0, dwSize);
			callEventWrapper(dwSize,byteArray);
		}

		void callEventWrapper(int dwSize, array<byte>^ pbData){
			int stride = _camWidth * 3;
			GCHandle handle = GCHandle::Alloc(pbData, GCHandleType::Pinned);
            int scan0 = (int)handle.AddrOfPinnedObject();
            scan0 += (_camHeight - 1) * stride;
			capturedImg = gcnew Bitmap(_camWidth, _camHeight, -stride, PixelFormat::Format24bppRgb, (IntPtr)scan0);
            
            // Copy the image using the Thumbnail function to also resize if needed
            //Bitmap copyBitmap = (Bitmap)b.GetThumbnailImage(_currentCamera.CaptureWidth, _currentCamera.CaptureHeight, null, System.IntPtr.Zero);

            // Now you can free the handle
            handle.Free();

			onNewImageAvailable();
		}
	public:
		Bitmap^ capturedImg;
		int _camWidth, _camHeight;
		DirectShowCameraSource* cs;
		ObservableCollection<String^>^ cList;
		event NewImageCallback^ onNewImageAvailable;

		POIAlgResult result;
		
		Class1(){
			cList=gcnew ObservableCollection<String^>();
			std::list<char*> cLst;
			cs=DirectShowCameraSource::getCameraInstance();
			cLst=cs->getAvailableCameras();
			int nr=cLst.size();

			//Marshal camera names:
			for(std::list<char*>::iterator it=cLst.begin(); it!=cLst.end(); it++){
				String^ name=gcnew String((*it));
				cList->Add(name);
			}

			//marshalling events
			fPtr= gcnew CaptureCallbackProcU(this, &Class1::callEventWrapperU);
			gch = GCHandle::Alloc(fPtr);
			IntPtr ip = Marshal::GetFunctionPointerForDelegate(fPtr);
			result = static_cast<POIAlgResult>(ip.ToPointer());
			alg=new SurfAlg(cs, result);
		}

		!Class1(){
			gch.Free();
			delete alg;
		}

		void selectCamera(int i){
			cs->setActiveCamera(i);
		}

		void refreshCameraList(){
			cList->Clear();
			std::list<char*> cLst;
			cLst=cs->getAvailableCameras(true);
			int nr=cLst.size();

			//Marshal camera names:
			for(std::list<char*>::iterator it=cLst.begin(); it!=cLst.end(); it++){
				String^ name=gcnew String((*it));
				cList->Add(name);
			}

		}

		void displayDriverProperties(IntPtr window){
			cs->displayCameraPropertiesDialog((HWND)(window.ToPointer()));
		}

		void startCapture(){
			cs->startCapture();
		}

		void stopCapture(){
			cs->stopCapture();
		}

		void runAlg(){
			alg->run();
			_camWidth=cs->width;
			_camHeight=cs->height;
		}

	};
}
