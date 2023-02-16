#include "graphics/noise_api.h"
#include "graphics/src/noise_table.c"
#include "math.h"

namespace graphics {

	/* LIMITS ZOOM-IN CAPABILITY (should dump core if exceeded) */
#define MAX_OCTAVES	11

#define EGG_SHAPE
#define JITTERING

#define TRUE		1
#define FALSE		0
#define EPSILON		1e-6

#define Boolean	unsigned char

#define M_1_2PI		0.15915494309189533576
#define LOG2E           1.4426950408889634074
#define LOG2(x)         (LOG2E * log(x))		/* nat. log to log 2 */
#define DEGTORAD(x)	((x) *  0.01745329251994329576)	/* times pi/180 */
#define RADTODEG(x)	((x) * 57.29577951308232087684)	/* times 180/pi */
#define ABS(a)		((a)>=0 ? (a): -(a))
#define FABS(a)		((a)>=0. ? (a): -(a))

	/* vector normalization */
#define NORM(v)		{double length, norm;				\
			 length = sqrt(v->x*v->x+v->y*v->y+v->z*v->z);	\
			 norm = 1.0 / length;				\
			 v->x *= norm;					\
			 v->y *= norm;					\
			 v->z *= norm; }

	/* vector cross product */
#define CROSS(a,b,o)	(o)->x = (a.y * b.z) - (a.z * b.y);	\
			(o)->y = (a.z * b.x) - (a.x * b.z);	\
			(o)->z = (a.x * b.y) - (a.y * b.x);

	/* scalor multiplication of a 3-vector */
#define SMULT(s, v)	( (v).x*=(s), (v).y*=(s), (v).z*=(s), (v) )

	/* vector operations */
#define DOT(a,b)  	((a).x*(b).x + (a).y*(b).y + (a).z*(b).z)
#define VECADD(a,b,c)  	{(c)->x=(a).x+(b).x;	\
			 (c)->y=(a).y+(b).y;	\
			 (c)->z=(a).z+(b).z; }
#define VECSUB(a,b,c)  	{(c)->x = (a).x-(b).x;	\
			 (c)->y = (a).y-(b).y;	\
			 (c)->z = (a).z-(b).z; }
#define SCALARPROD(a,b,c)	\
			{(c)->x=(a)*(b).x;	\
			 (c)->y=(a)*(b).y;	\
			 (c)->z=(a)*(b).z; }
#define COLORSCALE(c1,s,c2)	\
			{(c2)->r=(s)*(c1).r;	\
			 (c2)->g=(s)*(c1).g;	\
			 (c2)->b=(s)*(c1).b; }
#define RAYPOS(ray,t,pos)	\
        		{(pos)->x = (ray)->origin.x + t*(ray)->dir.x;	\
			 (pos)->y = (ray)->origin.y + t*(ray)->dir.y;	\
			 (pos)->z = (ray)->origin.z + t*(ray)->dir.z; }


	typedef struct {	/* catch-all type for 3-D vectors and potisions */
		double x;
		double y;
		double z;
	} Vector;


	typedef struct {	/* unquantized illumination values */
		double r;
		double g;
		double b;
	} Color;

#define DELTA		0.001
#define DELTA_INV	1000.

#define B	0x100
#define BM	0xff

#define N	0x100000
#define NP	12   /* 2^N */
#define NM	0xfff

	static int p[B + B + 2];
	static double g[B + B + 2][3];

	/* cubic spline interpolation */
#define s_curve(t)	( t * t * (3. - 2. * t) )
	/* linear interpolation */
