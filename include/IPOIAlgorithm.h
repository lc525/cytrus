/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** in namespace cytrus::alg
** -----------------------------------------------------------------
** IPOIAlgorithm.h : An interface that inherits IImageConsumer, and enforces
** a contract for all the algorithms that work on Points of Interest (POI),
** for object detection ( Locator + Descriptor )
**
*/

#ifndef _IPOIALGORITHM_H_
#define _IPOIALGORITHM_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif

#include "IImageConsumer.h"
#include "ILocator.h"
#include "IDescriptor.h"

using namespace cytrus::cameraHAL;
using namespace cytrus::alg;

namespace cytrus{
	namespace alg{
		typedef void (__stdcall *POIAlgResult)(unsigned long dwSize, unsigned char* pbData);
		class CYTRUSALGLIB_API IPOIAlgorithm : public IImageConsumer{
			protected:
				ILocator *_poiLoc;
				IDescriptor *_poiDescr;
				POIAlgResult _outputAlgResult;
			public:
				IPOIAlgorithm(IImageSource* imgSrc, ILocator* poiLocator, IDescriptor* poiDescriptor, POIAlgResult outputFunc);

				//This function should be overriden to process the image data from the source.
				//It will be automatically called by the image source, when the algorithm runs.
				//If one has to display the results of the algorithm to an outside source,
				//the implementation of this function can call _outputAlgResult with appropiate data.
				virtual void processImage(unsigned long dwSize, unsigned char* pbData)=0;
				
				//Actions to take when the image from the source changes size
				virtual void onSourceSizeChange()=0;
				
				//Runs the POI Algorithm
				void run();
		};
	}
}

#endif
