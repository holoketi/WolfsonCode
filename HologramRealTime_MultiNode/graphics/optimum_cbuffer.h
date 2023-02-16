// optimum_cbuffer.h: interface for the optimum_cbuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	    __optimum_cbuffer_h
#define	    __optimum_cbuffer_h

#include "color_buffer.h"

namespace graphics {

class Camera;

class optimum_cbuffer : public color_buffer  
{
    
public:
    optimum_cbuffer();
    virtual ~optimum_cbuffer();

	void resize() 
		{ resize(base_size, base_size); }

    virtual void resize(int w, int h);

	virtual void get_objects(int x, int y, int w, int h, std::set<uint>& ret) const;
    
	virtual void begin_draw() const;
    
	virtual void end_draw();


protected:
    virtual void read_in();

private:
    inline uint read(int x, int y) const;
    void    optimum_size(int cw, int ch, int& w, int& h) const;

    mutable int	save_width, save_height;    // to save original viewport

    int	    o_width, o_height;
    int	    base_size;  // base_size x base_size
    real    ratio;	// o_width/width
};

inline uint optimum_cbuffer::read(int x, int y) const
{
    #ifdef GL_RADEON
	return buffer[(y%o_height)*o_width + (x%o_width)];
    #else
	uint ret = buffer[(y%o_height)*o_width + (x%o_width)];
	return (((ret&0xFF000000)>>24)|((ret&0x00FF0000)>>8)|((ret&0x0000FF00)<<8)|((ret&0x000000FF)<<24));
    #endif
}
};  // namespace graphics
#endif
