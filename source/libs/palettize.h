/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

palettize.h - v0.1 - 
*/

/* TODO: custom allocators / memctx */


#ifndef palettize_h
#define palettize_h

#ifndef PALETTIZE_U32
	#define PALETTIZE_U32 unsigned int
#endif

#ifndef PALETTIZE_U16
	#define PALETTIZE_U16 unsigned short
#endif

#ifndef PALETTIZE_U8
	#define PALETTIZE_U8 unsigned char
#endif


int palettize_gen_palette_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, PALETTIZE_U32* palette, int palette_size );
void palettize_remap_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, PALETTIZE_U32* palette, int palette_size, PALETTIZE_U8* output );

int palettize_gen_palette_rgb16( PALETTIZE_U16 const* rgb, int width, int height, PALETTIZE_U16* palette, int palette_size );
void palettize_remap_rgb16( PALETTIZE_U16 const* rgb, int width, int height, PALETTIZE_U16* palette, int palette_size, PALETTIZE_U8* output );

#endif /* palettize_h */

/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifdef PALETTIZE_IMPLEMENTATION
#undef PALETTIZE_IMPLEMENTATION

/* TODO: refactor and clean up  */

#ifndef __MedianCutPalettizer_H__
#define __MedianCutPalettizer_H__

// Includes

// Forward declares

// MedianCutPalettizer
namespace MedianCutPalettizer
	{
	int GeneratePalette(
		unsigned int* imageData, 
		int imageWidth,
		int imageHeight,
		unsigned int* palette, 
		int paletteMaxCount
		);

	int GeneratePalette(
		unsigned short* imageData, 
		int imageWidth,
		int imageHeight,
		unsigned short* palette, 
		int paletteMaxCount
		);

	int GeneratePalette(
		unsigned int* imageData, 
		int imageWidth,
		int imageHeight,
		unsigned short* palette, 
		int paletteMaxCount
		);

	void PalettizeImage(
		unsigned int* imageData, 
		int imageWidth,
		int imageHeight,
		unsigned int* palette, 
		int paletteCount,
		unsigned char* outputData
		);

	void PalettizeImage(
		unsigned short* imageData, 
		int imageWidth,
		int imageHeight,
		unsigned short* palette, 
		int paletteCount,
		unsigned char* outputData
		);
	};
	 
#endif /* __MedianCutPalettizer_H__ */


__forceinline unsigned short RGB32TO16(
	unsigned int color
	)
	{
	return
		((unsigned short) (
			((color & 0x00f80000)>>8) |
			((color & 0x0000fc00)>>5) |
			((color & 0x000000f8)>>3)
		));
	}

// Helper function for converting color values from 16 bit R5G6B5 to 32 bit X8R8G8B8
__forceinline unsigned int RGB16TO32(
	unsigned short color
	)
	{
	return 
		(((unsigned int)color & 0xf800)<<8) |
		(((unsigned int)color & 0x07e0)<<5) |
		(((unsigned int)color & 0x001f)<<3) |
		(0xff000000);
	}


const int NUM_DIMENSIONS = 3;

struct PalPoint
{
    unsigned char x[NUM_DIMENSIONS];
};

struct Block
{
private:
    PalPoint minCorner, maxCorner;
    PalPoint* points;
    int pointsLength;
public:
    Block(PalPoint* points, int pointsLength);
    PalPoint * getPoints();
    int numPoints() const;
    int longestSideIndex() const;
    int longestSideLength() const;
    bool operator<(const Block& rhs) const;
    bool operator>(const Block& rhs) const;
    void shrink();
private:
    template< typename T >
    static T Min(const T a, const T b)
    {
        if (a < b)
            return a;
        else
            return b;
    }

    template< typename T >
    static T Max(const T a, const T b)
    {
        if (a > b)
            return a;
        else
            return b;
    }

};

template< int index >
struct CoordinatePointComparator
{
    bool operator()(PalPoint left, PalPoint right)
    {
        return left.x[index] < right.x[index];
    }
};


Block::Block(PalPoint* points, int pointsLength)
{
    this->points = points;
    this->pointsLength = pointsLength;
    for(int i=0; i < NUM_DIMENSIONS; i++)
    {
        minCorner.x[i] = 0;
        maxCorner.x[i] = 255;
    }
}

