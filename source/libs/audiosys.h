/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

audiosys.h - v0.1 - Sound and music playback for C/C++.

Do this:
	#define AUDIOSYS_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.

Dependencies: 
    handles.h
*/

#ifndef audiosys_h
#define audiosys_h

#ifndef AUDIOSYS_S16
    #define AUDIOSYS_S16 signed short
#endif

#ifndef AUDIOSYS_U64
    #define AUDIOSYS_U64 unsigned long long
#endif

#define AUDIOSYS_DEFAULT_BUFFERED_SAMPLE_PAIRS_COUNT 22050
#define AUDIOSYS_DEFAULT_VOICE_COUNT 16

#define AUDIOSYS_FEATURE_MUSIC 0x0001
#define AUDIOSYS_FEATURE_MUSIC_CROSSFADE 0x0002
#define AUDIOSYS_FEATURE_AMBIENCE 0x0004
#define AUDIOSYS_FEATURE_AMBIENCE_CROSSFADE 0x0008
#define AUDIOSYS_FEATURES_ALL 0xffff

#define AUDIOSYS_UNDEFINED_LENGTH -1

typedef struct audiosys_audio_source_t
{
    void* instance;
    int (*length_in_sample_pairs)( void* instance ); // TODO: remove - detect looping by sample_pairs() returning 0
    int (*sample_pairs)( void* instance, int offset_in_sample_pairs, float* sample_pairs, int sample_pairs_count );
    void (*release)( void* instance );
} audiosys_audio_source_t;


typedef struct audiosys_t audiosys_t;
audiosys_t* audiosys_create( int active_voice_count, int buffered_sample_pairs_count, int features, void* memctx );
void audiosys_destroy( audiosys_t* audiosys );

void audiosys_update( audiosys_t* audiosys );
int audiosys_consume( audiosys_t* audiosys, int sample_pairs_to_advance, 
    AUDIOSYS_S16* output_sample_pairs, int output_sample_pairs_count );

void audiosys_master_volume_set( audiosys_t* audiosys, float volume );
float audiosys_master_volume( audiosys_t* audiosys );

void audiosys_gain_set( audiosys_t* audiosys, float gain );
float audiosys_gain( audiosys_t* audiosys );

void audiosys_pause( audiosys_t* audiosys );
void audiosys_resume( audiosys_t* audiosys );

typedef enum audiosys_paused_t
    {
    AUDIOSYS_NOT_PAUSED,
    AUDIOSYS_PAUSED,
    } audiosys_paused_t;

audiosys_paused_t audiosys_paused( audiosys_t* audiosys );


typedef enum audiosys_loop_t
    {
    AUDIOSYS_LOOP_OFF,
    AUDIOSYS_LOOP_ON,
    } audiosys_loop_t;

void audiosys_music_play( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_in_time );
void audiosys_music_stop( audiosys_t* audiosys, float fade_out_time );
void audiosys_music_pause( audiosys_t* audiosys );
void audiosys_music_resume( audiosys_t* audiosys );
void audiosys_music_switch( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_out_time, float fade_in_time );
void audiosys_music_cross_fade( audiosys_t* audiosys, audiosys_audio_source_t source, float cross_fade_time );
void audiosys_music_position_set( audiosys_t* audiosys, float position );
float audiosys_music_position( audiosys_t* audiosys );
audiosys_audio_source_t audiosys_music_source( audiosys_t* audiosys );
void audiosys_music_loop_set( audiosys_t* audiosys, audiosys_loop_t loop );
audiosys_loop_t audiosys_music_loop( audiosys_t* audiosys );
void audiosys_music_volume_set( audiosys_t* audiosys, float volume );
float audiosys_music_volume( audiosys_t* audiosys );
void audiosys_music_pan_set( audiosys_t* audiosys, float pan );
float audiosys_music_pan( audiosys_t* audiosys );

void audiosys_ambience_play( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_in_time );
void audiosys_ambience_stop( audiosys_t* audiosys, float fade_out_time );
void audiosys_ambience_pause( audiosys_t* audiosys );
void audiosys_ambience_resume( audiosys_t* audiosys );
void audiosys_ambience_switch( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_out_time, float fade_in_time );
void audiosys_ambience_cross_fade( audiosys_t* audiosys, audiosys_audio_source_t source, float cross_fade_time );
void audiosys_ambience_position_set( audiosys_t* audiosys, float position );
float audiosys_ambience_position( audiosys_t* audiosys );
audiosys_audio_source_t audiosys_ambience_source( audiosys_t* audiosys );
void audiosys_ambience_loop_set( audiosys_t* audiosys, audiosys_loop_t loop );
audiosys_loop_t audiosys_ambience_loop( audiosys_t* audiosys );
void audiosys_ambience_volume_set( audiosys_t* audiosys, float volume );
float audiosys_ambience_volume( audiosys_t* audiosys );
void audiosys_ambience_pan_set( audiosys_t* audiosys, float pan );
float audiosys_ambience_pan( audiosys_t* audiosys );

