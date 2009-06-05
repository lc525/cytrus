// CytrusManagedLib.h

#pragma once
#include "CytrusAlgLib.h"
#include <list>
#include <iostream>

using namespace System;
using namespace cytrus::cameraHAL;

namespace CytrusManagedLib {

	public ref class Class1
	{
	public:
		int u;
		Class1(){
			//DirectShowCameraSource p;
			DirectShowCameraSource* cs=DirectShowCameraSource::getCameraInstance();
			std::list<char*> lst=cs->getAvailableCameras();
		}

		int getU(){
			return u;
		}
	};
}