PalPoint * Block::getPoints()
{
    return points;
}

int Block::numPoints() const
{
    return pointsLength;
}

int Block::longestSideIndex() const
{
    int m = maxCorner.x[0] - minCorner.x[0];
    int maxIndex = 0;
    for(int i=1; i < NUM_DIMENSIONS; i++)
    {
        int diff = maxCorner.x[i] - minCorner.x[i];
        if (diff > m)
        {
            m = diff;
            maxIndex = i;
        }
    }
    return maxIndex;
}

int Block::longestSideLength() const
{
    int i = longestSideIndex();
    return maxCorner.x[i] - minCorner.x[i];
}

bool Block::operator<(const Block& rhs) const
{
    return this->longestSideLength() < rhs.longestSideLength();
}

bool Block::operator>(const Block& rhs) const
{
    return this->longestSideLength() > rhs.longestSideLength();
}

void Block::shrink()
{
    int i,j;
    for(j=0; j<NUM_DIMENSIONS; j++)
    {
        minCorner.x[j] = maxCorner.x[j] = points[0].x[j];
    }
    for(i=1; i < pointsLength; i++)
    {
        for(j=0; j<NUM_DIMENSIONS; j++)
        {
            minCorner.x[j] = Min(minCorner.x[j], points[i].x[j]);
            maxCorner.x[j] = Max(maxCorner.x[j], points[i].x[j]);
        }
    }
}


template< typename T, int (*COMPARE)( T const&, T const& ) > 
void priority_queue_push( array_ns::array<T>* queue, T const& value )
    {
    queue->add( value );
	int index=queue->count();
	while (index>1 && COMPARE( (*queue)[index-1], (*queue)[index/2-1] ) > 0 )
		{
		// Swap items
		T temp=(*queue)[index/2-1];
		(*queue)[index/2-1]=(*queue)[index-1];
		(*queue)[index-1]=temp;

		index=index/2;
		}
    }


template< typename T, int (*COMPARE)( T const&, T const& ) > 
void priority_queue_pop( array_ns::array<T>* queue )
    {
	if (queue->count()==0) return;

    (*queue)[0]=(*queue)[queue->count()-1];
    queue->remove( queue->count() - 1 );	

	int v=1;
	int u=0;
	while (u!=v)
		{
		u=v;
		// If both children exist
		if ((2*u+1)<=queue->count()) 
			{
			// Select the lowest of the two children.
			if (COMPARE((*queue)[u-1],(*queue)[2*u -1]) <= 0)
				{
				v=2*u;
				}
			if ( COMPARE((*queue)[v-1],(*queue)[2*u+1 -1]) <= 0 )
				{
				v=2*u+1;
				}
			}

		// If only child #1 exists
		else if (2*u<=queue->count())
			{
			// Check if the cost is greater than the child
			if (COMPARE((*queue)[u-1],(*queue)[2*u-1]) <= 0 )
				{
				v=2*u;
				}
			}

		if (u!=v)
			{
			// Swap items
			T temp=(*queue)[u-1];
			(*queue)[u-1]=(*queue)[v-1];
			(*queue)[v-1]=temp;
			}
		}
	}


template< typename T > 
T const& priority_queue_top( array_ns::array<T> const& queue )
	{
	return queue[ 0 ];
	}
	
	
template< typename T > 
T& priority_queue_top( array_ns::array<T>& queue )
	{
	return queue[ 0 ];
	}
	
	
template< typename T > 
int priority_queue_default_compare( T const& a, T const& b ) 
    {
    return ( a < b ) ? -1 : ( a > b ) ? 1 : 0; 
    }


template< typename T > 
void priority_queue_push( array_ns::array<T>* queue, T const& value )
	{
	priority_queue_push< T, priority_queue_default_compare<T> >( queue, value );
	}


template< typename T > 
void priority_queue_pop( array_ns::array<T>* queue )
	{
	priority_queue_pop< T, priority_queue_default_compare<T> >( queue );
	}


	
