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
** IImageConsumer.h : Image Consumer Interface
**
*/

#ifndef _IIMAGECONSUMER_H_
#define _IIMAGECONSUMER_H_

#ifndef CYTRUSALGLIB_API
	#define CYTRUSALGLIB_API
#endif

namespace cytrus{
	namespace cameraHAL{

		class IImageSource;

		//
		// Abstract base class for ImageConsumers
		// All the classes that make use of image sources should implement this, so that they can be notified
		// with images when those are available at the source.
		//
		class CYTRUSALGLIB_API IImageConsumer{
			protected:
				IImageSource* _imgSource;
			public: 
				
				IImageConsumer(IImageSource* imgSource);
				//
				// This function is called by the image source when the image data is ready
				// (has been loaded from disk, has been recieved from a camera etc.). 
				//
				virtual void processImage(unsigned long dwSize, char* pbData)=0;

				//
				// This function is called by the image source when the size of the image
				// changes.
				//
				virtual void onSourceSizeChange()=0;
		};
	
	}
}


#endif