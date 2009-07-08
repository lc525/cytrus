/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** ImageFileManaged.h Assures .NET managed wrappers for the image file unmanaged cytrus code
**
*/

#pragma once
#include "CytrusAlgLib.h"
#include "CytrusManagedLib.h"

using namespace System::Runtime::InteropServices;
using namespace cytrus::cameraHAL;
using namespace cytrus::alg;


namespace cytrus {
	namespace managed{
	
		public ref class ImageFileMgr
		{	
		private:
			GCHandle gch;
			IPOIAlgorithm* alg;
			bool isInvalid;

			RenderResultCallbackProc^ fPtr;
			FileImageSource* fs;
			char* filePath;
			
			ObservableCollection<OutputMode^>^ outputModes;
			POIAlgResult result;
			NewImageAvailableCallback newImageAvailable;

			void callImageCaptureEvent(int dwSize, unsigned char* pbData, int index);

		public:

			property int _imgWidth
			{
				int get()
				{
					if(fs!=NULL)
						return fs->width;
					return 0;
				}
			}

			property int _imgHeight
			{
				int get()
				{
					if(fs!=NULL)
						return fs->height;
					return 0;
				}
			}

			// Treat this event to recieve the images from the selected camera
			// as an array of bytes.
			event ImageCaptureCallback^ onImageAvailableForRendering;
			
			ImageFileMgr();
			!ImageFileMgr();
			~ImageFileMgr();

			void setActiveOutputMode(int modeIndex);
			ObservableCollection<OutputMode^>^ getOutputModesList();
			bool setProcessingSize(int width, int height);
			void setImagePath(String^ path);

			void startImageProcessing();
			void freeResources();

		};
	
	}
}
