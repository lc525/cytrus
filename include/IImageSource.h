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
** IImageSource.h : Image Source Interface
**
*/

#ifndef _IIMAGESOURCE_H_
#define _IIMAGESOURCE_H_

#ifndef CYTRUSALGLIB_API
	#define CYTRUSALGLIB_API
#endif

#include <set>
#include "IImageConsumer.h"

namespace cytrus{
	namespace cameraHAL{
		
		// Abstract base class for all image sources.
		// Defines the functions and members for registering and removing image consumers (IImageConsumer)
		// Register your image consumer by calling registerImageConsumer, and the processImage function
		// will be called by the image source when an image is available.
		// Current available implementations:
		//	cytrus::cameraHal::DirectShowCameraSource - win32/directx only
		//	cytrus::cameraHal::ImageFromFileSource
		// Info for subclassing:
		// If your source provides external consumer notification (like function callbacks provided by a
		// SO-specific image aquisition system), you should implement those in separate (static?) functions. 
		// In derived classes, implement the four pure virtual functions,
		//	notifyConsumers() - for manual consumer notification
		//	getImageSize() - for providing source-dependent image size information
		class CYTRUSALGLIB_API IImageSource{
			protected: 
				std::set<IImageConsumer*> consumers;
			public:

				virtual ~IImageSource();

				//
				// This function manually notifies all the registered consumers.
				// The implementation usually calls IImageConsumer.processImage, passing the source's image
				// as a parameter.
				//
				virtual void notifyConsumers()=0;

				//
				// Returns the size <width,height> of the images that this
				// source currently provides.
				//
				virtual std::pair<int,int> getImageSize()=0;

				//
				// Starts the capture on the specific image source. This function will be called
				// after configuring the image source. 
				// Implementation examples:
				//	For a static single-image source, calling the startCapture function should read the
				// image from disk and then notify all the consumers.
				//	For a stream-type image source, calling the startCapture should determine periodic
				// notifications to the consumers, as the frames come from the capture device.
				//
				//
				virtual void startCapture()=0;

				//
				// Stops the capture process on the specific image source.
				//
				virtual void stopCapture()=0;

				//
				// Default implementation does nothing. Subclasses might overwrite this to account for the cases when
				// the size of the image provided by the source changes. Maybe it's a camera source and the user
				// connected a second camera that provides another resolution (for example)
				//
				virtual void notifySizeChange(){};

				//
				// Registers an ImageConsumer with this source. 
				// The registered ImageConsumers will be notified when new images are produced by the source,
				// and the image will be sent to them for processing. 
				// 
				// returns true if the consumer could be registered correctly
				//         false if the consumer already exists in the list.
				//
				bool registerImageConsumer(IImageConsumer *c);

				//
				// Remove Image Consumer from the consumers that are waiting for this image source.
				// returns true if the consumer could be removed correctly
				//         false if the consumer did not previously exist in the list.
				//
				bool removeImageConsumer(IImageConsumer *c);
		};
	
	}
}


#endif
