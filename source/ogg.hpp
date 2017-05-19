#define STB_VORBIS_HEADER_ONLY
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_INTEGER_CONVERSION
#include "libs/stb_vorbis.h"
	
struct ogg_t
	{
	float* sample_pairs;
	int sample_pairs_count;
	};


int ogg_length_in_sample_pairs( void* instance )
	{
	ogg_t* ogg = (ogg_t*) instance;
	return ogg->sample_pairs_count;
	}

int ogg_sample_pairs( void* instance, int offset_in_sample_pairs, float* sample_pairs, int sample_pairs_count )
	{
	ogg_t* ogg = (ogg_t*) instance;
	int count_left = ogg->sample_pairs_count - offset_in_sample_pairs;
	if( count_left <= 0 ) return 0;
	int to_copy = sample_pairs_count > count_left ? count_left : sample_pairs_count;
	memcpy( sample_pairs, ogg->sample_pairs + offset_in_sample_pairs * 2, sizeof( float ) * 2 * to_copy );
	return to_copy;
	}
	
void ogg_release( void* instance )
	{
	ogg_t* ogg = (ogg_t*) instance;
	(void) ogg;
	}
	
audiosys_audio_source_t ogg_load( void const* data, int size )
	{
	audiosys_audio_source_t source;
	memset( &source, 0, sizeof( source ) );

	float* sample_pairs = 0;
	int sample_pairs_count = 0;

	if( strnicmp( (char const*) data, "OggS", 4 ) != 0 ) return source;
	
	int ogg_error = 0;

	stb_vorbis_alloc ogg_alloc;
	ogg_alloc.alloc_buffer_length_in_bytes = 2 * 1024 * 1024;
	ogg_alloc.alloc_buffer = (char*) malloc( (size_t) ogg_alloc.alloc_buffer_length_in_bytes );

	stb_vorbis* ogg = stb_vorbis_open_memory( (unsigned char const*) data, (int)size, &ogg_error, &ogg_alloc );
	while( ogg_error == VORBIS_outofmem )
		{
		free( ogg_alloc.alloc_buffer );
		ogg_alloc.alloc_buffer_length_in_bytes *= 2;
		ogg_alloc.alloc_buffer = (char*) malloc( (size_t) ogg_alloc.alloc_buffer_length_in_bytes );
		ogg_error = 0;
		ogg = stb_vorbis_open_memory( (unsigned char const*) data, (int)size, &ogg_error, &ogg_alloc );
		}
	
	assert( ogg_error == VORBIS__no_error );
	if( ogg_error != VORBIS__no_error ) 
		{
		free( ogg_alloc.alloc_buffer );
		return source;
		}
	
	stb_vorbis_info info = stb_vorbis_get_info( ogg );
	assert( ( info.channels == 2 || info.channels == 1 ) && info.sample_rate == 44100 );
	if( !( info.channels == 2 || info.channels == 1 ) || info.sample_rate != 44100 )
		{
		free( ogg_alloc.alloc_buffer );
		stb_vorbis_close( ogg );
		return source;
		}
	
	unsigned int length = stb_vorbis_stream_length_in_samples( ogg );
	sample_pairs_count = (int) length;
	sample_pairs = (float*) malloc( sizeof( float ) * length * 2 );
	
	unsigned int p = 0;
	while( p < length * 2 )
		{
		int n;
		float **outputs;
		n = stb_vorbis_get_frame_float( ogg, 0, &outputs );
		if( n == 0 ) break;
		for( int i = 0; i < n; ++i )
			{
			if( info.channels == 2 )
				{
				sample_pairs[ p++ ] = outputs[ 0 ][ i ];
				sample_pairs[ p++ ] = outputs[ 1 ][ i ];
				}
			else
				{
				sample_pairs[ p++ ] = outputs[ 0 ][ i ];
				sample_pairs[ p++ ] = outputs[ 0 ][ i ];
				}
			}
		}
	stb_vorbis_close( ogg );
	free( ogg_alloc.alloc_buffer );
	
		
	ogg_t* ogg_out = (ogg_t*) malloc( sizeof( ogg_t ) );
	ogg_out->sample_pairs = sample_pairs;
	ogg_out->sample_pairs_count = sample_pairs_count;
		
	source.instance = ogg_out;
	source.length_in_sample_pairs = ogg_length_in_sample_pairs;
	source.release = ogg_release;
	source.sample_pairs = ogg_sample_pairs;
	return source;
	}
	
	
void ogg_free( audiosys_audio_source_t source )
	{
	ogg_t* ogg = (ogg_t*) source.instance;
	if( ogg && ogg->sample_pairs ) free( ogg->sample_pairs );
	if( ogg ) free( ogg );
	}