AUDIOSYS_U64 audiosys_sound_play( audiosys_t* audiosys, audiosys_audio_source_t source, float priority, float fade_in_time );
void audiosys_sound_stop( audiosys_t* audiosys, AUDIOSYS_U64 handle, float fade_out_time );
void audiosys_sound_pause( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_resume( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_position_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float position );
float audiosys_sound_position( audiosys_t* audiosys, AUDIOSYS_U64 handle );
audiosys_audio_source_t audiosys_sound_source( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_loop_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, audiosys_loop_t loop );
audiosys_loop_t audiosys_sound_loop( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_volume_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float volume );
float audiosys_sound_volume( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_pan_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float pan );
float audiosys_sound_pan( audiosys_t* audiosys, AUDIOSYS_U64 handle );


typedef enum audiosys_sound_valid_t
    {
    AUDIOSYS_SOUND_INVALID,
    AUDIOSYS_SOUND_VALID,
    } audiosys_sound_valid_t;

audiosys_sound_valid_t audiosys_sound_valid(audiosys_t* audiosys, AUDIOSYS_U64 handle );

#endif /* audiosys_h */

/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifdef AUDIOSYS_IMPLEMENTATION
#undef AUDIOSYS_IMPLEMENTATION

#ifndef AUDIOSYS_MALLOC
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
	#include <stdlib.h>
	#define AUDIOSYS_MALLOC( ctx, size ) ( malloc( size ) )
	#define AUDIOSYS_FREE( ctx, ptr ) ( free( ptr ) )
#endif

#ifndef AUDIOSYS_MEMCPY
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define AUDIOSYS_MEMCPY( dst, src, cnt ) ( memcpy( dst, src, cnt ) )
#endif 

#ifndef AUDIOSYS_MEMMOVE
    #define _CRT_NONSTDC_NO_DEPRECATE 
    #define _CRT_SECURE_NO_WARNINGS
    #include <string.h>
    #define AUDIOSYS_MEMMOVE( dst, src, cnt ) ( memmove( dst, src, cnt ) )
#endif 

#include "handles.h"


typedef enum audiosys_internal_voice_state_t
	{
	AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED,
	AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING,
	AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN,
	AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT,
	AUDIOSYS_INTERNAL_VOICE_STATE_CROSSFADING,
	AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED,
	} audiosys_internal_voice_state_t;


typedef struct audiosys_internal_voice_t
	{
	AUDIOSYS_U64 handle;
	int modified;
	int initialized;
	int paused;
	audiosys_internal_voice_state_t state;
	audiosys_audio_source_t source;
	int position;
	int loop;
	float volume;
	float pan;
	float fade_in_time;
	float fade_out_time;
	float priority;
	float fade_progress;
	int assigned_voice_index;
	} audiosys_internal_voice_t;
		

struct audiosys_t
	{
    void* memctx;

    int modified;
	float master_volume;
	int paused;
    float gain;
	
	audiosys_internal_voice_t music;
	audiosys_internal_voice_t music_crossfade;
	float music_crossfade_time;
	
	audiosys_internal_voice_t ambience;
	audiosys_internal_voice_t ambience_crossfade;
	float ambience_crossfade_time;
	
    int sounds_capacity;
    int sounds_count;
	audiosys_internal_voice_t* sounds;
	int* sorted_sounds;
	handles_t sounds_handles;
	
    int buffered_sample_pairs_count;
    int active_voice_count;
	float** active_voice_data_buffers;
    AUDIOSYS_S16* mix_buffer;

	int voice_music_primary;
	int voice_music_crossfade;
	int voice_ambience_primary;
	int voice_ambience_crossfade;
	int voice_sounds_start;
	
	int previous_update_last_updated_voice_index;
    int sample_pairs_to_advance_next_update;
    };


audiosys_t* audiosys_create( int active_voice_count, int buffered_sample_pairs_count, int features, void* memctx )
    {
    size_t size = sizeof( audiosys_t ) + active_voice_count * buffered_sample_pairs_count * 2 * sizeof( float ) + 
        + buffered_sample_pairs_count * 2 * sizeof( AUDIOSYS_S16 ) + sizeof( float* ) * buffered_sample_pairs_count;
    audiosys_t* audiosys = (audiosys_t*) AUDIOSYS_MALLOC( memctx, size );
    memset( audiosys, 0, size );

    audiosys->memctx = memctx;
    audiosys->gain = 1.0f;

    audiosys->active_voice_count = active_voice_count;
    audiosys->master_volume = 1.0f;
    audiosys->music.volume = 1.0f;
    audiosys->music_crossfade.volume = 1.0f;
    audiosys->ambience.volume = 1.0f;
    audiosys->ambience_crossfade.volume = 1.0f;

    audiosys->sounds_capacity = 128;
    audiosys->sounds_count = 0;
    audiosys->sounds = (audiosys_internal_voice_t*) AUDIOSYS_MALLOC( memctx, 
        ( sizeof( audiosys_internal_voice_t ) + sizeof( int ) ) * audiosys->sounds_capacity );
    audiosys->sorted_sounds = (int*) ( audiosys->sounds + audiosys->sounds_capacity );
    handles_init( &audiosys->sounds_handles, audiosys->sounds_capacity, memctx );

    audiosys->buffered_sample_pairs_count = buffered_sample_pairs_count;
    audiosys->mix_buffer = (AUDIOSYS_S16*)( audiosys + 1 );
    audiosys->active_voice_data_buffers = (float**)( audiosys->mix_buffer + buffered_sample_pairs_count * 2 );
    float* buffers_array = (float*)( audiosys->active_voice_data_buffers + active_voice_count );
    for( int i = 0; i < active_voice_count; ++i )
        {
        audiosys->active_voice_data_buffers[ i ] = buffers_array;
        buffers_array += buffered_sample_pairs_count * 2;
        }

    int voice_index = 0;
    audiosys->voice_music_primary = ( features & AUDIOSYS_FEATURE_MUSIC ) ? voice_index++ : -1;
    audiosys->voice_music_crossfade = ( features & AUDIOSYS_FEATURE_MUSIC_CROSSFADE ) ? voice_index++ : -1;
    audiosys->voice_ambience_primary = ( features & AUDIOSYS_FEATURE_AMBIENCE ) ? voice_index++ : -1;
    audiosys->voice_ambience_crossfade = ( features & AUDIOSYS_FEATURE_AMBIENCE_CROSSFADE ) ? voice_index++ : -1;
    audiosys->voice_sounds_start = voice_index;

    return audiosys;
    }


void audiosys_destroy( audiosys_t* audiosys )
	{

    handles_term( &audiosys->sounds_handles );
    AUDIOSYS_FREE( audiosys->memctx, audiosys->sounds );
    AUDIOSYS_FREE( audiosys->memctx, audiosys );
	}


static AUDIOSYS_U64 audiosys_internal_add_sound( audiosys_t* audiosys, audiosys_internal_voice_t** sound, float priority )
	{
	if( audiosys->sounds_count >= audiosys->sounds_capacity )
		{
		int new_capacity = audiosys->sounds_capacity * 2;
		audiosys_internal_voice_t* new_sounds = (audiosys_internal_voice_t*) AUDIOSYS_MALLOC( audiosys->memctx, 
			( sizeof( audiosys_internal_voice_t ) + sizeof( int ) ) * new_capacity );
		AUDIOSYS_MEMCPY( new_sounds, audiosys->sounds, sizeof( audiosys_internal_voice_t ) * audiosys->sounds_count );
		AUDIOSYS_MEMCPY( new_sounds + new_capacity, audiosys->sounds + audiosys->sounds_capacity, 
			sizeof( int ) * audiosys->sounds_count );
		AUDIOSYS_FREE( audiosys->memctx, audiosys->sounds );
		audiosys->sounds_capacity = new_capacity;
		audiosys->sounds = new_sounds;
		}

	// TODO: binary search for insertion point
	int sorted_index = audiosys->sounds_count;
	for( int i = 0; i < audiosys->sounds_count; ++i )
		{
		if( audiosys->sounds[ audiosys->sorted_sounds[ i ] ].priority < priority )
			{
			AUDIOSYS_MEMMOVE( &audiosys->sorted_sounds[ i + 1 ], &audiosys->sorted_sounds[ i ], 
				( audiosys->sounds_count - i ) * sizeof( int ) );
			sorted_index = i;
			break;
			}
		}

	int index = audiosys->sounds_count++;
	*sound = &audiosys->sounds[ index ];
	audiosys->sorted_sounds[ sorted_index ] = index;
	AUDIOSYS_U64 handle = handles_to_u64( &audiosys->sounds_handles, handles_alloc( &audiosys->sounds_handles, index ) );
	return handle;
	}

	
static void audiosys_internal_remove_sound( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
	int index = handles_index( &audiosys->sounds_handles, handles_from_u64( &audiosys->sounds_handles, handle ) );
	if( index < 0 ) return;

	// TODO: update sorted sounds (keep track of which sorted index each sound is at)
		
	handles_release( &audiosys->sounds_handles, handles_from_u64( &audiosys->sounds_handles, audiosys->sounds[ index ].handle ) );
	if( index != audiosys->sounds_count - 1 )
		{
		audiosys->sounds[ index ] = audiosys->sounds[ --audiosys->sounds_count ];
		handles_update( &audiosys->sounds_handles, handles_from_u64( &audiosys->sounds_handles, audiosys->sounds[ index ].handle ), index );
		}		
	else
		{
		--audiosys->sounds_count;
		}
	}
	

static int audiosys_internal_update_voice( audiosys_t* audiosys, audiosys_internal_voice_t* voice, int advance, int sample_pairs_count )		
	{	
    int finished = 0;
	float delta_time = 0.0f;
		
	// advance position
	if( voice->initialized ) 
		{
		if( voice->position < voice->source.length_in_sample_pairs( voice->source.instance ) ) 
			{
			voice->position += advance;
			delta_time = advance / 44100.0f;
			}
				
		if( voice->position >= voice->source.length_in_sample_pairs( voice->source.instance ) )
			{
			if( voice->loop ) 
				{
				voice->position = voice->position % voice->source.length_in_sample_pairs( voice->source.instance );
				}
			else 
				{				
                if( voice->source.release ) voice->source.release( voice->source.instance );
				voice->source.instance = NULL;
                voice->source.length_in_sample_pairs = NULL;
                voice->source.release = NULL;
                voice->source.sample_pairs = NULL;
				memset( audiosys->active_voice_data_buffers[ voice->assigned_voice_index ], 0, sample_pairs_count * 2 * sizeof( float ) );
				return 1;
				}
			}
		}
	voice->initialized = 1;
		
	// copy samples from resource, handle looping
	int position = voice->position;
	float* out = audiosys->active_voice_data_buffers[ voice->assigned_voice_index ];
    int count_written = voice->source.sample_pairs( voice->source.instance, position, out, sample_pairs_count );
	out += count_written * 2;
	while( count_written < sample_pairs_count )
		{
		if( voice->loop )
			{
			position = 0;
            count_written += voice->source.sample_pairs( voice->source.instance, position, out, sample_pairs_count );
			out += count_written * 2;
			position += count_written;
			}
		else
			{
			for( int i = count_written * 2; i < sample_pairs_count * 2; ++i ) 
                audiosys->active_voice_data_buffers[ voice->assigned_voice_index ][ i ] = 0.0f;	
			count_written = sample_pairs_count;
            finished = 1;
			}
		}
		
	// fading	
	float fade_volume = 1.0f;
	float fade_delta = 0.0f;
	if( voice->state == AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT && voice->fade_out_time > 0.0f )
		{
		voice->fade_progress -= delta_time / voice->fade_out_time;
		if( voice->fade_progress <= 0.0f ) 
			{
			voice->fade_progress = 0.0f;
            if( voice->source.release ) voice->source.release( voice->source.instance );
			voice->source.instance = NULL;
            voice->source.length_in_sample_pairs = NULL;
            voice->source.release = NULL;
            voice->source.sample_pairs = NULL;
			voice->state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			fade_volume = 0.0f;
			}
		else
			{
			float volume_dec_per_second = 1.0f / voice->fade_out_time;
			float volume_dec_per_sample = volume_dec_per_second / 44100.0f;
			fade_volume = voice->fade_progress;
			fade_volume = fade_volume < 0.0f ? 0.0f : fade_volume > 1.0f ? 1.0f : fade_volume;			
			fade_delta = -volume_dec_per_sample;
			}
		}
	else if( voice->state == AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN && voice->fade_in_time > 0.0f )
		{
		voice->fade_progress += delta_time / voice->fade_in_time;
		if( voice->fade_progress >= 1.0f ) 
			{				
			voice->fade_progress = 1.0f;
			voice->state = AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING;
			fade_volume = 1.0f;
			}
		else
			{
			float volume_inc_per_second = 1.0f / voice->fade_in_time;
			float volume_inc_per_sample = volume_inc_per_second / 44100.0f;
			fade_volume = voice->fade_progress;
			fade_volume = fade_volume < 0.0f ? 0.0f : fade_volume > 1.0f ? 1.0f : fade_volume;			
			fade_delta = volume_inc_per_sample;
			}
		}
			
	// panning and volume
	// TODO: more efficient inner loop - multiple variations based on preconditions
	for( int i = 0; i < sample_pairs_count; ++i )				
		{
		float left = audiosys->active_voice_data_buffers[ voice->assigned_voice_index ][ i * 2 + 0 ];
		float right = audiosys->active_voice_data_buffers[ voice->assigned_voice_index ][ i * 2 + 1 ];
		float m = ( left + right ) / 2.0f;
		left = m;
		right = m;
		if( voice->pan < 0.0f )
			{
			float a = -voice->pan; 
			float ia = 1.0f - a;
			left = left + a * right;
			right = right * ia;
			}
		else if( voice->pan > 0.0f )
			{
			float a = voice->pan;
			float ia = 1.0f - a;
			right = left * a + right;
			left = left * ia;
			}
		left *= voice->volume * fade_volume * audiosys->master_volume;
		right *= voice->volume * fade_volume * audiosys->master_volume;
		audiosys->active_voice_data_buffers[ voice->assigned_voice_index ][ i * 2 + 0 ] = left;
		audiosys->active_voice_data_buffers[ voice->assigned_voice_index ][ i * 2 + 1 ] = right;
		if( fade_delta != 0.0f )
			{
			fade_volume += fade_delta;
			fade_volume = fade_volume < 0.0f ? 0.0f : fade_volume > 1.0f ? 1.0f : fade_volume;			
			}
		}

    return finished;
	}

   
void audiosys_update( audiosys_t* audiosys )
	{
	int remix = 0;
    int sample_pairs_to_advance = audiosys->sample_pairs_to_advance_next_update; // TODO: Protect with mutex
    audiosys->sample_pairs_to_advance_next_update = 0;
		
	// music
	if( audiosys->music.modified || audiosys->modified || sample_pairs_to_advance )
		{
		audiosys->music.modified = 0;
		remix = 1;
        if( audiosys->voice_music_primary >= 0  )
            {
		    if( audiosys->music.source.sample_pairs && !audiosys->music.paused && !audiosys->paused && audiosys->music.state != AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED )
			    {
                audiosys->music.assigned_voice_index = audiosys->voice_music_primary;
			    audiosys_internal_update_voice( audiosys, &audiosys->music, sample_pairs_to_advance, audiosys->buffered_sample_pairs_count );
			    }
		    else
			    memset( audiosys->active_voice_data_buffers[ audiosys->voice_music_primary ], 0, 
                    audiosys->buffered_sample_pairs_count * 2 * sizeof( float ) );			
            }
		}
			
	// music_crossfade
	if( audiosys->music_crossfade.modified || audiosys->modified || sample_pairs_to_advance )
		{
		audiosys->music_crossfade.modified = 0;
		remix = 1;
        if( audiosys->voice_music_crossfade >= 0  )
            {
		    if( audiosys->music_crossfade.source.sample_pairs && !audiosys->music_crossfade.paused && !audiosys->paused && audiosys->music_crossfade.state != AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED )
			    {
                audiosys->music_crossfade.assigned_voice_index = audiosys->voice_music_crossfade;
			    int finished = audiosys_internal_update_voice( audiosys, &audiosys->music_crossfade, sample_pairs_to_advance, audiosys->buffered_sample_pairs_count );
			    if( finished && audiosys->music.state == AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED ) 
				    {
				    audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING;
				    if( audiosys->music.fade_in_time > 0.0f ) audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
				    }
			    }
		    else
			    memset( audiosys->active_voice_data_buffers[ audiosys->voice_music_crossfade ], 0, 
                    audiosys->buffered_sample_pairs_count * 2 * sizeof( float ) );			
            }
		}
			

	// ambience
	if( audiosys->ambience.modified || audiosys->modified || sample_pairs_to_advance )
		{
		audiosys->ambience.modified = 0;
		remix = 1;
        if( audiosys->voice_ambience_primary >= 0  )
            {
		    if( audiosys->ambience.source.sample_pairs && !audiosys->ambience.paused && !audiosys->paused && audiosys->ambience.state != AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED )
			    {
                audiosys->ambience.assigned_voice_index = audiosys->voice_ambience_primary;
			    audiosys_internal_update_voice( audiosys, &audiosys->ambience, sample_pairs_to_advance, audiosys->buffered_sample_pairs_count );
			    }
		    else
			    memset( audiosys->active_voice_data_buffers[ audiosys->voice_ambience_primary ], 0, 
                    audiosys->buffered_sample_pairs_count * 2 * sizeof( float ) );			
            }
		}
			
	// ambience_crossfade
	if( audiosys->ambience_crossfade.modified || audiosys->modified || sample_pairs_to_advance )
		{
		audiosys->ambience_crossfade.modified = 0;
		remix = 1;
        if( audiosys->voice_ambience_crossfade >= 0  )
            {
		    if( audiosys->ambience_crossfade.source.sample_pairs && !audiosys->ambience_crossfade.paused && !audiosys->paused && audiosys->ambience_crossfade.state != AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED )
			    {
                audiosys->ambience_crossfade.assigned_voice_index = audiosys->voice_ambience_crossfade;
			    int finished = audiosys_internal_update_voice( audiosys, &audiosys->ambience_crossfade, sample_pairs_to_advance, audiosys->buffered_sample_pairs_count );
			    if( finished && audiosys->ambience.state == AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED ) 
				    {
				    audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING;
				    if( audiosys->ambience.fade_in_time > 0.0f ) audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
				    }
			    }
		    else
			    memset( audiosys->active_voice_data_buffers[ audiosys->voice_ambience_crossfade ], 0, 
                    audiosys->buffered_sample_pairs_count * 2 * sizeof( float ) );			
            }
		}
			


			
	// sounds effects
		
	// update mixer with most high priority sounds
	int last_updated_voice_index = audiosys->voice_sounds_start - 1;
	for( int i = 0; i < audiosys->sounds_count; ++i )
		{
		int voice_index = audiosys->voice_sounds_start + i;
		if( voice_index >= audiosys->active_voice_count ) break;
		last_updated_voice_index = voice_index;
		audiosys_internal_voice_t* voice = &audiosys->sounds[ audiosys->sorted_sounds[ i ] ];
		if( voice->modified || audiosys->modified || sample_pairs_to_advance || voice->assigned_voice_index != voice_index )
			{
			voice->modified = 0;
			voice->assigned_voice_index = voice_index;
			remix = 1;
			audiosys_internal_update_voice( audiosys, voice, sample_pairs_to_advance, audiosys->buffered_sample_pairs_count );
			}
		}		
			
	// zero out mixer voices which are no longer in use
	for( int i = last_updated_voice_index + 1; i <= audiosys->previous_update_last_updated_voice_index; ++i )
		memset( audiosys->active_voice_data_buffers[ i ], 0, audiosys->buffered_sample_pairs_count * 2 * sizeof( float ) );			
	audiosys->previous_update_last_updated_voice_index = last_updated_voice_index;

	// advance sound effects which have no assigned mixer voice
	for( int i = last_updated_voice_index + 1; i < audiosys->sounds_count; ++i )
		{
		audiosys_internal_voice_t* voice = &audiosys->sounds[ audiosys->sorted_sounds[ i ] ];
		int voice_length = voice->source.length_in_sample_pairs( voice->source.instance );
		if( voice->initialized ) 
			{
			if( voice->position < voice_length ) 
				voice->position += sample_pairs_to_advance;
					
			while( voice->position >= voice_length )
				{
				if( voice->loop ) 
					voice->position -= voice_length;
				else
                    {
                    if( voice->source.release ) voice->source.release( voice->source.instance );
				    voice->source.instance = NULL;
                    voice->source.length_in_sample_pairs = NULL;
                    voice->source.release = NULL;
                    voice->source.sample_pairs = NULL;
					break;
                    }
				}
			}
		voice->initialized = 1;		
		}

	// remove sounds which have finished playing
	for( int i = audiosys->sounds_count - 1; i >= 0; --i )
		{
		audiosys_internal_voice_t* voice = &audiosys->sounds[ i ];
		if( voice->source.sample_pairs == NULL ) audiosys_internal_remove_sound( audiosys, voice->handle );
		}



	// run mixer to output buffer
    // TODO: separate summarization buffer and S16 buffer, for more efficient updating
	if( remix )
		{
        for( int i = 0; i < audiosys->buffered_sample_pairs_count * 2; ++i )
            {
            float s = 0.0f;
            for( int j = 0; j < audiosys->active_voice_count; ++j ) s += audiosys->active_voice_data_buffers[ j ][ i ];
            s *= audiosys->gain;
            s /= ( audiosys->active_voice_count / 8 );
            s = s < -1.0f ? -2.0f / 3.0f : s > 1.0f ? 2.0f / 3.0f : s - ( s * s * s ) / 3; // soft clip // TODO: make optional?
            s *= 32000.0f;
            audiosys->mix_buffer[ i ] = (AUDIOSYS_S16)( s );
            }
		}

    audiosys->modified = 0;
	}


int audiosys_consume( audiosys_t* audiosys, int sample_pairs_to_advance, AUDIOSYS_S16* output_sample_pairs, int output_sample_pairs_count )
    {
    // TODO: protect by mutex
    audiosys->sample_pairs_to_advance_next_update = sample_pairs_to_advance;
    int count = output_sample_pairs_count <= audiosys->buffered_sample_pairs_count ? 
        output_sample_pairs_count : audiosys->buffered_sample_pairs_count;
    memcpy( output_sample_pairs, audiosys->mix_buffer, count * 2 * sizeof( AUDIOSYS_S16 ) );
    return count;
    }


void audiosys_master_volume_set( audiosys_t* audiosys, float volume )
	{
    audiosys->master_volume = volume;
    audiosys->modified = 1;
	}


float audiosys_master_volume( audiosys_t* audiosys )
	{
    return audiosys->master_volume;
	}


void audiosys_gain_set( audiosys_t* audiosys, float gain )
	{
    audiosys->gain = gain;
    audiosys->modified = 1;
	}


float audiosys_gain( audiosys_t* audiosys )
	{
    return audiosys->gain;
	}


void audiosys_pause( audiosys_t* audiosys )
	{
    audiosys->paused = 1;
    audiosys->modified = 1;
	}


void audiosys_resume( audiosys_t* audiosys )
	{
    audiosys->paused = 0;
    audiosys->modified = 1;
	}


audiosys_paused_t audiosys_paused( audiosys_t* audiosys )
	{
    return audiosys->paused ? AUDIOSYS_PAUSED : AUDIOSYS_NOT_PAUSED;
	}


static void audiosys_internal_init_voice( audiosys_internal_voice_t* voice, audiosys_audio_source_t source )
	{
	voice->handle = 0;
	voice->modified = 1;
	voice->initialized = 0;
	voice->paused = 0;
	voice->state = AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING;
	voice->source = source;
	voice->position = 0;
	voice->loop = 0;
	voice->volume = 1.0f;
	voice->pan = 0.0f;
	voice->fade_in_time = 0.0f;
	voice->fade_out_time = 0.0f;
	voice->priority = 0.0f;
	voice->fade_progress = 1.0f;
	voice->assigned_voice_index = -1;
	}

#pragma warning( push ) // TODO: remove
#pragma warning( disable: 4100 ) // TODO: remove

void audiosys_music_play( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_in_time )
	{
	audiosys_internal_init_voice( &audiosys->music, source );
	audiosys->music.fade_in_time = fade_in_time;
	if( fade_in_time > 0.0f ) 
		{
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
		audiosys->music.fade_progress = 0.0f;
		}
	}


void audiosys_music_stop( audiosys_t* audiosys, float fade_out_time )
	{
	audiosys->music.modified = 1;
	if( fade_out_time > 0.0f ) 
		{
		audiosys->music.fade_out_time = fade_out_time;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		}
	else
		{
        if( audiosys->music.source.release ) audiosys->music.source.release( audiosys->music.source.instance );
		audiosys->music.source.instance = NULL;
        audiosys->music.source.length_in_sample_pairs = NULL;
        audiosys->music.source.release = NULL;
        audiosys->music.source.sample_pairs = NULL;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
		}
	}


void audiosys_music_pause( audiosys_t* audiosys )
	{
    if( audiosys->music.paused == 1 ) return;
    audiosys->music.modified = 1;
    audiosys->music.paused = 1;
	}


void audiosys_music_resume( audiosys_t* audiosys )
	{
    if( audiosys->music.paused == 0 ) return;
    audiosys->music.modified = 1;
    audiosys->music.paused = 0;
	}


void audiosys_music_switch( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_out_time, float fade_in_time )
	{
	audiosys->music.modified = 1;
	if( fade_out_time > 0.0f ) 
		{
		audiosys->music.fade_out_time = fade_out_time;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		}
	else
		{
        if( audiosys->music.source.release ) audiosys->music.source.release( audiosys->music.source.instance );
		audiosys->music.source.instance = NULL;
        audiosys->music.source.length_in_sample_pairs = NULL;
        audiosys->music.source.release = NULL;
        audiosys->music.source.sample_pairs = NULL;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
		audiosys_music_play( audiosys, source, fade_in_time );
		return;
		}
    audiosys_internal_voice_t temp = audiosys->music;
    audiosys->music = audiosys->music_crossfade;
    audiosys->music_crossfade = temp;

    audiosys_internal_init_voice( &audiosys->music, source );
	audiosys->music.fade_in_time = fade_in_time;
	if( fade_in_time > 0.0f ) audiosys->music.fade_progress = 0.0f;
	audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED;
	}


void audiosys_music_cross_fade( audiosys_t* audiosys, audiosys_audio_source_t source, float cross_fade_time )
	{
	audiosys->music.modified = 1;
	if( cross_fade_time > 0.0f ) 
		{
		audiosys->music.fade_out_time = cross_fade_time;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		}
	else
		{
        if( audiosys->music.source.release ) audiosys->music.source.release( audiosys->music.source.instance );
		audiosys->music.source.instance = NULL;
        audiosys->music.source.length_in_sample_pairs = NULL;
        audiosys->music.source.release = NULL;
        audiosys->music.source.sample_pairs = NULL;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
		}
    audiosys_internal_voice_t temp = audiosys->music;
    audiosys->music = audiosys->music_crossfade;
    audiosys->music_crossfade = temp;

    audiosys_internal_init_voice( &audiosys->music, source );
	audiosys->music.fade_in_time = cross_fade_time;
	if( cross_fade_time > 0.0f ) 
		{
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
		audiosys->music.fade_progress = 0.0f;
		}
	}


void audiosys_music_position_set( audiosys_t* audiosys, float position )
	{
    // TODO: implement
	}


float audiosys_music_position( audiosys_t* audiosys )
	{
    // TODO: implement
    return 0.0f;
    }


audiosys_audio_source_t audiosys_music_source( audiosys_t* audiosys )
	{
    return audiosys->music.source;
	}


void audiosys_music_loop_set( audiosys_t* audiosys, audiosys_loop_t loop )
    {
    audiosys->music.modified = 1;
	audiosys->music.loop = loop == AUDIOSYS_LOOP_ON ? 1 : 0;	
	}


audiosys_loop_t audiosys_music_loop( audiosys_t* audiosys )
	{
    return audiosys->music.loop ? AUDIOSYS_LOOP_ON : AUDIOSYS_LOOP_OFF;
	}


void audiosys_music_volume_set( audiosys_t* audiosys, float volume )
	{
	audiosys->music.modified = 1;
	audiosys->music.volume = volume < 0.0f ? 0.0f : volume > 1.0f ? 1.0f : volume;
	}


float audiosys_music_volume( audiosys_t* audiosys )
	{
    return audiosys->music.volume;
    }


void audiosys_music_pan_set( audiosys_t* audiosys, float pan )
	{
	audiosys->music.modified = 1;
	audiosys->music.pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
	}


float audiosys_music_pan( audiosys_t* audiosys )
	{
    return audiosys->music.pan;
	}


void audiosys_ambience_play( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_in_time )
	{
	}


void audiosys_ambience_stop( audiosys_t* audiosys, float fade_out_time )
	{
	}


void audiosys_ambience_pause( audiosys_t* audiosys )
	{
	}


void audiosys_ambience_resume( audiosys_t* audiosys )
	{
	}


void audiosys_ambience_switch( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_out_time, float fade_in_time )
	{
	}


void audiosys_ambience_cross_fade( audiosys_t* audiosys, audiosys_audio_source_t source, float cross_fade_time )
	{
	}


void audiosys_ambience_position_set( audiosys_t* audiosys, float position )
	{
	}


float audiosys_ambience_position( audiosys_t* audiosys )
	{
    return 0;
	}


audiosys_audio_source_t audiosys_ambience_source( audiosys_t* audiosys )
	{
    audiosys_audio_source_t source;
    memset( &source, 0, sizeof( source ) );
    return source;
	}


void audiosys_ambience_loop_set( audiosys_t* audiosys, audiosys_loop_t loop )
	{
	}


audiosys_loop_t audiosys_ambience_loop( audiosys_t* audiosys )
	{
    return audiosys->ambience.loop ? AUDIOSYS_LOOP_ON : AUDIOSYS_LOOP_OFF;
	}


void audiosys_ambience_volume_set( audiosys_t* audiosys, float volume )
	{
	}


float audiosys_ambience_volume( audiosys_t* audiosys )
	{
    return audiosys->ambience.volume;
	}


void audiosys_ambience_pan_set( audiosys_t* audiosys, float pan )
	{
	}


float audiosys_ambience_pan( audiosys_t* audiosys )
	{
    return audiosys->ambience.pan;
	}


static audiosys_internal_voice_t* audiosys_internal_get_sound( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
	int index = handles_index( &audiosys->sounds_handles, handles_from_u64( &audiosys->sounds_handles, handle ) );
	if( index < 0 ) return 0;
	return &audiosys->sounds[ index ];
	}


AUDIOSYS_U64 audiosys_sound_play( audiosys_t* audiosys, audiosys_audio_source_t source, float priority, float fade_in_time )
	{
	audiosys_internal_voice_t* sound;
	AUDIOSYS_U64 handle = audiosys_internal_add_sound( audiosys, &sound, priority ); // TODO: priority sort
	audiosys_internal_init_voice( sound, source );
	sound->handle = handle;
	sound->priority = priority;
	sound->fade_in_time = fade_in_time;
	if( fade_in_time > 0.0f ) 
		{
		sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
		sound->fade_progress = 0.0f;
		}
	return handle;
    }


void audiosys_sound_stop( audiosys_t* audiosys, AUDIOSYS_U64 handle, float fade_out_time )
	{
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) return;

	sound->modified = 1;
	if( fade_out_time > 0.0f ) 
		{
		sound->fade_out_time = fade_out_time;
		sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		}
	else
		{
		sound->source.instance = NULL;
        sound->source.length_in_sample_pairs = NULL;
        sound->source.release = NULL;
        sound->source.sample_pairs = NULL;
		sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
		}
	}


