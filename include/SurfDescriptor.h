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
** SurfDescriptor.h : Describe POI's
** matrix. 
**
** Some parts are adapted from Bay's SURF implementation/notes:
** C. Evans, Research Into Robust Visual Features,
** MSc University of Bristol, 2008.     
**
*/

#ifndef _SURFDESCRIPTOR_H_
#define _SURFDESCRIPTOR_H_

#ifndef CYTRUSALGLIB_API
    #ifdef CYTRUSALGLIB_EXPORTS
        #define CYTRUSALGLIB_API __declspec(dllexport)
    #else
        #define CYTRUSALGLIB_API __declspec(dllimport)
    #endif
#endif

#define PI  3.1415926535897932384626433832795

#include <vector>
#include <iostream>
#include "math.h"
#include "poi.h"
#include "IDescriptor.h"

namespace cytrus{
    namespace alg{
		const float pi = float(PI);

        template <typename IntegralImageView>
        class SurfDescriptor:public IDescriptor{

            private:

				void getOrientation(Poi& pt);
				void getDescriptor(Poi& pt);

				inline float gaussian(int x, int y, float sig);
				inline float gaussian(float x, float y, float sig);
				inline float haarX(int row, int column, int s);
				inline float haarY(int row, int column, int s);
				float getAngleFromOrigin(float x, float y);

            protected:

            IntegralImageView* _img;
            int i_width, i_height;
			bool _oriented;

            public:
                SurfDescriptor(IntegralImageView& intImg, bool oriented=true);

				// Delayed image init constructor. Call setSourceIntegralImg to set image.
                SurfDescriptor(bool oriented=true);

				virtual ~SurfDescriptor(){}

                void setSourceIntegralImg(IntegralImageView& intImg);
                void setParameters(const int octaves, 
                                   const int intervals, 
                                   const int sampling , 
                                   const float threshold);
                
                virtual void getDescriptorsFor(std::vector<Poi>& iPts_out);
        };
    }
}

#include "SurfDescriptor.hpp"

#endif