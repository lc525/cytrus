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
** ObjectPoiStorage.h : Stores the interest points for objects to be detected
**
*/

#ifndef _OBJECTPOISTORAGE_H_
#define _OBJECTPOISTORAGE_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif
#define FLT_MAX         3.402823466e+38F        /* max value */

#include "IPOIAlgorithm.h"

namespace cytrus{
	namespace alg{

		class CYTRUSALGLIB_API ObjectPoiStorage{
			// TODO: add thread safetyness to singleton & other members.
			private:
				static ObjectPoiStorage* instance;  // singleton instance
				static std::vector<std::vector<Poi>*> _registeredObjects;
				
				ObjectPoiStorage();
				virtual ~ObjectPoiStorage();

				static float comparePOIs(Poi& p1, Poi& p2);

			public:

				static ObjectPoiStorage* getInstance();
				static int registerObject(int startX, int startY, int width, int height, IPOIAlgorithm* poiSource);
				static void removeObject(int index);
				static void matchObjects(std::vector<Poi>& iPts);
		};
	
	}
}


#endif