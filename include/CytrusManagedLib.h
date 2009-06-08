// CytrusManagedLib.h

#pragma once
#include "CytrusAlgLib.h"
#include <list>
#include <iostream>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace cytrus::cameraHAL;


namespace CytrusManagedLib {

	public ref class Class1
	{
		public:
		int u;
		DirectShowCameraSource* cs;
		List<String^>^ cList;
		
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
		}

		void selectCamera(int i){
			cs->setActiveCamera(i);
		}

		void startCapture(){
			cs->startCapture();
		}

		void stopCapture(){
			cs->stopCapture();
		}

		int getU(){
			return u;
		}
	};
}
