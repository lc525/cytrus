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

//-------------------------------------------------------
template <typename IntegralImageView>
SurfDescriptor<IntegralImageView>::SurfDescriptor(IntegralImageView& intImg, bool oriented){
    _img=&intImg;
    i_width=_img->width();
    i_height=_img->height();  
	_oriented=oriented;
}

template <typename IntegralImageView>
SurfDescriptor<IntegralImageView>::SurfDescriptor(bool oriented){ 
	_oriented=oriented;
}

template <typename IntegralImageView>
void SurfDescriptor<IntegralImageView>::setSourceIntegralImg(IntegralImageView& intImg){
    _img=&intImg;
    i_width=_img->width();
    i_height=_img->height();  

}

template <typename IntegralImageView>
void SurfDescriptor<IntegralImageView>::getDescriptorsFor(std::vector<Poi>& iPts_out){
	for(std::vector<Poi>::iterator it=iPts_out.begin(); it!=iPts_out.end(); it++){
		if(_oriented){
			getOrientation(*it);
		}
		getDescriptor(*it);
	}
}

template <typename IntegralImageView>
void SurfDescriptor<IntegralImageView>::getOrientation(Poi& ipt){
  float gauss, scale = ipt.scale;
  int s = floor(scale), r = floor(ipt.y), c = floor(ipt.x);
  std::vector<float> haarResX, haarResY, angles;

  // calculate haar responses for points within radius of 6*scale
  for(int i = -6*s; i <= 6*s; i+=s) 
  {
    for(int j = -6*s; j <= 6*s; j+=s) 
    {
      if( i*i + j*j  < 36*s*s ) 
      {
        gauss = gaussian(i, j, 2.5f*s);
        haarResX.push_back( gauss * haarX(r+j, c+i, 4*s) );
        haarResY.push_back( gauss * haarY(r+j, c+i, 4*s) );
        angles.push_back(getAngleFromOrigin(haarResX.back(), haarResY.back()));
      }
    }
  }

  // calculate the dominant direction 
  float sumX, sumY;
  float max=0, old_max = 0, orientation = 0, old_orientation = 0;
  float ang1, ang2, ang;

  // loop slides pi/3 window around feature point
  for(ang1 = 0; ang1 < 2*pi;  ang1+=0.2f) {
    ang2 = ( ang1+pi/3.0f > 2*pi ? ang1-5.0f*pi/3.0f : ang1+pi/3.0f);
    sumX = sumY = 0; 
    for(unsigned int k = 0; k < angles.size(); k++) 
    {
      // get angle from the x-axis of the sample point
      ang = angles.at(k);

      // determine whether the point is within the window
      if (ang1 < ang2 && ang1 < ang && ang < ang2) 
      {
        sumX+=haarResX.at(k);  
        sumY+=haarResY.at(k);
      } 
      else if (ang2 < ang1 && 
        ((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2*pi) )) 
      {
        sumX+=haarResX.at(k);  
        sumY+=haarResY.at(k);
      }
    }

    // if the vector produced from this window is longer than all 
    // previous vectors then this forms the new dominant direction
    if (sumX*sumX + sumY*sumY > max) 
    {
      // store largest orientation
      max = sumX*sumX + sumY*sumY;
      orientation = getAngleFromOrigin(sumX, sumY);
    }
  }

  // assign orientation of the dominant response vector
  ipt.orientation = orientation;
}

template <typename IntegralImageView>
void SurfDescriptor<IntegralImageView>::getDescriptor(Poi& ipt){
  int y, x, count=0;
  float scale, *desc, dx, dy, mdx, mdy, co, si;
  float gauss, rx, ry, rrx, rry, sample_x, sample_y, len=0;
  
  scale = ipt.scale;
  x = floor(ipt.x);
  y = floor(ipt.y);  
  co = cos(ipt.orientation);
  si = sin(ipt.orientation);
  desc = ipt.getDescriptor();

  // Calculate descriptor for this interest point
  for (int i = -10; i < 10; i+=5)
  {
    for (int j = -10; j < 10; j+=5) 
    {
      dx=dy=mdx=mdy=0;
      for (int k = i; k < i + 5; ++k) 
      {
        for (int l = j; l < j + 5; ++l) 
        {
			if(_oriented){
			  // Get coords of sample point on the rotated axis
			  sample_x = floor(x + (-l*scale*si + k*scale*co));
			  sample_y = floor(y + ( l*scale*co + k*scale*si));
			}
			else{
				sample_x = l*scale+x;
				sample_y = k*scale+y;
			}

          // Get the gaussian weighted x and y responses
          gauss = gaussian(k*scale, l*scale, 3.3f*scale);  
          rx = gauss * haarX(sample_y, sample_x, 2*floor(scale));
          ry = gauss * haarY(sample_y, sample_x, 2*floor(scale));

		  if(_oriented){
			  // Get the gaussian weighted x and y responses on rotated axis
			  rrx = -rx*si + ry*co;
			  rry = rx*co + ry*si;
		  }
		  else{
			rrx=rx;
			rry=ry;
		  }

          dx += rrx;
          dy += rry;
          mdx += fabs(rrx);
          mdy += fabs(rry);
        }
      }

      // add the values to the descriptor vector
      desc[count++] = dx;
      desc[count++] = dy;
      desc[count++] = mdx;
      desc[count++] = mdy;

      // store the current length^2 of the vector
      len += dx*dx + dy*dy + mdx*mdx + mdy*mdy;
    }
  }

  // convert to unit vector
  len = sqrt(len);
  for(int i = 0; i < 64; i++)
    desc[i] /= len;
}



// ---- Utility functions ----
template <typename IntegralImageView>
float SurfDescriptor<IntegralImageView>::getAngleFromOrigin(float x, float y){
	if(x >= 0 && y >= 0)
    return atan(y/x);

  if(x < 0 && y >= 0)
    return pi - atan(-y/x);

  if(x < 0 && y < 0)
    return pi + atan(y/x);

  if(x >= 0 && y < 0)
    return 2*pi - atan(-y/x);

  return 0;
}

template <typename IntegralImageView>
float SurfDescriptor<IntegralImageView>::gaussian(int x, int y, float sig){
	return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

template <typename IntegralImageView>
float SurfDescriptor<IntegralImageView>::gaussian(float x, float y, float sig){
	return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

template <typename IntegralImageView>
float SurfDescriptor<IntegralImageView>::haarX(int row, int column, int s){
	return IntegralImageTransform::boxFilter(_img, row-s/2, column, s, s/2) - 
		IntegralImageTransform::boxFilter(_img,  row-s/2, column-s/2, s, s/2);
}

template <typename IntegralImageView>
float SurfDescriptor<IntegralImageView>::haarY(int row, int column, int s){
	return IntegralImageTransform::boxFilter(_img, row, column-s/2, s/2, s) -
		IntegralImageTransform::boxFilter(_img, row-s/2, column-s/2, s/2, s);
}