void audiosys_sound_pause( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
	}


void audiosys_sound_resume( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
	}


void audiosys_sound_position_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float position )
	{
	}


float audiosys_sound_position( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
    return 0;
	}


audiosys_audio_source_t audiosys_sound_source( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
    audiosys_audio_source_t source;
    memset( &source, 0, sizeof( source ) );
    return source;
	}


void audiosys_sound_loop_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, audiosys_loop_t loop )
	{
	}


audiosys_loop_t audiosys_sound_loop( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
    return AUDIOSYS_LOOP_OFF;
	}


void audiosys_sound_volume_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float volume )
	{
	}


float audiosys_sound_volume( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
    return 0;
	}


void audiosys_sound_pan_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float pan )
	{
	}


float audiosys_sound_pan( audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
    return 0;
	}


audiosys_sound_valid_t audiosys_sound_valid(audiosys_t* audiosys, AUDIOSYS_U64 handle )
	{
	return AUDIOSYS_SOUND_INVALID;
	}


#pragma warning( pop ) // TODO: remove

                                                                #if 0

                                                                int const soundstream_sample_pairs_count = 735 * 3;
                                                                int const sound_chunks_count = 10;
                                                                int const audio_voices_count = 16; // TODO: configurable via define
                                                                int const audio_channels_count = 2;

                                                                struct audio_manager final
	                                                                {
	                                                                enum state_t
		                                                                {
		                                                                AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED,
		                                                                AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING,
		                                                                AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN,
		                                                                AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT,
		                                                                AUDIOSYS_INTERNAL_VOICE_STATE_CROSSFADING,
		                                                                AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED,
		                                                                };
		
	                                                                struct audiosys_internal_voice_t
		                                                                {
		                                                                AUDIOSYS_U64 handle;
		                                                                bool modified;
		                                                                bool initialized;
		                                                                bool paused;
		                                                                state_t state;
		                                                                resource<audio> audio_resource;
		                                                                int position;
		                                                                bool loop;
		                                                                float volume;
		                                                                float pan;
		                                                                float fade_in_time;
		                                                                float fade_out_time;
		                                                                float priority;
		                                                                float fade_progress;
		                                                                int assigned_index;
		                                                                };
		
	                                                                float master_volume;
	                                                                float previous_master_volume;
	                                                                bool paused;
	                                                                bool previous_paused;
		
	                                                                audiosys_internal_voice_t music;
	                                                                audiosys_internal_voice_t music_crossfade;
	                                                                float music_crossfade_time;
	
	                                                                audiosys_internal_voice_t ambience;
	                                                                audiosys_internal_voice_t ambience_crossfade;
	                                                                float ambience_crossfade_time;
	
	                                                                array<audiosys_internal_voice_t> sounds;
	                                                                pod_array<audiosys_internal_voice_t*> sorted_sounds;
	                                                                handles_t sounds_handles;
	
	                                                                float scratch_mem[ sound_chunks_count * soundstream_sample_pairs_count * 2  ];

	                                                                int voice_music_primary;
	                                                                int voice_music_crossfade;
	                                                                int voice_ambience_primary;
	                                                                int voice_ambience_crossfade;
	                                                                int voice_sounds_start;
	
	                                                                int previous_update_last_updated_voice_index;
	
	                                                                audio_manager( void* memctx ) :
		                                                                master_volume( 1.0f ),
		                                                                previous_master_volume( 0.0f ),
		                                                                paused( false ),
		                                                                previous_paused( true ),
		                                                                sounds( 256 ),
		                                                                voice_music_primary( 0 ),
		                                                                voice_music_crossfade( 1 ),
		                                                                voice_ambience_primary( 2 ),
		                                                                voice_ambience_crossfade( 3 ),
		                                                                voice_sounds_start( 4 ),
		                                                                previous_update_last_updated_voice_index( 0 )
		                                                                {
		                                                                handles_init( &sounds_handles, 256, memctx );	
		                                                                }

		
	                                                                ~audio_manager()
		                                                                {
		                                                                handles_term( &sounds_handles );	
		                                                                }
		

	                                                                void pause_audio()
		                                                                {
		                                                                paused = true;
		                                                                }
	

	                                                                void resume_audio()
		                                                                {
		                                                                paused = false;
		                                                                }


	                                                                void init_voice( audiosys_internal_voice_t* voice, resource<audio> const& audio_resource, bool loop, float volume, float pan )
		                                                                {
		                                                                voice->handle = 0;
		                                                                voice->modified = true;
		                                                                voice->initialized = false;
		                                                                voice->paused = false;
		                                                                voice->state = AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING;
		                                                                voice->audio_resource = audio_resource;
		                                                                voice->position = 0;
		                                                                voice->loop = loop;
		                                                                voice->volume = volume < 0.0f ? 0.0f : volume > 1.0f ? 1.0f : volume;
		                                                                voice->pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
		                                                                voice->fade_in_time = 0.0f;
		                                                                voice->fade_out_time = 0.0f;
		                                                                voice->priority = 0.0f;
		                                                                voice->fade_progress = 1.0f;
		                                                                voice->assigned_index = -1;
		                                                                }


	                                                                void play_music( resource<audio> const& audio_resource, float fade_in_time, bool loop, float volume, float pan )
		                                                                {
		                                                                init_voice( &music, audio_resource, loop, volume, pan );
		                                                                music.fade_in_time = fade_in_time;
		                                                                if( fade_in_time > 0.0f ) 
			                                                                {
			                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
			                                                                music.fade_progress = 0.0f;
			                                                                }
		                                                                }
		
	                                                                void stop_music( float fade_out_time )
		                                                                {
		                                                                music.modified = true;
		                                                                if( fade_out_time > 0.0f ) 
			                                                                {
			                                                                music.fade_out_time = fade_out_time;
			                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
			                                                                }
		                                                                else
			                                                                {
			                                                                music.audio_resource = resource<audio>();
			                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			                                                                }
		                                                                }

	                                                                void pause_music()
		                                                                {
		                                                                music.modified = true;
		                                                                music.paused = true;
		                                                                }

	                                                                void resume_music()
		                                                                {
		                                                                music.modified = true;
		                                                                music.paused = false;
		                                                                }

	                                                                void switch_music( resource<audio> const& audio_resource, float fade_out_time, float fade_in_time, bool loop, float volume, float pan )
		                                                                {
		                                                                music.modified = true;
		                                                                if( fade_out_time > 0.0f ) 
			                                                                {
			                                                                music.fade_out_time = fade_out_time;
			                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
			                                                                }
		                                                                else
			                                                                {
			                                                                music.audio_resource = resource<audio>();
			                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			                                                                play_music( audio_resource, fade_in_time, loop, volume, pan );
			                                                                return;
			                                                                }
		                                                                swap( &music, &music_crossfade );
		                                                                init_voice( &music, audio_resource, loop, volume, pan );
		                                                                music.fade_in_time = fade_in_time;
		                                                                if( fade_in_time > 0.0f ) music.fade_progress = 0.0f;
		                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED;
		                                                                }

	                                                                void cross_fade_music( resource<audio> const& audio_resource, float cross_fade_time, bool loop, float volume, float pan )
		                                                                {
		                                                                music.modified = true;
		                                                                if( cross_fade_time > 0.0f ) 
			                                                                {
			                                                                music.fade_out_time = cross_fade_time;
			                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
			                                                                }
		                                                                else
			                                                                {
			                                                                music.audio_resource = resource<audio>();
			                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			                                                                }
		                                                                swap( &music, &music_crossfade );
		                                                                init_voice( &music, audio_resource, loop, volume, pan );
		                                                                music.fade_in_time = cross_fade_time;
		                                                                if( cross_fade_time > 0.0f ) 
			                                                                {
			                                                                music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
			                                                                music.fade_progress = 0.0f;
			                                                                }
		                                                                }

	                                                                void music_position( float position )
		                                                                {
		                                                                if( !music.audio_resource ) return;
		                                                                music.modified = true;
		                                                                music.position = music.audio_resource->seek( position );
		                                                                }

	                                                                float music_position()
		                                                                {
		                                                                if( !music.audio_resource ) return 0.0f;
		                                                                return music.audio_resource->position( music.position );
		                                                                }

	                                                                resource<audio> const& current_music()
		                                                                {
		                                                                return music.audio_resource;
		                                                                }

	                                                                void music_loop( bool loop )
		                                                                {
		                                                                music.modified = true;
		                                                                music.loop = loop;
		                                                                }

	                                                                bool music_loop()
		                                                                {
		                                                                return music.loop;
		                                                                }

	                                                                void music_volume( float volume )
		                                                                {
		                                                                music.modified = true;
		                                                                music.volume = volume < 0.0f ? 0.0f : volume > 1.0f ? 1.0f : volume;
		                                                                }

	                                                                float music_volume()
		                                                                {
		                                                                return music.volume;
		                                                                }

	                                                                void music_pan( float pan )
		                                                                {
		                                                                music.modified = true;
		                                                                music.pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
		                                                                }

	                                                                float music_pan()
		                                                                {
		                                                                return music.pan;
		                                                                }


	                                                                void play_ambience( resource<audio> const& audio_resource, float fade_in_time, bool loop, float volume, float pan )
		                                                                {
		                                                                init_voice( &ambience, audio_resource, loop, volume, pan );
		                                                                ambience.fade_in_time = fade_in_time;
		                                                                if( fade_in_time > 0.0f ) 
			                                                                {
			                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
			                                                                ambience.fade_progress = 0.0f;
			                                                                }
		                                                                }
		
	                                                                void stop_ambience( float fade_out_time )
		                                                                {
		                                                                ambience.modified = true;
		                                                                if( fade_out_time > 0.0f ) 
			                                                                {
			                                                                ambience.fade_out_time = fade_out_time;
			                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
			                                                                }
		                                                                else
			                                                                {
			                                                                ambience.audio_resource = resource<audio>();
			                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			                                                                }
		                                                                }

	                                                                void pause_ambience()
		                                                                {
		                                                                ambience.modified = true;
		                                                                ambience.paused = true;
		                                                                }

	                                                                void resume_ambience()
		                                                                {
		                                                                ambience.modified = true;
		                                                                ambience.paused = false;
		                                                                }

	                                                                void switch_ambience( resource<audio> const& audio_resource, float fade_out_time, float fade_in_time, bool loop, float volume, float pan )
		                                                                {
		                                                                ambience.modified = true;
		                                                                if( fade_out_time > 0.0f ) 
			                                                                {
			                                                                ambience.fade_out_time = fade_out_time;
			                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
			                                                                }
		                                                                else
			                                                                {
			                                                                ambience.audio_resource = resource<audio>();
			                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			                                                                play_ambience( audio_resource, fade_in_time, loop, volume, pan );
			                                                                return;
			                                                                }
		                                                                swap( &ambience, &ambience_crossfade );
		                                                                init_voice( &ambience, audio_resource, loop, volume, pan );
		                                                                ambience.fade_in_time = fade_in_time;
		                                                                if( fade_in_time > 0.0f ) ambience.fade_progress = 0.0f;
		                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED;
		                                                                }

	                                                                void cross_fade_ambience( resource<audio> const& audio_resource, float cross_fade_time, bool loop, float volume, float pan )
		                                                                {
		                                                                ambience.modified = true;
		                                                                if( cross_fade_time > 0.0f ) 
			                                                                {
			                                                                ambience.fade_out_time = cross_fade_time;
			                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
			                                                                }
		                                                                else
			                                                                {
			                                                                ambience.audio_resource = resource<audio>();
			                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			                                                                }
		                                                                swap( &ambience, &ambience_crossfade );
		                                                                init_voice( &ambience, audio_resource, loop, volume, pan );
		                                                                ambience.fade_in_time = cross_fade_time;
		                                                                if( cross_fade_time > 0.0f ) 
			                                                                {
			                                                                ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
			                                                                ambience.fade_progress = 0.0f;
			                                                                }
		                                                                }

	                                                                void ambience_position( float position )
		                                                                {
		                                                                if( !ambience.audio_resource ) return;
		                                                                ambience.modified = true;
		                                                                ambience.position = ambience.audio_resource->seek( position );
		                                                                }

	                                                                float ambience_position()
		                                                                {
		                                                                if( !ambience.audio_resource ) return 0.0f;
		                                                                return ambience.audio_resource->position( ambience.position );
		                                                                }

	                                                                resource<audio> const& current_ambience()
		                                                                {
		                                                                return ambience.audio_resource;
		                                                                }

	                                                                void ambience_loop( bool loop )
		                                                                {
		                                                                ambience.modified = true;
		                                                                ambience.loop = loop;
		                                                                }

	                                                                bool ambience_loop()
		                                                                {
		                                                                return ambience.loop;
		                                                                }

	                                                                void ambience_volume( float volume )
		                                                                {
		                                                                ambience.modified = true;
		                                                                ambience.volume = volume < 0.0f ? 0.0f : volume > 1.0f ? 1.0f : volume;
		                                                                }

	                                                                float ambience_volume()
		                                                                {
		                                                                return ambience.volume;
		                                                                }

	                                                                void ambience_pan( float pan )
		                                                                {
		                                                                ambience.modified = true;
		                                                                ambience.pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
		                                                                }

	                                                                float ambience_pan()
		                                                                {
		                                                                return ambience.pan;
		                                                                }


	                                                                AUDIOSYS_U64 add_sound( audiosys_internal_voice_t** sound )
		                                                                {
		                                                                int index = sounds.count();
		                                                                *sound = &sounds.add();
		                                                                TWEEN_U64 handle = handles_to_u64( &sounds_handles, handles_alloc( &sounds_handles, index ) );
		                                                                return handle;
		                                                                }
		

	                                                                void remove_sound( AUDIOSYS_U64 handle )
		                                                                {
		                                                                int index = handles_index( &sounds_handles, handles_from_u64( &sounds_handles, handle ) );
		                                                                if( index < 0 ) return;
		
		                                                                handles_release( &sounds_handles, handles_from_u64( &sounds_handles, sounds[ index ].handle ) );
		                                                                if( index != sounds.count() - 1 )
			                                                                {
			                                                                sounds[ index ] = sounds[ sounds.count() - 1 ];
			                                                                sounds.remove( sounds.count() - 1 );
			                                                                handles_update( &sounds_handles, handles_from_u64( &sounds_handles, sounds[ index ].handle ), index );
			                                                                }		
		                                                                else
			                                                                {
			                                                                sounds.remove( sounds.count() - 1 );
			                                                                }
		                                                                }
		
	                                                                audiosys_internal_voice_t* get_sound( AUDIOSYS_U64 handle )
		                                                                {
		                                                                int index = handles_index( &sounds_handles, handles_from_u64( &sounds_handles, handle ) );
		                                                                if( index < 0 ) return 0;
		                                                                return &sounds[ index ];
		                                                                }

	                                                                AUDIOSYS_U64 play_sound( resource<audio> const& audio_resource, float priority, float fade_in_time, bool loop, float volume, float pan )
		                                                                {
		                                                                audiosys_internal_voice_t* sound;
		                                                                AUDIOSYS_U64 handle = add_sound( &sound );
		                                                                init_voice( sound, audio_resource, loop, volume, pan );
		                                                                sound->handle = handle;
		                                                                sound->priority = priority;
		                                                                sound->fade_in_time = fade_in_time;
		                                                                if( fade_in_time > 0.0f ) 
			                                                                {
			                                                                sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
			                                                                sound->fade_progress = 0.0f;
			                                                                }
		                                                                return handle;
		                                                                }
		
	                                                                void stop_sound( AUDIOSYS_U64 handle, float fade_out_time )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return;
		
		                                                                sound->modified = true;
		                                                                if( fade_out_time > 0.0f ) 
			                                                                {
			                                                                sound->fade_out_time = fade_out_time;
			                                                                sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
			                                                                }
		                                                                else
			                                                                {
			                                                                sound->audio_resource = resource<audio>();
			                                                                sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			                                                                }
		                                                                }

	                                                                void pause_sound( AUDIOSYS_U64 handle )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return;

		                                                                sound->modified = true;
		                                                                sound->paused = true;
		                                                                }

	                                                                void resume_sound( AUDIOSYS_U64 handle )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return;

		                                                                sound->modified = true;
		                                                                sound->paused = false;
		                                                                }

	                                                                void sound_position( AUDIOSYS_U64 handle, float position )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return;

		                                                                if( !sound->audio_resource ) return;
		                                                                sound->modified = true;
		                                                                sound->position = sound->audio_resource->seek( position );
		                                                                }

	                                                                float sound_position( AUDIOSYS_U64 handle )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return 0.0f;

		                                                                if( !sound->audio_resource ) return 0.0f;
		                                                                return sound->audio_resource->position( sound->position );
		                                                                }

	                                                                resource<audio> sound_audio_resource( AUDIOSYS_U64 handle )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return resource<audio>();

		                                                                return sound->audio_resource;
		                                                                }

	                                                                void sound_loop( AUDIOSYS_U64 handle, bool loop )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return;

		                                                                sound->modified = true;
		                                                                sound->loop = loop;
		                                                                }

	                                                                bool sound_loop( AUDIOSYS_U64 handle )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return false;

		                                                                return sound->loop;
		                                                                }

	                                                                void sound_volume( AUDIOSYS_U64 handle, float volume )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return;

		                                                                sound->modified = true;
		                                                                sound->volume = volume < 0.0f ? 0.0f : volume > 1.0f ? 1.0f : volume;
		                                                                }

	                                                                float sound_volume( AUDIOSYS_U64 handle )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return 0.0f;

		                                                                return sound->volume;
		                                                                }

	                                                                void sound_pan( AUDIOSYS_U64 handle, float pan )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return;

		                                                                sound->modified = true;
		                                                                sound->pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
		                                                                }

	                                                                float sound_pan( AUDIOSYS_U64 handle )
		                                                                {
		                                                                audiosys_internal_voice_t* sound = get_sound( handle );
		                                                                if( !sound ) return 0.0f;

		                                                                return sound->pan;
		                                                                }


		
	                                                                static int compare_priority( audiosys_internal_voice_t* const& a, audiosys_internal_voice_t* const& b )
		                                                                {
		                                                                return a->priority > b->priority ? -1 : a->priority < b->priority ? 1 : 0;
		                                                                }
		

	                                                                void update( int played_chunks_count, i16* output_sample_pairs, mixer_t* mixer, thread_mutex_t* mutex )
		                                                                {
		                                                                }
	                                                                };
                                                                #endif

#endif /* AUDIOSYS_IMPLEMENTATION */

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2017 Mattias Gustavsson

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
