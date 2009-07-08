/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** CytrusManagedLib.h Assures .NET managed camera wrappers for the unmanaged cytrus code
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
using namespace System::Windows::Media;

using namespace cytrus::cameraHAL;
using namespace cytrus::alg;


namespace cytrus {
	namespace managed{

		delegate void RenderResultCallbackProc(int dwSize, unsigned char* pbData, int index);
		delegate void NewImageCallback();
		public delegate void ImageCaptureCallback(array<byte>^ pbData, List<Poi_m^>^ poiData);

		public ref struct OutputMode{
			String^ modeName;
			PixelFormat^ pixelFormat;

			virtual String^ ToString() override
			{
				return modeName;
			}
		};

		//public delegate void OutputModeCallback(OutputMode^ newMode); // outputMode change events (not used)

		// Managed class that is responsible for camera and capture management. It uses the unmanaged classes from cytrus::alg and cytrus::hal
		// for running the SURF algorithm in real time on the captured video.
		//
		// This class also implements coarse-grained parallelism, dispatching work on multiple threads (there are multiple
		// separate instances of the algorithm working in the same time)
		public ref class CameraMgr
		{	
		private:
			static GCHandle gch, nigch;
			static int thNr=0;
			//static int lastdwSize; // outputMode change events (not used)
			std::list<IPOIAlgorithm*>* alg_ProcessingPool;
			Dictionary<int, int>^ threadIndexes;

			RenderResultCallbackProc^ fPtr;
			NewImageCallback^ newImage;
			DirectShowCameraSource* cs;
			ObservableCollection<String^>^ cList;
			
			ObservableCollection<OutputMode^>^ outputModes;
			POIAlgResult result;
			NewImageAvailableCallback newImageAvailable;

			void callImageCaptureEvent(int dwSize, unsigned char* pbData, int index);
			void newImageAvailableEvent();
			void cameraNotifyConsumers(Object^ o);

		public:
			int _camWidth, _camHeight;

			// Treat this event to recieve the images from the selected camera
			// as an array of bytes.
			event ImageCaptureCallback^ onImageAvailableForRendering;
			//event OutputModeCallback^ onOutputModeChange; // outputMode change events (not used)
			
			CameraMgr();
			!CameraMgr();

			void selectCamera(int i);
			void refreshCameraList();
			ObservableCollection<String^>^ getCameraList();
			void showPropertiesDialog(IntPtr window);

			void setActiveOutputMode(int modeIndex);
			ObservableCollection<OutputMode^>^ getOutputModesList();
			bool setProcessingSize(int width, int height);

			void startCapture();
			void stopCapture();

		};
	}
}
