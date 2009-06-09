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
** IDescriptor.h : An interface that defines the contract for describing
** the points of interest in an image.
**
*/

#ifndef _IDESCRIPTOR_H_
#define _IDESCRIPTOR_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif

namespace cytrus{
	namespace alg{
		class CYTRUSALGLIB_API IDescriptor{

		};
	}
}

#endif