#define lerp(t, a, b)	( a + t * (b - a) )
#define at3(rx,ry,rz)	( rx * q[0] + ry * q[1] + rz * q[2] )
#define at2(rx,ry)	( rx * q[0] + ry * q[1] )
#define setup(u,b0,b1,r0,r1)\
	t = u + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;


	double
		Noise3(Vector vec)

	{
		int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
		double rx0, rx1, ry0, ry1, rz0, rz1, *q, sx, sy, sz, t;
		double a, b, c, d, u, v;
		register int i, j;

#ifdef UNLIMITED_NOISE_DOMAIN
		filter_args(&vec.x, &vec.y, &vec.z);
#endif

		setup(vec.x, bx0, bx1, rx0, rx1);
		setup(vec.y, by0, by1, ry0, ry1);
		setup(vec.z, bz0, bz1, rz0, rz1);

		i = p[bx0];
		j = p[bx1];

		b00 = p[i + by0];
		b10 = p[j + by0];
		b01 = p[i + by1];
		b11 = p[j + by1];

		sx = s_curve(rx0);
		sy = s_curve(ry0);
		sz = s_curve(rz0);

		q = g[b00 + bz0];	u = at3(rx0, ry0, rz0);
		q = g[b10 + bz0];	v = at3(rx1, ry0, rz0);
		a = lerp(sx, u, v);

		q = g[b01 + bz0];	u = at3(rx0, ry1, rz0);
		q = g[b11 + bz0];	v = at3(rx1, ry1, rz0);
		b = lerp(sx, u, v);

		c = lerp(sy, a, b);		/* interpolate in y at lo z */

		q = g[b00 + bz1];	u = at3(rx0, ry0, rz1);
		q = g[b10 + bz1];	v = at3(rx1, ry0, rz1);
		a = lerp(sx, u, v);

		q = g[b01 + bz1];	u = at3(rx0, ry1, rz1);
		q = g[b11 + bz1];	v = at3(rx1, ry1, rz1);
		b = lerp(sx, u, v);

		d = lerp(sy, a, b);		/* interpolate in y at hi z */

		return 1.5 * lerp(sz, c, d);	/* interpolate in z */
	} /* Noise3() */


	double
		Noise2(Vector vec)
	{
		int	bx0, bx1, by0, by1, b00, b10, b01, b11;
		double	rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v, result;
		int i, j;

#ifdef UNLIMITED_NOISE_DOMAIN
		filter_args(&vec.x, &vec.y, &vec.z);
#endif

		setup(vec.x, bx0, bx1, rx0, rx1);
		setup(vec.y, by0, by1, ry0, ry1);

		i = p[bx0];
		j = p[bx1];

		b00 = p[i + by0];
		b10 = p[j + by0];
		b01 = p[i + by1];
		b11 = p[j + by1];

		sx = s_curve(rx0);
		sy = s_curve(ry0);

		q = g[b00];		/* get random gradient */
		u = at2(rx0, ry0);	/* get weight on lo x side (lo y) */
		q = g[b10];
		v = at2(rx1, ry0);	/* get weight on hi x side (lo y) */
		a = lerp(sx, u, v);	/* get value at distance sx between u & v */

		/* similarly at hi y... */
		q = g[b01];	u = at2(rx0, ry1);
		q = g[b11];	v = at2(rx1, ry1);
		b = lerp(sx, u, v);

		result = 1.5 * lerp(sy, a, b);	/* interpolate in y */

		return (result);
	}  /* Noise2() */




	void
		Init_Noise()
	{
		int i;

		for (i = 0; i < B + B + 2; ++i)
		{
			p[i] = p_precomputed[i];
			g[i][0] = g_precomputed[i][0];
			g[i][1] = g_precomputed[i][1];
			g[i][2] = g_precomputed[i][2];
		}
	}

	/*
	* Procedural fBm evaluated at "point"; returns value stored in "value".
	*
	* Parameters:
	* 	"H" is the fractal increment
	* 	"lacunarity" is the gap between successive frequencies
	* 	"octaves" is the number of frequencies in the fBm
	*/
	double
		fBm(Vector point, double H, double lacunarity, double octaves)
	{
		double		value, frequency, remainder;
		static double   exponent_array[MAX_OCTAVES];
		static int	first = TRUE;
		int		i;

		/* precompute and store spectral weights */
		//if (first) {

			/* compute weight for each frequency */
			for (i = 0; i<MAX_OCTAVES; i++)
				exponent_array[i] = pow(lacunarity, -i*H);

			first = FALSE;
		//}

		value = 0.0;		/* initialize "value" */

		/* inner loop of spectral construction, where the fractal is built */
		for (i = 0; i<octaves; i++) {
			value += Noise3(point) * exponent_array[i];
			point.x *= lacunarity;
			point.y *= lacunarity;
			point.z *= lacunarity;
		} /* for */

		/* take care of remainder in "octaves" */
		remainder = octaves - (int)octaves;
		if (remainder)
			/* "i" and spatial freq. are preset in loop above */
			value += remainder * Noise3(point) * exponent_array[i];

		return(value);

	} /* fBm() */


	/*
	* Procedural "turbulence" function: fBm constructed from abs( Noise() )
	*
	* Parameters:
	* 	"H" is the fractal increment
	* 	"lacunarity" is the gap between successive frequencies
	* 	"octaves" is the number of frequencies in the fBm
	*/
	double
		turbulence(Vector point, double H, double lacunarity, double octaves)
	{
		double		temp, value, frequency, remainder;
		static double   exponent_array[MAX_OCTAVES];
		static int	first = TRUE;
		int		i;

		/* precompute and store spectral weights */
		//if (first) {

			/* compute weight for each frequency */
			for (i = 0; i<MAX_OCTAVES; i++)
				exponent_array[i] = pow(lacunarity, -i*H);

			first = FALSE;
		//}

		value = 0.0;		/* initialize "value" */

		/* inner loop of spectral construction, where the fractal is built */
		for (i = 0; i<octaves; i++) {
			temp = Noise3(point) * exponent_array[i];
			if (temp < 0.0)
				temp = -temp;
			value += temp;
			point.x *= lacunarity;
			point.y *= lacunarity;
			point.z *= lacunarity;
		} /* for */

		/* take care of remainder in "octaves" */
		remainder = octaves - (int)octaves;
		if (remainder) {
			/* "i" and spatial freq. are preset in loop above */
			temp = remainder * Noise3(point) * exponent_array[i];
			if (temp < 0.0)
				temp = -temp;
			value += temp;
		}

		return(value);

	} /* turbulence() */
	void    initNoise()
	{
		Init_Noise();
	}

	double	fBm(double x, double y, double z, double H, double lacunarity, double octaves)
	{
		Vector pnt = { x, y, z };
		return fBm(pnt, H, lacunarity, octaves);
	}
	double	turbulence(double x, double y, double z, double H, double lacunarity, double octaves)
	{
		Vector pnt = { x, y, z };
		return turbulence(pnt, H, lacunarity, octaves);
	}

	double noise2(double x, double y, double z) { Vector pnt = { x, y, z }; return Noise2(pnt); }

	double noise3(double x, double y, double z) { Vector pnt = { x, y, z }; return Noise3(pnt); }
}