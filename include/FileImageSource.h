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
** FileImageSource.h : File Image Source. Implements IImageSource
**
*/

#ifndef _FILEIMAGESOURCE_H_
#define _FILEIMAGESOURCE_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif

#include "IImageSource.h"
#include <boost/gil/gil_all.hpp>


namespace cytrus{
	namespace cameraHAL{

		//
		// FileImageSource is a source that reads images from files. 
		// <Limitation> Currently, the implementation only accepts .jpg files
		//
		class CYTRUSALGLIB_API FileImageSource: public IImageSource{
			private:

				unsigned long imageDataSize;
				unsigned char* imageData;
				const char* _path;
				boost::gil::rgb8_image_t* imageFile;
				boost::gil::rgb8_view_t* imageFileView;

			public:

				int width, height;
				
				FileImageSource();
				virtual ~FileImageSource();

				virtual void notifyConsumers();
				virtual void notifyConsumer(int consumerIndex);
				virtual void notifySizeChange(){};

				virtual void startCapture();
				virtual void stopCapture();
				virtual void setPath(const char* path);
				virtual std::pair<int,int> getImageSize();

		};
	
	}
}


#endif