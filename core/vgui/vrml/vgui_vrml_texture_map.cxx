//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_vrml_texture_map
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 28 Sep 99
//
//-----------------------------------------------------------------------------

#include "vgui_vrml_texture_map.h"
#include <vbl/vbl_file.h>
#include <vil/vil_image.h>
#include <vil/vil_file_image.h>
#include <vil/vil_memory_image_of.h>
#include <vbl/vbl_printf.h>
#include <vcl_iostream.h>

int VrmlDraw_TEX_MAX = 256;

vcl_string vgui_vrml_texture_map::vrml_dirname("");

// Scale to next higher powers-of-2
static inline int texmap_dimension(int in)
{
  int MAX_SIZE = VrmlDraw_TEX_MAX;
  for(int w = 1; w < MAX_SIZE; w *= 2)
    if (w >= in)
      return w;
  return MAX_SIZE;
}




vgui_vrml_texture_map* vgui_vrml_texture_map::create(char const* filename)
{
  if (filename == 0 || *filename == 0)
    return 0;

  //if (gl_mode != MWinGLMode::textured)
  //  return 0;

  // Try some directory hilarity if file not found
  vcl_string fn;
  if (vbl_file::exists(filename))
    fn = filename;
  else if (vrml_dirname.size() > 0) {
    fn = vrml_dirname; fn += "/"; fn += filename;
    if (!vbl_file::exists(fn.c_str())) {
      vbl_printf(vcl_cerr, "Can't find texture [tried %s and %s]\n", filename, fn.c_str());
      return 0;
    }
  }
  filename = fn.c_str();

  vbl_printf(vcl_cerr, "Loading texture from [%s]... ", filename);
  vil_file_image fileimage(filename, vil_file_image::laconic);
  if (!fileimage) {
    vcl_cerr << "Failed!";
    return 0;
  }
  
  int w = fileimage.width();
  int h = fileimage.height();

  int tex_w = texmap_dimension(w);
  int tex_h = texmap_dimension(h);
  
  bool need_rescale = (tex_w != w) || (tex_h != h);
  if (need_rescale)
    vbl_printf(vcl_cerr, "Rescale from %dx%d to %dx%d, ", w, h, tex_w, tex_h);

  // Rescale and flip Y
  if (vil_pixel_format(fileimage) == VIL_RGB_BYTE) {
    vgui_vrml_texture_map* newmap = new vgui_vrml_texture_map(filename, tex_w, tex_h);
    vil_memory_image_of<vil_rgb<unsigned char> > rgb(fileimage.width(), fileimage.height()); 
    fileimage.get_section(rgb.get_buffer(), 0,0, fileimage.width(), fileimage.height());
    for(int y = 0; y < tex_h; ++y) {
      int orig_y = y * h / tex_h;
      for(int x = 0; x < tex_w; ++x)
	newmap->rgb(x,tex_h - y - 1) = rgb(x * w / tex_w, orig_y);
    }
    vcl_cerr << "Done.\n";
    return newmap;
  } else if (vil_pixel_format(fileimage) == VIL_BYTE) {
    vgui_vrml_texture_map* newmap = new vgui_vrml_texture_map(filename, tex_w, tex_h);
    //vil_memory_image_of<byte> gray( fileimage.get_image_ptr() ); //im8);
    vil_memory_image_of<vil_byte> gray(fileimage.width(), fileimage.height()); 
    fileimage.get_section(gray.get_buffer(), 0,0, fileimage.width(), fileimage.height());
    for(int y = 0; y < tex_h; ++y) {
      int orig_y = y * h / tex_h;
      for(int x = 0; x < tex_w; ++x) {
	int v = gray(x * w / tex_w, orig_y);
	newmap->rgb(x,tex_h - y - 1).r = v;
	newmap->rgb(x,tex_h - y - 1).g = v;
	newmap->rgb(x,tex_h - y - 1).b = v;
      }
    }
    vcl_cerr << "Done.\n";
    return newmap;
  }
  
  vbl_printf(vcl_cerr, " ignoring %d b x %d comp image\n", fileimage.bits_per_component(), fileimage.components() );
  return 0;
}

