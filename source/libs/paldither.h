/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

paldither.h - v0.1 - Convert true-color image to custom palette, with dither.

Do this:
	#define PALDITHER_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef paldither_h
#define paldither_h

#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#include <stddef.h>

#ifndef PALDITHER_U32
	#define PALDITHER_U32 unsigned int
#endif

#ifndef PALDITHER_U8
	#define PALDITHER_U8 unsigned char
#endif

typedef struct paldither_palette_t paldither_palette_t;

paldither_palette_t* paldither_palette_create( PALDITHER_U32 const* xbgr, int count, size_t* palette_size );
paldither_palette_t* paldither_palette_create_from_data( void const* data, size_t size );

void paldither_palette_destroy( paldither_palette_t* palette );

void paldither_palettize( PALDITHER_U32* abgr, int width, int height, paldither_palette_t const* palette, PALDITHER_U8* output );

#endif /* paldither_h */


/*
----------------------
	IMPLEMENTATION
----------------------
*/


#ifdef PALDITHER_IMPLEMENTATION
#undef PALDITHER_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>


struct paldither_mix_t
	{
	int first;
	int second;
	int d;
	unsigned char ratio;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char l;
	};

struct paldither_palette_t
	{
	int color_count;
	PALDITHER_U32 colortable[ 256 ];
	int mix_count;
	};

	