#pragma warning( push )
#pragma warning( disable: 4619 ) // there is no warning number 'nnnn'
#pragma warning( disable: 4577 )
#include <algorithm> // TODO: replace with custom implementation
#pragma warning( pop )

array_ns::array<PalPoint> medianCut(PalPoint* image, int numPoints, unsigned int desiredSize)
{
    array_ns::array<Block> blockQueue;

    Block initialBlock(image, numPoints);
    initialBlock.shrink();

    priority_queue_push( &blockQueue, initialBlock );
    while (blockQueue.count() < (int)desiredSize && priority_queue_top( blockQueue ).numPoints() > 1)
    {
        Block longestBlock = priority_queue_top( blockQueue );

        priority_queue_pop( &blockQueue );
        PalPoint * begin  = longestBlock.getPoints();
	PalPoint * median = longestBlock.getPoints() + (longestBlock.numPoints()+1)/2;
	PalPoint * end    = longestBlock.getPoints() + longestBlock.numPoints();

	switch(longestBlock.longestSideIndex())
	{
	    case 0: std::nth_element(begin, median, end, CoordinatePointComparator<0>()); break;
	    case 1: std::nth_element(begin, median, end, CoordinatePointComparator<1>()); break;
	    case 2: std::nth_element(begin, median, end, CoordinatePointComparator<2>()); break;
	}

	Block block1(begin, (int)( median-begin ) ), block2( median, (int)( end-median ) );
	block1.shrink();
	block2.shrink();

        priority_queue_push( &blockQueue, block1);
        priority_queue_push( &blockQueue, block2);
    }

    array_ns::array<PalPoint> result;
    while(blockQueue.count() > 0)
    {
        Block block = priority_queue_top( blockQueue );
        priority_queue_pop( &blockQueue );
        PalPoint * points = block.getPoints();

        int sum[NUM_DIMENSIONS] = {0};
        for(int i=0; i < block.numPoints(); i++)
        {
            for(int j=0; j < NUM_DIMENSIONS; j++)
            {
                sum[j] += points[i].x[j];
            }
        }

        PalPoint averagePoint;
        for(int j=0; j < NUM_DIMENSIONS; j++)
        {
            averagePoint.x[j] = (unsigned char) ( sum[j] / block.numPoints() );
        }

        result.add(averagePoint);
    }

    return result;
}



unsigned char FindNearestColor(unsigned int color, unsigned int* palette, int paletteCount) 
	{
    int i, distanceSquared, minDistanceSquared, bestIndex = 0;
    minDistanceSquared = 255*255 + 255*255 + 255*255 + 1;
    for (i=0; i<paletteCount; i++) 
		{
		unsigned char cR=((unsigned char)((color&0x00ff0000)>>16));
		unsigned char cG=((unsigned char)((color&0x0000ff00)>>8 ));
		unsigned char cB=((unsigned char)((color&0x000000ff)    ));
		unsigned char pR=((unsigned char)((palette[i]&0x00ff0000)>>16));
		unsigned char pG=((unsigned char)((palette[i]&0x0000ff00)>>8 ));
		unsigned char pB=((unsigned char)((palette[i]&0x000000ff)    ));
        int Rdiff = ((int)cR) - pR;
        int Gdiff = ((int)cG) - pG;
        int Bdiff = ((int)cB) - pB;
        distanceSquared = Rdiff*Rdiff + Gdiff*Gdiff + Bdiff*Bdiff;
        if (distanceSquared < minDistanceSquared) 
			{
            minDistanceSquared = distanceSquared;
            bestIndex = i;
			}
		}
    return (unsigned char)bestIndex;
	}


