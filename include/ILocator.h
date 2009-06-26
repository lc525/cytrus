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
** ILocator.h : An interface that defines the contract for locating
** the points of interest in an image.
**
*/

#ifndef _ILOCATOR_H_
#define _ILOCATOR_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif

#include <vector>
#include "poi.h"

namespace cytrus{
	namespace alg{
		class CYTRUSALGLIB_API ILocator{
			public:
				virtual void locatePOIInImage(std::vector<Poi>& iPts_out)=0;
		};
	}
}

#endif