/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** in namespace cytrus::cameraHAL
** -----------------------------------------------------------------
** DirectShowCameraSource.h : Camera Image Source. Implements IImageSource
**
*/

#ifndef _CAMERASOURCE_H_
#define _CAMERASOURCE_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif

#include <list>
#include "WebCamLib.h"
#include "IImageSource.h"

#define NO_CAMERA -256

namespace cytrus{
	namespace cameraHAL{

		#ifdef WIN32
		//
		// DirectShowCameraSource is an image source aquiring images from camera devices
		// (connected by usb, firewire etc) through DirectShow/DirectX
		// As such, this is a Windows-specific implementation.
		// Not threadsafe (yet)
		//
		typedef void (__stdcall *NewImageAvailableCallback)(void);

		class CYTRUSALGLIB_API DirectShowCameraSource: public IImageSource{
			// TODO: add thread safetyness to singleton & other members.
			private:
				static DirectShowCameraSource* instance;  // singleton instance
				static NewImageAvailableCallback signalNewImageAvailable;
				IUnknown** deviceHandle;

				static DWORD imageDataSize;
				static BYTE* imageData;
				
				// current capture device spec:
				int _currentCamera;
				bool _cameraIsStarted;
				//
				
				DirectShowCameraSource();
				virtual ~DirectShowCameraSource();
				void getCameraList();

			public:

				int width, height;
				int _nrAvailableCameras;
				std::list<char*> _availableCameras;


				static DirectShowCameraSource* getCameraInstance(NewImageAvailableCallback callback);
				std::list<char*> getAvailableCameras(bool refresh=true);
				void setActiveCamera(int cIndex);
				void displayCameraPropertiesDialog(HWND hwnd);

				virtual void notifyConsumers();
				virtual void notifySizeChange(){};

				virtual void startCapture();
				virtual void stopCapture();
				virtual std::pair<int,int> getImageSize();

				static void __stdcall callbackFunc(DWORD dwSize, BYTE* pbData);
		};
		#endif
	
	}
}


#endif