unsigned char FindNearestColor(unsigned short color, unsigned short* palette, int paletteCount) 
	{
    int i, distanceSquared, minDistanceSquared, bestIndex = 0;
    minDistanceSquared = 255*255 + 255*255 + 255*255 + 1;
    for (i=0; i<paletteCount; i++) 
		{
		unsigned char cR=((unsigned char)((RGB16TO32(color)&0x00ff0000)>>16));
		unsigned char cG=((unsigned char)((RGB16TO32(color)&0x0000ff00)>>8 ));
		unsigned char cB=((unsigned char)((RGB16TO32(color)&0x000000ff)    ));
		unsigned char pR=((unsigned char)((RGB16TO32(palette[i])&0x00ff0000)>>16));
		unsigned char pG=((unsigned char)((RGB16TO32(palette[i])&0x0000ff00)>>8 ));
		unsigned char pB=((unsigned char)((RGB16TO32(palette[i])&0x000000ff)    ));
        int Rdiff = ((int)cR) - pR;
        int Gdiff = ((int)cG) - pG;
        int Bdiff = ((int)cB) - pB;
        distanceSquared = Rdiff*Rdiff + Gdiff*Gdiff + Bdiff*Bdiff;
        if (distanceSquared < minDistanceSquared) 
			{
            minDistanceSquared = distanceSquared;
            bestIndex = i;
			}
		}
    return (unsigned char)bestIndex;
	}


//*** GeneratePalette ***

int MedianCutPalettizer::GeneratePalette(unsigned short* imageData, int imageWidth, int imageHeight, unsigned short* palette, int paletteMaxCount)
	{
	int colorCount=imageWidth*imageHeight;
	PalPoint* data= (PalPoint*) malloc( sizeof( PalPoint ) * colorCount );
	int dataSize=0;
	for (int p=0; p<colorCount; p++)
		{
		unsigned int color=RGB16TO32(*imageData);
		imageData++;
		unsigned char a=((unsigned char)((color&0xff000000)>>24));
		if (a>0)
			{
			unsigned char b=((unsigned char)((color&0x00ff0000)>>16));
			unsigned char g=((unsigned char)((color&0x0000ff00)>>8 ));
			unsigned char r=((unsigned char)((color&0x000000ff)    ));
			data[dataSize].x[0]=r;
			data[dataSize].x[1]=g;
			data[dataSize].x[2]=b;
			dataSize++;
			}
		}
	array_ns::array<PalPoint> result=medianCut(data,dataSize,(unsigned int)paletteMaxCount);
	free( data );

    int i = 0;
	for( int it = 0; it < result.count(); ++it )
		{
		PalPoint p=result[ it ];
		unsigned int c=0xff000000;
		c|=p.x[2]<<16;
		c|=p.x[1]<<8;
		c|=p.x[0];
		int found=false;
		for (int j=0; j<i; j++)
			{
			if (palette[j]==RGB32TO16(c))
				{
				found=true;
				break;
				}
			}
		if (!found)
			{
			palette[i]=RGB32TO16(c);
			i++;
            if( i >= paletteMaxCount ) 
                return i;
			}
		}

	return i;
	}


//*** GeneratePalette ***

int MedianCutPalettizer::GeneratePalette(unsigned int* imageData, int imageWidth, int imageHeight, unsigned int* palette, int paletteMaxCount)
	{
	int colorCount=imageWidth*imageHeight;
	PalPoint* data= (PalPoint*) malloc( sizeof( PalPoint ) * colorCount );
	int dataSize=0;
	for (int p=0; p<colorCount; p++)
		{
		unsigned int color=*imageData;
		imageData++;
		unsigned char a=((unsigned char)((color&0xff000000)>>24));
		if (a>0)
			{
			unsigned char b=((unsigned char)((color&0x00ff0000)>>16));
			unsigned char g=((unsigned char)((color&0x0000ff00)>>8 ));
			unsigned char r=((unsigned char)((color&0x000000ff)    ));
			data[dataSize].x[0]=r;
			data[dataSize].x[1]=g;
			data[dataSize].x[2]=b;
			dataSize++;
			}
		}
	array_ns::array<PalPoint> result=medianCut(data,dataSize,(unsigned int)paletteMaxCount);
	free( data );
	
    int i = 0;
	for( int it = 0; it < result.count(); ++it )
		{
		PalPoint p=result[ it ];
		unsigned int c=0xff000000;
		c|=p.x[2]<<16;
		c|=p.x[1]<<8;
		c|=p.x[0];
		int found=false;
		for (int j=0; j<i; j++)
			{
			if (palette[j]==c)
				{
				found=true;
				break;
				}
			}
		if (!found)
			{
			palette[i]=c;
			i++;
            if( i >= paletteMaxCount ) 
                return i;
			}
		}

	return i;
	}


