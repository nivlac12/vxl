// This is mul/vil2/algo/vil2_grayscale_dilate.txx
#ifndef vil2_greyscale_dilate_txx_
#define vil2_greyscale_dilate_txx_
//:
//  \file
//  \brief Perform greyscale dilation on images
//  \author Tim Cootes

#include "vil2_greyscale_dilate.h"
#include <vcl_cassert.h>

//: Dilates src_image to produce dest_image (assumed single plane).
// dest_image(i0,j0) is the maximum value of the pixels under the
// structuring element when it is centred on src_image(i0,j0)
template <class T>
void vil2_greyscale_dilate(const vil2_image_view<T>& src_image,
                           vil2_image_view<T>& dest_image,
                           const vil2_structuring_element& element)
{
  assert(src_image.nplanes()==1);
  unsigned ni = src_image.ni();
  unsigned nj = src_image.nj();
  dest_image.set_size(ni,nj,1);

  int s_istep = src_image.istep(),  s_jstep = src_image.jstep();
  int d_istep = dest_image.istep(), d_jstep = dest_image.jstep();

  const T* src_row0 = src_image.top_left_ptr();
  T* dest_row0 = dest_image.top_left_ptr();

  vcl_vector<int> offset;
  vil2_compute_offsets(offset,element,s_istep,s_jstep);

  // Define box in which all element will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();

  // Deal with left edge
  for (int i=0;i<ilo;++i)
    for (unsigned int j=0;j<nj;++j)
      dest_image(i,j,0)=vil2_greyscale_dilate(src_image,0,element,i,j);
  // Deal with right edge
  for (unsigned int i=ihi+1;i<ni;++i)
    for (unsigned int j=0;j<nj;++j)
      dest_image(i,j,0)=vil2_greyscale_dilate(src_image,0,element,i,j);
  // Deal with bottom edge
  for (int i=ilo;i<=ihi;++i)
    for (int j=0;j<jlo;++j)
      dest_image(i,j,0)=vil2_greyscale_dilate(src_image,0,element,i,j);
  // Deal with top edge
  for (int i=ilo;i<=ihi;++i)
    for (unsigned int j=jhi+1;j<nj;++j)
      dest_image(i,j,0)=vil2_greyscale_dilate(src_image,0,element,i,j);

  for (int j=jlo;j<=jhi;++j)
  {
    const T* src_p = src_row0 + j*s_jstep + ilo*s_istep;
    T* dest_p = dest_row0 + j*d_jstep + ilo * d_istep;

    for (int i=ilo;i<=ihi;++i,src_p+=s_istep,dest_p+=d_istep)
      *dest_p=vil2_greyscale_dilate(src_p,&offset[0],offset.size());
  }
}

#undef VIL2_GREYSCALE_DILATE_INSTANTIATE
#define VIL2_GREYSCALE_DILATE_INSTANTIATE(T) \
template void vil2_greyscale_dilate(const vil2_image_view< T >& src_image, \
                                    vil2_image_view< T >& dest_image, \
                                    const vil2_structuring_element& element)

#endif // vil2_greyscale_dilate_txx_
