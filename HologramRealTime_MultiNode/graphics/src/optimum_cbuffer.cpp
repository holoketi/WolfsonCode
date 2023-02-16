// optimum_cbuffer.cpp: implementation of the optimum_cbuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "graphics/optimum_cbuffer.h"
#include <set>
#include "graphics/Camera.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace graphics {

optimum_cbuffer::optimum_cbuffer() : base_size(600)
{
}

optimum_cbuffer::~optimum_cbuffer()
{
}

void
optimum_cbuffer::optimum_size(int cw, int ch, int& w, int& h) const
{
    real aspect_ratio = ((real)cw)/((real)ch);
    int t_base_size;
    
    if (aspect_ratio >= 1.5 && ch < 300)
	t_base_size = 0.5 * ch;
    else
	t_base_size = base_size;

    w = aspect_ratio * t_base_size;
    h = t_base_size;
}

void
optimum_cbuffer::resize(int w, int h)
{

    if (w < 0 || h < 0) return;
	//color_buffer::resize(w, h);
	o_width = w;
	o_height = h;

 //   if (h < base_size && (real)w/h < 1.5) {
	//color_buffer::resize(w, h);
	//o_width = w;
	//o_height = h;
	//ratio = 1.0;
 //   
	//return;
 //   }

 //   set_need_update();

 //   int rw, rh;
 //   optimum_size(w, h, rw, rh);
 //   
 //   if (((rw * rh) >= get_max_width() * get_max_height()) && 
	//((rw * rh) > (o_width * o_height)))
 //   {
	//new_buffer(rw * rh);
 //   }

 //   o_width = rw;
 //   o_height = rh;

 //   set_width(w);
 //   set_height(h);

 //   ratio = ((real)o_width)/((real)get_width());
}

void
optimum_cbuffer::get_objects(int x, int y, int w, int h, std::set<uint> &a_set) const
{

	color_buffer::get_objects(x, y, w, h, a_set);
	
	//a_set.clear();
 //   
	//x = (int)(x*ratio);
 //   y = (int)(y*ratio);
	//

	//if ( x < 0 || x > o_width-1) return;
	//if ( y < 0 || y > o_height-1) return;

 //   if (w > 1) w = (int)(w*ratio) + 1;
 //   if (h > 1) h = (int)(h*ratio) + 1;
 //   if (w < 1) w = 1;
 //   if (h < 1) h = 1;


 //   int sy = ((y-h) > 0) ? (y-h) : 0;
 //   int ey = ((y+h) > o_height-1) ? o_height-1 : (y+h);
 //   int sx = ((x-w) > 0) ? (x-w) : 0;
 //   int ex = ((x+w) > o_width-1) ? o_width-1 : (x+w);


 //   for (int i = sy ; i <= ey ;++i){
	//for (int j = sx ; j <= ex ; ++j) {
	//    uint val = read(j, i);
	//    if (val) a_set.insert(val);
	//}
 //   }
}

void
optimum_cbuffer::read_in()
{
    glReadBuffer(GL_BACK);

    // 0x8000 : GL_ABGR_EXT (only with RADEON)
    #ifdef GL_RADEON
	glReadPixels(0, 0, o_width, o_height, 0x8000, GL_UNSIGNED_BYTE, (GLvoid*)get_buffer());
    #else
	glReadPixels(0, 0, o_width, o_height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)get_buffer());
    #endif
}

void
optimum_cbuffer::begin_draw() const
{
	color_buffer::begin_draw();
    //GLint viewport[4];
    //glGetIntegerv(GL_VIEWPORT, viewport);
    //save_width = viewport[2];
    //save_height = viewport[3];

    //glViewport(0, 0, o_width, o_height);
    //if (vp) vp->set_view((real) o_width/o_height);

    //glDepthFunc(GL_LEQUAL);

    //glDisable(GL_LINE_SMOOTH);
    //glDisable(GL_BLEND);

    //glDisable(GL_ALPHA_TEST);
    //glDisable(GL_COLOR_MATERIAL);
    //glDisable(GL_DITHER);

    //glDisable(GL_LIGHTING);
    //glClearColor(0.0, 0.0, 0.0, 0.0);
    //glDrawBuffer(GL_BACK);
    //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glPushMatrix();
}

void
optimum_cbuffer::end_draw()
{
	color_buffer::end_draw();
    // Radeon Crashes here
    //glPopMatrix();
    //read_in();
    //set_need_update(false);
    //glDepthFunc(GL_LESS);
    //glViewport(0, 0, save_width, save_height);
    //if (vp) vp->set_view((real) o_width/o_height);
    //glMatrixMode(GL_MODELVIEW);
}

}; // namespace graphics