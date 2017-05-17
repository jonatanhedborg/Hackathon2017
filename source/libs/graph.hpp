/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

graph.hpp - v0. 1 - Util lib for basic graphics drawing, for C++.

*/

#ifndef graph_hpp
#define graph_hpp

namespace graph_ns {
	
template< typename SCREEN_TYPE, typename COLOR_TYPE > 
struct graph
	{
	graph();
    graph( SCREEN_TYPE* scr );

	void pset( int x, int y, COLOR_TYPE color );
	COLOR_TYPE pget( int x, int y );

	void hline( int x, int y, int len, COLOR_TYPE color );
	void vline( int x, int y, int len, COLOR_TYPE color );

	void line( int x1, int y1, int x2, int y2, COLOR_TYPE color );

	void box( int x, int y, int w, int h, COLOR_TYPE color );
	void box_fill( int x, int y, int w, int h, COLOR_TYPE color );

	void circle( int x, int y, int r, COLOR_TYPE color );
	void circle_fill( int x, int y, int r, COLOR_TYPE color );

	void ellipse( int x, int y, int rx, int ry, COLOR_TYPE color );
	void ellipse_fill( int x, int y, int rx, int ry, COLOR_TYPE color );

	// arc
    
    void polygon( int* points_xy, int count, COLOR_TYPE color );
	template< int MAX_POLYGON_POINTS > void polygon_fill( int* points_xy, int count, COLOR_TYPE color );
	
	void fill( int x, int y, COLOR_TYPE color );


	SCREEN_TYPE* screen;
	};


} /* namespace graph_ns */

#endif /* graph_hpp */


/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifndef graph_impl
#define graph_impl