paldither_palette_t* paldither_palette_create( PALDITHER_U32 const* xbgr, int count, size_t* palette_size )
	{	
    const int mix_levels = 11;

    int mix_count = ( count * ( count + 1 ) ) / 2 * 11;
    paldither_mix_t* mix = (paldither_mix_t*) malloc( mix_count * sizeof( paldither_mix_t ) );

    int c = 0;
	for( int i = 0; i < count; ++i )
		{
        PALDITHER_U32 fcolor = xbgr[ i ] & 0x00ffffff;
        int fr = (int)( ( fcolor & 0x000000ff ) );
        int fg = (int)( ( fcolor & 0x0000ff00 ) >> 8 );
        int fb = (int)( ( fcolor & 0x00ff0000 ) >> 16 );                
        int fl = (int)( ( 54 * fr + 183 * fg + 19 * fb + 127 ) >> 8 );
        assert( fl <= 0xff && fl >= 0 );                   
		for( int j = i; j < count; ++j )	
			{
            PALDITHER_U32 scolor = xbgr[ j ] & 0x00ffffff;
            int sr = (int)( ( scolor & 0x000000ff ) );
            int sg = (int)( ( scolor & 0x0000ff00 ) >> 8 );
            int sb = (int)( ( scolor & 0x00ff0000 ) >> 16 );                
            int sl = (int)( ( 54 * sr + 183 * sg + 19 * sb + 127 ) >> 8 );
            assert( sl <= 0xff && sl >= 0 );                   

            int dr = fr - sr;
            int dg = fg - sg;
            int db = fb - sb;
            int dl = fl - sl;
            int d = ( ( ( ( dr*dr + dg*dg + db*db ) >> 1 ) + dl * dl ) * 38 + 127 ) >> 8;
       
            for( int k = 0; k < mix_levels; ++k )	
				{
                int r = fr;
                int g = fg;
                int b = fb;
                int l = fl;
                if( i != j )
                    {
                    int s = mix_levels - 1;
                    int ik = s - k;                    
                    r = ( r * k + sr * ik + 5 ) / s;
                    g = ( g * k + sg * ik + 5  ) / s;
                    b = ( b * k + sb * ik + 5  ) / s;
                    l = ( 54 * r + 183 * g + 19 * b  + 127 ) >> 8;
                    assert( r <= 0xff && g <= 0xff && b <= 0xff && r >= 0 && g >= 0 && b >= 0 && l <= 0xff && l >= 0 );                   
                    }
                mix[ c ].first = i;
                mix[ c ].second = j;
                mix[ c ].ratio = (unsigned char) k;
    			mix[ c ].r = (unsigned char) r;
    			mix[ c ].g = (unsigned char) g;
     			mix[ c ].b = (unsigned char) b;
                mix[ c ].l = (unsigned char) l;
                mix[ c ].d = d;
                ++c;
                }
			}
		}
	
	int cube[ 17 * 17 * 17 ];	
	for( int r = 0; r < 17; ++r )
		{
		for( int g = 0; g < 17; ++g )
			{
			for( int b = 0; b < 17; ++b )
				{
				int best_diff = 0x7FFFFFFF;
				int best_mix = 0;
				paldither_mix_t const* m = mix;
				int l = ( 54 * (r * 16) + 183 * (g * 16) + 19 * (b * 16) + 127 ) >> 8;
				for( int i = 0; i < mix_count; ++i, ++m )
					{
					int dr = r * 16 - m->r;
					int dg = g * 16 - m->g;
					int db = b * 16 - m->b;
					int dl = l - m->l;
					int d = ( ( ( dr*dr + dg*dg + db*db ) >> 1 ) + dl*dl ) + m->d;
					if( d < best_diff ) { best_diff = d; best_mix = i; }
					}
				cube[ r * 17 * 17 + g * 17 + b ] = best_mix;
				}
			}
		}

	int map[ 16 * 16 * 16 ];
	int list_capacity = mix_count * 256;
	int* list = (int*) malloc( list_capacity * sizeof( int ) );
	int freelist = 0;
	
	int* mapptr = map;
	for( int r = 0; r < 16; ++r )
		{
		for( int g = 0; g < 16; ++g )
			{
			for( int b = 0; b < 16; ++b )
				{
				paldither_mix_t const* m = mix;
				*mapptr++ = freelist;
				if( freelist == list_capacity ) { list_capacity *= 2; list = (int*)realloc( list, list_capacity * sizeof( int ) ); }
				int* count_ptr = &list[ freelist++ ];
				*count_ptr = 0;
				for( int i = 0; i < mix_count; ++i, ++m )
					{
					paldither_mix_t const* best_mix = &mix[ cube[ ( r ) * 17 * 17 + ( g ) * 17 + ( b ) ] ];
					paldither_mix_t const* best_mix2 = &mix[ cube[ ( r + 1 ) * 17 * 17 + ( g + 1 ) * 17 + ( b + 1 ) ] ];
					bool pass = 				
					    ( m->r >= best_mix->r && m->r <= best_mix2->r )
					 && ( m->g >= best_mix->g && m->g <= best_mix2->g )
					 && ( m->b >= best_mix->b && m->b <= best_mix2->b );
					if( pass ) 
						{
						(*count_ptr)++;
						if( freelist == list_capacity ) { list_capacity *= 2; list = (int*)realloc( list, list_capacity * sizeof( int ) ); }
						list[ freelist++ ] = i;
						}
					
					}
				}
			}
		}

	size_t size = sizeof( paldither_palette_t ) + freelist * sizeof( int ) + mix_count * sizeof( paldither_mix_t ) + sizeof( map );	
	paldither_palette_t* palette = (paldither_palette_t*) malloc( size );
	
	palette->color_count = count;
	memcpy( palette->colortable, xbgr, sizeof( *xbgr ) * count );
	
	palette->mix_count = mix_count;
	uintptr_t dest = (uintptr_t)( palette + 1) ;

	memcpy( (void*)dest, mix, mix_count * sizeof( paldither_mix_t ) );
	dest += mix_count * sizeof( paldither_mix_t );

	memcpy( (void*)dest, map, sizeof( map ) );
	dest += sizeof( map );

	memcpy( (void*)dest, list, freelist * sizeof( int ) );

	free( list );
	free( mix );
	
	if( palette_size ) *palette_size = size;
	return palette;
	}

	
paldither_palette_t* paldither_palette_create_from_data( void const* data, size_t size )
	{
	paldither_palette_t* palette = (paldither_palette_t*)malloc( size );
	memcpy( palette, data, size );
	return palette;
	}


