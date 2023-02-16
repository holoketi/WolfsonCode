#ifndef ___limits_h
#define ___limits_h

#include "graphics/sys.h"
#include "graphics/real.h"

namespace graphics {

	/*
	* Procedural fBm evaluated at (x,y,z); returns value stored in "value".
	*
	* Parameters:
	* 	"H" is the fractal increment
	* 	"lacunarity" is the gap between successive frequencies
	* 	"octaves" is the number of frequencies in the fBm
	*/
void    initNoise();
double	noise2(double x, double y, double z);
double	noise2(double x, double y, double z);
double	fBm(double x, double y, double z, double H, double lacunarity, double octaves);
double	turbulence(double x, double y, double z, double H, double lacunarity, double octaves);
}

#endif