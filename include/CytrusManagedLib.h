/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** CytrusManagedLib.h Assures .NET managed wrappers for the unmanaged cytrus code
**
*/

#pragma once
#include "CytrusAlgLib.h"
#include <list>
#include <iostream>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Runtime::InteropServices;

using namespace cytrus::cameraHAL;
using namespace cytrus::alg;


namespace cytrus {
	namespace managed{

		delegate void CaptureCallbackProc(int dwSize, unsigned char* pbData);
		public delegate void ImageCaptureCallback(array<byte>^ pbData);

		public ref class CameraMgr
		{	
		private:
			static GCHandle gch;
			IPOIAlgorithm* alg;
			CaptureCallbackProc^ fPtr;
			DirectShowCameraSource* cs;
			ObservableCollection<String^>^ cList;
			POIAlgResult result;

			void callImageCaptureEvent(int dwSize, unsigned char* pbData);

		public:
			int _camWidth, _camHeight;

			// Treat this event to recieve the images from the selected camera
			// as an array of bytes.
			event ImageCaptureCallback^ onNewImageAvailable;
			
			CameraMgr();
			!CameraMgr();

			void selectCamera(int i);
			void refreshCameraList();
			ObservableCollection<String^>^ getCameraList();
			void showPropertiesDialog(IntPtr window);

			void startCapture();
			void stopCapture();

		};
	}
}