void paldither_palette_destroy( paldither_palette_t* palette )
	{
	free( palette );
	}

	
void paldither_palettize( PALDITHER_U32* abgr, int width, int height, paldither_palette_t const* palette, PALDITHER_U8* output )
    {
    unsigned char dither_pattern[ 4 * 4 * 11 ] = 
	    {
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,

	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,

	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,
	    0,0,0,0,

	    0,0,0,0,
	    0,0,0,1,
	    0,1,0,0,
	    0,0,0,1,

	    1,0,1,0,
	    0,1,0,0,
	    1,0,1,0,
	    0,0,0,1,

	    1,0,1,0,
	    0,1,0,1,
	    1,0,1,0,
	    0,1,0,1,

	    1,0,1,0,
	    1,1,0,1,
	    1,0,1,0,
	    0,1,1,1,

	    1,1,0,1,
	    0,1,1,1,
	    1,1,1,1,
	    1,1,1,1,

	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,

	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,

	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,
	    1,1,1,1,
	    };
		
	uintptr_t ptr = (uintptr_t)( palette + 1 );
	paldither_mix_t const* pal_mix = (paldither_mix_t*) ptr; ptr += palette->mix_count * sizeof( paldither_mix_t );
	int const* pal_map = (int*) ptr; ptr += 16 * 16 * 16 * sizeof( int );
	int const* pal_list = (int*) ptr;

	for( int y = 0; y < height; ++y )
		{
		for( int x = 0; x < width; ++x )	
			{
            PALDITHER_U32 color = abgr[ x + y * width ];
            int r = (int)( ( color & 0x000000ff ) );
            int g = (int)( ( color & 0x0000ff00 ) >> 8 );
            int b = (int)( ( color & 0x00ff0000 ) >> 16 );                
            int l = (int)( ( 54 * r + 183 * g + 19 * b + 127 ) >> 8 );
            assert( l <= 0xff && l >= 0 );           

			paldither_mix_t const* best_mix = 0;
			int pal_index = pal_map[ ( r >> 4 ) * 16 * 16 + ( g >> 4 ) * 16 + ( b >> 4 ) ];
			int count = pal_list[ pal_index++ ];
			if( count != 0 )
				{
				int best_diff = 0x7FFFFFFF;
				int const* index = &pal_list[ pal_index ];
				for( int i = 0; i < count; ++i, ++index )
					{
					paldither_mix_t const* m = &pal_mix[ *index ];
					int dr = r - m->r;
					int dg = g - m->g;
					int db = b - m->b;
					int dl = l - m->l;
					int d = ( ( ( dr*dr + dg*dg + db*db ) >> 1 ) + dl*dl ) + m->d;
					if( d < best_diff ) { best_diff = d; best_mix = m; }
					}
				}
			else
				{
				int best_diff = 0x7FFFFFFF;
				paldither_mix_t const* m = pal_mix;
				for( int i = 0; i < palette->mix_count; ++i, ++m )
					{
					int dr = r - m->r;
					int dg = g - m->g;
					int db = b - m->b;
					int dl = l - m->l;
					int d = ( ( ( dr*dr + dg*dg + db*db ) >> 1 ) + dl*dl ) + m->d;
					if( d < best_diff ) { best_diff = d; best_mix = m; }
					}
				}

			int index;
            if( dither_pattern[ best_mix->ratio * 4 * 4 + ( x & 3 ) + ( y & 3 ) * 4 ] == 1 )			
				index = best_mix->first;
            else
			    index = best_mix->second;
				
            if( output) output[ x + y * width ] = (PALDITHER_U8) index;
			abgr[ x + y * width ] = ( abgr[ x + y * width ] & 0xff000000 ) | ( palette->colortable[ index ] & 0x00ffffff );
			}
		}
		
    }


#endif /* PALDITHER_IMPLEMENTATION */



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
