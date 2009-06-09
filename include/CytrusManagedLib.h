// CytrusManagedLib.h

#pragma once
#include "CytrusAlgLib.h"
#include <list>
#include <iostream>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace cytrus::cameraHAL;
using namespace cytrus::alg;


namespace CytrusManagedLib {

	public delegate void CaptureCallbackProc(int dwSize, array<byte>^ pbData);

	public ref class Class1
	{	
	private:
		static GCHandle gch;
		
		CaptureCallbackProc^ fPtr;
		void callEventWrapper(int dwSize, array<byte>^ pbData){
			onImageAvailable(dwSize,pbData);
		}
	public:
		DirectShowCameraSource* cs;
		List<String^>^ cList;
		event CaptureCallbackProc^ onImageAvailable;

		POIAlgResult result;
		
		Class1(){
			cList=gcnew List<String^>();
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
			fPtr= gcnew CaptureCallbackProc(this, &Class1::callEventWrapper);
			gch = GCHandle::Alloc(fPtr);
			IntPtr ip = Marshal::GetFunctionPointerForDelegate(fPtr);
			result = static_cast<POIAlgResult>(ip.ToPointer());
		}

		void selectCamera(int i){
			cs->setActiveCamera(i);
		}

		void startCapture(){
			cs->startCapture();
		}

		void stopCapture(){
			cs->stopCapture();
			gch.Free();
		}

		void runAlg(){
			SurfAlg* alg=new SurfAlg(cs, result);
			alg->run();
		}

	};
}