//*** GeneratePalette ***

int MedianCutPalettizer::GeneratePalette(unsigned int* imageData, int imageWidth, int imageHeight, unsigned short* palette, int paletteMaxCount)
	{
	int colorCount=imageWidth*imageHeight;
	PalPoint* data= (PalPoint*) malloc( sizeof( PalPoint ) * colorCount );
	int dataSize=0;
	for (int p=0; p<colorCount; p++)
		{
		unsigned int color=*imageData;
		imageData++;
		unsigned char a=((unsigned char)((color&0xff000000)>>24));
		if (a>0)
			{
			color=RGB16TO32(RGB32TO16(color));
			unsigned char b=((unsigned char)((color&0x00ff0000)>>16));
			unsigned char g=((unsigned char)((color&0x0000ff00)>>8 ));
			unsigned char r=((unsigned char)((color&0x000000ff)    ));
			data[dataSize].x[0]=r;
			data[dataSize].x[1]=g;
			data[dataSize].x[2]=b;
			dataSize++;
			}
		}
	array_ns::array<PalPoint> result=medianCut(data,dataSize,(unsigned int)paletteMaxCount);
	free( data );
	
    int i = 0;
	for( int it = 0; it < result.count(); ++it )
		{
		PalPoint p=result[ it ];
		unsigned int c=0xff000000;
		c|=p.x[2]<<16;
		c|=p.x[1]<<8;
		c|=p.x[0];
		unsigned short c16=RGB32TO16(c);
		int found=false;
		for (int j=0; j<i; j++)
			{
			if (palette[j]==c16)
				{
				found=true;
				break;
				}
			}
		if (!found)
			{
			palette[i]=c16;
			i++;
            if( i >= paletteMaxCount ) 
                return i;
			}
		}

	return i;
	}


//*** PalettizeImage ***

void MedianCutPalettizer::PalettizeImage(unsigned int* imageData, int imageWidth, int imageHeight, unsigned int* palette, int paletteCount, unsigned char* outputData)
	{
	for (int y=0; y<imageHeight; y++)
		{
		for (int x=0; x<imageWidth; x++)
			{
			unsigned char paletteIndex=FindNearestColor(*imageData,palette,paletteCount);
			*outputData=paletteIndex;
			imageData++;
			outputData++;
			}
		}
	}


//*** PalettizeImage ***

void MedianCutPalettizer::PalettizeImage(unsigned short* imageData, int imageWidth, int imageHeight, unsigned short* palette, int paletteCount, unsigned char* outputData)
	{
	for (int y=0; y<imageHeight; y++)
		{
		for (int x=0; x<imageWidth; x++)
			{
			unsigned char paletteIndex=FindNearestColor(*imageData,palette,paletteCount);
			*outputData=paletteIndex;
			imageData++;
			outputData++;
			}
		}
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int palettize_gen_palette_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, PALETTIZE_U32* palette, int palette_size )
	{
	return MedianCutPalettizer::GeneratePalette( (unsigned int*) xbgr, width, height, (unsigned int*) palette, palette_size );
	}

void palettize_remap_xbgr32( PALETTIZE_U32 const* xbgr, int width, int height, PALETTIZE_U32* palette, int palette_size, PALETTIZE_U8* output )
	{
	MedianCutPalettizer::PalettizeImage( (unsigned int*) xbgr, width, height, (unsigned int*) palette, palette_size, output );
	}

int palettize_gen_palette_rgb16( PALETTIZE_U16 const* rgb, int width, int height, PALETTIZE_U16* palette, int palette_size )
	{
	return MedianCutPalettizer::GeneratePalette( (unsigned short*) rgb, width, height, (unsigned short*) palette, palette_size );
	}

void palettize_remap_rgb16( PALETTIZE_U16 const* rgb, int width, int height, PALETTIZE_U16* palette, int palette_size, PALETTIZE_U8* output )
	{
	MedianCutPalettizer::PalettizeImage( (unsigned short*) rgb, width, height, (unsigned short*) palette, palette_size, output );
	}


#endif /* PALETTIZE_IMPLEMENTATION */


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