namespace graph_ns {


template< typename SCREEN_TYPE, typename COLOR_TYPE >
graph<SCREEN_TYPE, COLOR_TYPE>::graph() : screen(0)
	{ }


template< typename SCREEN_TYPE, typename COLOR_TYPE >
graph<SCREEN_TYPE, COLOR_TYPE>::graph( SCREEN_TYPE* scr ) : screen( scr )
	{ }


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::pset( int x, int y, COLOR_TYPE color )
	{
	screen->pset( x, y, color );
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
COLOR_TYPE graph<SCREEN_TYPE, COLOR_TYPE>::pget( int x, int y )
	{
	return screen->pget( x, y );
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::hline( int x, int y, int len, COLOR_TYPE color )
	{
	screen->hline( x, y, len, color );
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::vline( int x, int y, int len, COLOR_TYPE color )
	{
	for( int i = y; i < y + len; ++i )
		pset( x, i, color );
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::line( int x1, int y1, int x2, int y2, COLOR_TYPE color )
	{
	// TODO remove abs
	int dx = ::abs( x2 - x1 );
	int sx = x1 < x2 ? 1 : -1;
	int dy = ::abs( y2 - y1 );
	int sy = y1 < y2 ? 1 : -1; 
	int err = ( dx > dy ? dx : -dy ) / 2;
	 
	int x = x1;
	int y = y1;
	while( x != x2 || y != y2 )
		{
		pset( x, y, color );
		
		int e2 = err;
		if( e2 > -dx ) { err -= dy; x += sx; }
		if( e2 < dy ) { err += dx; y += sy; }
		}
	pset( x, y, color );
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::box( int x, int y, int w, int h, COLOR_TYPE color )
	{
	hline( x, y, w, color );
	hline( x, y + h - 1, w, color );
	vline( x, y, h, color );
	vline( x + w - 1, y, h, color );
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::box_fill( int x, int y, int w, int h, COLOR_TYPE color )
	{
	for( int i = y; i < y + h; ++i )
		hline( x, i, w, color );
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::circle( int x, int y, int r, COLOR_TYPE color )
	{       
	int f = 1 - r;
	int dx = 0;
	int dy = -2 * r;
	int ix = 0;
	int iy = r;
 
	pset( x, y + r, color );
	pset( x, y - r, color );
	pset( x + r, y, color );
	pset( x - r, y, color );
 
	while( ix < iy ) 
		{
		if( f >= 0 ) 
			{
			--iy;
			dy += 2;
			f += dy;
			}
		++ix;
		dx += 2;
		f += dx + 1;    

		pset( x + ix, y + iy, color );
		pset( x - ix, y + iy, color );
		pset( x + ix, y - iy, color );
		pset( x - ix, y - iy, color );
		pset( x + iy, y + ix, color );
		pset( x - iy, y + ix, color );
		pset( x + iy, y - ix, color );
		pset( x - iy, y - ix, color );
		}
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::circle_fill( int x, int y, int r, COLOR_TYPE color )
	{       
	int f = 1 - r;
	int dx = 0;
	int dy = -2 * r;
	int ix = 0;
	int iy = r;
 
	while( ix <= iy ) 
		{
		hline( x - iy, y + ix, 2 * iy, color );
		hline( x - iy, y - ix, 2 * iy, color );
		if( f >= 0 ) 
			{
			hline( x - ix, y + iy, 2 * ix, color );
			hline( x - ix, y - iy, 2 * ix, color );

            --iy;
			dy += 2;
			f += dy;
			}
        ++ix;
		dx += 2;
		f += dx + 1;    
		}
	}
	

template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::ellipse( int x, int y, int rx, int ry, COLOR_TYPE color )
	{
	int asq = rx * rx;
	int bsq = ry * ry;

	pset( x, y + ry, color );
	pset( x, y - ry, color );

	int wx = 0;
	int wy = ry;
	int xa = 0;
	int ya = asq * 2 * ry;
	int thresh = asq / 4 - asq * ry;

	for( ; ; )
		{
		thresh += xa + bsq;

		if (thresh >= 0) 
			{
			ya -= asq * 2;
			thresh -= ya;
			--wy;
			}

		xa += bsq * 2;
		++wx;

		if (xa >= ya) break;

		pset( x + wx, y - wy, color );
		pset( x - wx, y - wy, color );
		pset( x + wx, y + wy, color );
		pset( x - wx, y + wy, color );
		}

	pset( x + rx, y, color );
	pset( x - rx, y, color );

	wx = rx;
	wy = 0;
	xa = bsq * 2 * rx;

	ya = 0;
	thresh = bsq / 4 - bsq * rx;

	for( ; ; )
		{
		thresh += ya + asq;

		if (thresh >= 0) 
			{
			xa -= bsq * 2;
			thresh = thresh - xa;
			--wx;
			}

		ya += asq * 2;
		++wy;

		if (ya > xa) break;

		pset( x + wx, y - wy, color );
		pset( x - wx, y - wy, color );
		pset( x + wx, y + wy, color );
		pset( x - wx, y + wy, color );
		}
	}


template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::ellipse_fill( int x, int y, int rx, int ry, COLOR_TYPE color )
	{
	int asq = rx * rx;
	int bsq = ry * ry;

	int wx = 0;
	int wy = ry;
	int xa = 0;
	int ya = asq * 2 * ry;
	int thresh = asq / 4 - asq * ry;

	for( ; ; )
		{
		thresh += xa + bsq;

		if (thresh >= 0) 
			{
			ya -= asq * 2;
			thresh -= ya;
			hline( x - wx, y - wy, wx * 2, color );
			hline( x - wx, y + wy, wx * 2, color );
			--wy;
			}

		xa += bsq * 2;
		++wx;
		if (xa >= ya) break;
		}

	hline( x - rx, y, rx * 2, color );

	wx = rx;
	wy = 0;
	xa = bsq * 2 * rx;

	ya = 0;
	thresh = bsq / 4 - bsq * rx;

	for( ; ; )
		{
		thresh += ya + asq;

		if (thresh >= 0) 
			{
			xa -= bsq * 2;
			thresh = thresh - xa;
			--wx;
			}

		ya += asq * 2;
		++wy;

		if (ya > xa) break;

		hline( x - wx, y - wy, wx * 2, color );
		hline( x - wx, y + wy, wx * 2, color );
		}
	}

// arc
    
template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::polygon( int* points_xy, int count, COLOR_TYPE color )
	{
	for( int i = 0; i < count - 1; ++i )
		line( points_xy[ i * 2 + 0 ], points_xy[ i * 2 + 1 ], 
			points_xy[ ( i + 1 ) * 2 + 0 ], points_xy[ ( i + 1 ) * 2 + 1 ], color );
	}



template< typename SCREEN_TYPE, typename COLOR_TYPE >
template< int MAX_POLYGON_POINTS >
void graph<SCREEN_TYPE, COLOR_TYPE>::polygon_fill( int* points_xy, int count, COLOR_TYPE color )
	{
	static int node_x[ MAX_POLYGON_POINTS ];

	if( count <= 0 || count > MAX_POLYGON_POINTS ) return;

	int min_y = points_xy[ 0 + 1 ];
	int max_y = min_y;

	// find extents
	for( int i = 1; i < count; ++i )
		{
		if( points_xy[ i * 2 + 1 ] < min_y ) min_y = points_xy[ i * 2 + 1 ];
		if( points_xy[ i * 2 + 1 ] > max_y ) max_y = points_xy[ i * 2 + 1 ];
		}

	for( int y = min_y; y < max_y; ++y ) 
		{
		//  find intersection points_xy
		int nodes = 0; 
		int j = count - 1;
		for( int i = 0; i < count; ++i ) 
			{
			if( ( points_xy[ i * 2 + 1 ] <= y && points_xy[ j * 2 + 1 ] > y ) || 
				( points_xy[ j * 2 + 1 ] <= y && points_xy[ i * 2 + 1 ] > y ) )
				{
				int dx = points_xy[ j * 2 + 0 ] - points_xy[ i * 2 + 0 ];
				int dy = points_xy[ j * 2 + 1 ] - points_xy[ i * 2 + 1 ];
				node_x[ nodes++ ] = points_xy[ i * 2 + 0 ] + ( ( y - points_xy[ i * 2 + 1 ] ) * dx ) / dy ; 
				}
			j = i; 
			}

		// sort by x
		int xi = 0;
		while( xi < nodes - 1 ) 
			{
			if( node_x[ xi ] > node_x[ xi + 1 ] ) 
				{
				int swap = node_x[ xi ]; 
				node_x[ xi ] = node_x[ xi + 1 ]; 
				node_x[ xi + 1 ] = swap; 
				if( xi ) --xi; 
				}
			else 
				{
				++xi; 
				}
			}

		for( int i = 0; i < nodes; i += 2 ) 
			hline( node_x[ i ], y, node_x[ i + 1 ] - node_x[ i ], color );
		}
	}


/*
 * A Seed Fill Algorithm
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 *
 * user provides pixelread() and pixelwrite() routines
 *
 * fill.c : simple seed fill program
 * Calls pixelread() to read pixels, pixelwrite() to write pixels.
 *
 * Paul Heckbert	13 Sept 1982, 28 Jan 1987
 *
 * fill: set the pixel at (x,y) and all of its 4-connected neighbors
 * with the same pixel value to the new pixel value nv.
 * A 4-connected neighbor is a pixel above, below, left, or right of a pixel.
 */

template< typename SCREEN_TYPE, typename COLOR_TYPE >
void graph<SCREEN_TYPE, COLOR_TYPE>::fill( int x, int y, COLOR_TYPE color )
	{
	#define MAX 10000		/* max depth of stack */

	#define PUSH(Y, XL, XR, DY)	/* push new segment on stack */ \
		if (sp<stack+MAX && Y+(DY)>=0 && Y+(DY)<screen->height) \
		{sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; ++sp;}

	#define POP(Y, XL, XR, DY)	/* pop segment off stack */ \
		{--sp; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}

	/*
	 * Filled horizontal segment of scanline y for xl<=x<=xr.
	 * Parent segment was on line y-dy.  dy=1 or -1
	 */
	struct Segment { int y, xl, xr, dy; };

	int l, x1, x2, dy;
	COLOR_TYPE ov;	/* old pixel value */
	Segment stack[MAX], *sp = stack;	/* stack of filled segments */

	ov = pget(x, y);		/* read pv at seed point */
	if (ov==color|| x<0 || x>=screen->width || y<0 || y>=screen->height) return;
	PUSH(y, x, x, 1);			/* needed in some cases */
	PUSH(y+1, x, x, -1);		/* seed segment (popped 1st) */

	while (sp>stack) 
		{
		/* pop segment off stack and fill a neighboring scan line */
		POP(y, x1, x2, dy);
		/*
		 * segment of scan line y-dy for x1<=x<=x2 was previously filled,
		 * now explore adjacent pixels in scan line y
		 */
		for (x=x1; x>=0 && pget(x, y)==ov; --x) /* nothing */;
		hline( x + 1, y, x1 - x, color );
		if (x>=x1) goto skip;
		l = x+1;
		if (l<x1) PUSH(y, l, x1-1, -dy);		/* leak on left? */
		x = x1+1;
		do {
			int xs = x;
			for (; x<screen->width && pget(x, y)==ov; ++x) /* nothing */;
			hline( xs, y, x - xs, color );	
			PUSH(y, l, x-1, dy);
			if (x>x2+1) PUSH(y, x2+1, x-1, -dy);	/* leak on right? */
		skip:
			for (x++; x<=x2 && pget(x, y)!=ov; ++x);
			l = x;
			} while (x<=x2);
		}
	}


} /* namespace graph_ns */

#endif /* graph_impl */

  /*
  ------------------------------------------------------------------------------

  This software is available under 2 licenses - you may choose the one you like.

  ------------------------------------------------------------------------------

  ALTERNATIVE A - MIT License

  Copyright (c) 2015 Mattias Gustavsson

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ------------------------------------------------------------------------------

  ALTERNATIVE B - Public Domain (www.unlicense.org)

  This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
  software, either in source code form or as a compiled binary, for any purpose,
  commercial or non-commercial, and by any means.

  In jurisdictions that recognize copyright laws, the author or authors of this
  software dedicate any and all copyright interest in the software to the public
  domain. We make this dedication for the benefit of the public at large and to
  the detriment of our heirs and successors. We intend this dedication to be an
  overt act of relinquishment in perpetuity of all present and future rights to
  this software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  ------------------------------------------------------------------------------
  */
