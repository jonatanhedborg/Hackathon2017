#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS

#include "libs/app.h"
#include "libs/array.hpp"
#include "libs/assetsys.h"
#include "libs/audiosys.h"
#include "libs/ease.h"
#include "libs/frametimer.h"
#include "libs/gamestate.hpp"
#include "libs/graph.hpp"
#include "libs/math_util.hpp"
#include "libs/objrepo.hpp"
#include "libs/paldither.h"
#include "libs/palettize.h"
#include "libs/resources.hpp"
#include "libs/rnd.h"
#include "libs/sort.hpp"
#include "libs/strpool.h"
#include "libs/strpool_util.hpp"
#include "libs/sysfont.h"
#include "libs/thread.h"
#include "libs/tween.hpp"
#include "libs/vecmath.hpp"

#include "libs/tinyobj_loader_c.h"
#include "libs/stb_image.h"

#include "tobii/tobii.h"
#include "tobii/tobii_streams.h"
#pragma comment( lib, "tobii/tobii_stream_engine.lib" )

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <stdio.h>

#include "model3d.hpp"
#include "sound.hpp"


// additional vecmath helpers
namespace vecmath { 
inline float4x4 look_at_lh( float3 eye, float3 at, float3 up ) { float3 at_vec = normalize( at - eye ); float3 right_vec = normalize( cross( up, at_vec ) ); float3 up_vec = cross( at_vec, right_vec ); return float4x4( float4( right_vec, 0.0f ), float4( up_vec, 0.0f ), float4( at_vec, 0.0f ), float4( eye, 1.0f ) ); }
inline float4x4 perspective_lh( float w, float h, float zn, float zf ) { return float4x4( w, 0.0f, 0.0f, 0.0f, 0.0f, h,	0.0f, 0.0f, 0.0f, 0.0f, zf / ( zn - zf ), -1.0f, 0.0f, 0.0f, zn * zf / ( zn - zf ), 0.0f ); }
inline float4x4 perspective_fov_lh( float fovy, float aspect, float zn, float zf ) { float h = 1.0f / tanf( fovy * 0.5f ); float w = h / aspect; return float4x4( w, 0.0f, 0.0f, 0.0f, 0.0f, h,	0.0f, 0.0f, 0.0f, 0.0f, zf / ( zn - zf ), -1.0f, 0.0f, 0.0f, zn * zf / ( zn - zf ), 0.0f ); }
inline float4x4 rotation_axis( float3 axis, float angle ) { float s = sin( angle );  float c = cos( angle ); float ic = 1.0f - c; float xyic = (axis.x * axis.y) * ic; float xzic = (axis.x * axis.z) * ic; float yzic = (axis.y * axis.z) * ic; float xs = axis.x * s; float ys = axis.y * s; float zs = axis.z * s; return float4x4( c + ( ( axis.x * axis.x ) * ic ), xyic + zs, xzic - ys, 0.0f, xyic - zs, c + ((axis.y * axis.y) * ic ), yzic + xs, 0.0f, xzic + ys, yzic - xs, c + ((axis.z * axis.z) * ic ), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f ); }
inline float4x4 rotation_x( float angle ) { float s = sin( angle ); float c = cos( angle ); return float4x4( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c, s, 0.0f, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f ); }
inline float4x4 rotation_y( float angle ) { float s = sin( angle ); float c = cos( angle ); return float4x4( c, 0.0f, -s, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, s, 0.0f, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f ); }
inline float4x4 rotation_z( float angle ) { float s = sin( angle ); float c = cos( angle ); return float4x4( c, s, 0.0f, 0.0f, -s,	c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f ); }
inline float4x4 rotation_yaw_pitch_roll( float yaw, float pitch, float roll ) { return mul( rotation_y( yaw ), mul( rotation_x( pitch ), rotation_z( roll ) ) ); }
inline float4x4 scaling( float sx, float sy, float sz ) { return float4x4( sx, 0.0f, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 0.0f, sz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f ); }
inline float4x4 translation( float x, float y, float z ) { return float4x4( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, x, y, z, 1.0f ); }
inline float4 transform( float3 v, float4x4 matrix ) { return mul( float4( v, 1.0f ), matrix ); }
inline float3 transform_coord( float3 v, float4x4 matrix ) { return mul( float4( v, 1.0f ), matrix ).xyz(); }
inline float3 transform_normal( float3 v, float4x4 matrix ) { return mul( v, float3x3( matrix.x.xyz(), matrix.y.xyz(), matrix.z.xyz() ) ); }
inline float4 transform( float4 v, float4x4 matrix ) { return mul( v, matrix ); }
} /* namespace vecmath */

using namespace array_ns;
#include "intersection.hpp"

// screen
struct pal_screen
	{
	uint8_t* screen;
	int width;
	int height;

	inline void pset( int x, int y, uint8_t ink )
		{
		if( x >= 0 && y >= 0 && x < width && y < height )
//		if( ( ( ( 0 - x - 1 ) * ( width - x ) ) & ( ( 0 - y - 1 ) * ( height - y ) ) ) < 0 )
			screen[ x + y * width ] = ink;
		}


	inline int pget( int x, int y )
		{
		if( x >= 0 && y >= 0 && x < width && y < height )
//		if( ( ( ( 0 - x - 1 ) * ( width - x ) ) & ( ( 0 - y - 1 ) * ( height - y ) ) ) < 0 )
			return (int)( screen[ x + y * width ] );
		else
			return 0;
		}


	inline void hline( int x, int y, int len, uint8_t ink )
		{
        if( y < 0 || y >= height ) return;
        if( x < 0 ) { len += x; x = 0; }
        if( x + len > width ) len = width - x;

        uint8_t* scr = screen + y * width + x;
        uint8_t* end = scr + len;
		while( scr < end ) *scr++ = ink;
		}

	};


#include "batch_renderer.hpp"

// palette
APP_U32 palette[ 16 ] = 
	{ 	
	0x000000, 0x0000aa, 0x00aa00, 0x00aaaa, 0xaa0000, 0xaa00aa, 0xaa5500, 0xaaaaaa,
	0x555555, 0x5555ff, 0x55ff55, 0x55ffff, 0xff5555, 0xff55ff, 0xffff55, 0xffffff,
	};

	
// tobii
struct tobii_user_presence_t { int64_t timestamp_us; tobii_user_presence_status_t status; };
struct tobii_t
	{
	tobii_gaze_point_t gaze_point;
	tobii_gaze_origin_t gaze_origin;
	tobii_eye_position_normalized_t eye_position;
	tobii_user_presence_t presence;
	tobii_head_pose_t head_pose;
	};


// gamestates
using namespace vecmath;
using objrepo::object_repo;
#include "gamestate_common.hpp"
#include "gamestates.hpp"


// audio thread

struct audio_thread_context_t
    {
    int exit_flag;
    app_t* app;
    audiosys_t* audiosys;
    short* sound_buffer;
    int sound_buffer_sample_pairs_count;
    };


int audio_thread_proc( void* user_data)
    {
    audio_thread_context_t* context = (audio_thread_context_t*) user_data;
    int mid_point = context->sound_buffer_sample_pairs_count / 2;
    int half_size = mid_point;
    int prev_pos = 0;
    while( !context->exit_flag )
        {
        int pos = app_sound_position( context->app );
	    if( prev_pos >= mid_point && pos < mid_point )
            {
            audiosys_consume( context->audiosys, half_size, context->sound_buffer, half_size );
            app_sound_write( context->app, mid_point, half_size, context->sound_buffer );
            }
		else if( prev_pos < mid_point && pos >= mid_point )
            {
            audiosys_consume( context->audiosys, half_size, context->sound_buffer, half_size );
            app_sound_write( context->app, 0, half_size, context->sound_buffer );
            }
		prev_pos = pos;	
        }

    return 0;
    }

	
// find current display helper
app_display_t* app_current_display( app_t* app )
    {
	int win_x = app_window_x( app );
	int win_y = app_window_y( app );
	app_displays_t displays = app_displays( app );
	for( int i = 0; i < displays.count; ++i )
		{
		if( win_x >= displays.displays[ i ].x && win_y >= displays.displays[ i ].y && win_x < displays.displays[ i ].x + displays.displays[ i ].width && win_y < displays.displays[ i ].y + displays.displays[ i ].height )
			return &displays.displays[ i ];
		}
    return 0;
    }


// update thread	

struct update_thread_context_t
    {
    int exit_flag;
    app_t* app;
	audiosys_t* audiosys;
	thread_mutex_t audio_mutex;
	uint8_t* screen;
	thread_mutex_t screen_mutex;
    };


void play_sound(update_thread_context_t* context, audiosys_audio_source_t* source)
{
	thread_mutex_lock(&context->audio_mutex);
	audiosys_sound_play(context->audiosys, *source, 0.0f, 0.0f);
	thread_mutex_unlock(&context->audio_mutex);
}

void play_music(update_thread_context_t* context, audiosys_audio_source_t* source)
{
	thread_mutex_lock(&context->audio_mutex);
	audiosys_music_play(context->audiosys, *source, 0.0f);
	audiosys_music_loop_set(context->audiosys, AUDIOSYS_LOOP_ON);
	thread_mutex_unlock(&context->audio_mutex);
}

int update_thread_proc( void* user_data)
    {
    update_thread_context_t* context = (update_thread_context_t*) user_data;

	// tobii
	tobii_t tobii; memset( &tobii, 0, sizeof( tobii ) );
	tobii_api_t* api;
	tobii_api_create( &api, 0, 0 );
	tobii_device_t* device;
	tobii_device_create( api, 0, &device );
	struct callback_context_t { app_t* app; tobii_t* tobii; } callback_context =
		{ context->app, &tobii };

	tobii_gaze_point_subscribe( device, 
		[]( tobii_gaze_point_t const* gaze_point, void* user_data ) 
			{ 						
			callback_context_t* context = (callback_context_t*) user_data;
			
			if( gaze_point->validity == TOBII_VALIDITY_VALID )
				{
				app_display_t* display = app_current_display( context->app );
				if( !display ) return;
				
				int x = (int)( gaze_point->position_xy[ 0 ] * display->width );
				int y = (int)( gaze_point->position_xy[ 1 ] * display->height );
				void app_translate_mouse( app_t* app, int* x, int* y );
				app_translate_mouse( context->app, &x, &y );
				context->tobii->gaze_point.position_xy[ 0 ] = (float) x;
				context->tobii->gaze_point.position_xy[ 1 ] = (float) y;
				}
			
			context->tobii->gaze_point.timestamp_us = gaze_point->timestamp_us;
			context->tobii->gaze_point.validity = gaze_point->validity;
			}
		, &callback_context );

	tobii_gaze_origin_subscribe( device, 
		[]( tobii_gaze_origin_t const* gaze_origin, void* user_data ) 
			{ 
			callback_context_t* context = (callback_context_t*) user_data;			
			context->tobii->gaze_origin = *gaze_origin;
			}
		, &callback_context );

	tobii_eye_position_normalized_subscribe( device, 
		[]( tobii_eye_position_normalized_t const* eye_position, void* user_data ) 
			{ 
			callback_context_t* context = (callback_context_t*) user_data;			
			context->tobii->eye_position = *eye_position;
			}
		, &callback_context );

	tobii_user_presence_subscribe( device, 
		[]( tobii_user_presence_status_t status, int64_t timestamp_us, void* user_data ) 
			{ 
			callback_context_t* context = (callback_context_t*) user_data;			
			context->tobii->presence.timestamp_us = timestamp_us;
			context->tobii->presence.status = status;
			}
		, &callback_context );

	tobii_head_pose_subscribe( device, 
		[]( tobii_head_pose_t const* head_pose, void* user_data ) 
			{ 
			callback_context_t* context = (callback_context_t*) user_data;			
			context->tobii->head_pose = *head_pose;
			}
		, &callback_context );

		
	// Mount current working folder's "data" folder as a virtual "/data" path
	assetsys_t* assetsys = assetsys_create( 0 );
	assetsys_mount( assetsys, "./data", "/data" );

	// sound test
	audiosys_audio_source_t music;
	load_sound(assetsys, "/data/music.ogg", &music);
	play_music(context, &music);
	
	audiosys_audio_source_t pickup;
	load_sound(assetsys, "/data/pickup.ogg", &pickup);
	play_sound(context, &pickup);		
	
	// obj test
	model_3d suzanne;
	load_model(assetsys, "/data/suzanne.obj", &suzanne);

	//	screens/graphics
	static uint8_t screen[ 320 * 200 ];
	memset( screen, 0, sizeof( screen ) );
	pal_screen scr_pixels = { screen, 320, 200 };
	graph_ns::graph<pal_screen, uint8_t> graph_pixels( &scr_pixels );
	
	// frame time
	frametimer_t* frametimer = frametimer_init( 0 );
	frametimer_lock_rate( frametimer, 60 );

	// "global" systems
	object_repo objrepo;
	gamestate::game_state_system<object_repo> gamestates( &objrepo );
	objrepo.add( &scr_pixels );
	objrepo.add( &graph_pixels );
	objrepo.add( &tobii );
	objrepo.add( &gamestates );
	objrepo.add( frametimer );
	objrepo.add( assetsys );

	// init gamestates
	init_gamestates( &gamestates );
	
	// update loop
    while( !context->exit_flag )
        {
		// clear screen
		memset(screen, 0, sizeof( screen ) );

		// update frame
		float delta_time = frametimer_update( frametimer );
		tobii_process_callbacks( device );
		gamestates.update( delta_time );

		thread_mutex_lock( &context->screen_mutex );
		memcpy( context->screen, screen, sizeof( screen ) );
		thread_mutex_unlock( &context->screen_mutex );		
        }

	assetsys_destroy( assetsys );
    frametimer_term( frametimer );
	free_sound(&music);
    return 0;
    }
	

// main app loop and init
	
int app_proc( app_t* app, void* user_data )
	{
	(void) user_data;
	app_title( app, "Macula Run" );
	#ifdef _DEBUG	
		app_screenmode_set( app, APP_SCREENMODE_WINDOW );
	#endif	
	app_interpolation_set( app, APP_INTERPOLATION_NONE );
	app_crtmode_set( app, APP_CRTMODE_ENABLED );
	
    audiosys_t* audiosys = audiosys_create( AUDIOSYS_DEFAULT_VOICE_COUNT, AUDIOSYS_DEFAULT_BUFFERED_SAMPLE_PAIRS_COUNT, AUDIOSYS_FEATURES_ALL, 0 );

	int const audio_buffering_in_sample_pairs = 4410;
    static short sound_buffer[ audio_buffering_in_sample_pairs ];

	app_sound_format_t sound_format;
	sound_format.channels = 2;
	sound_format.frequency = 44100;
	sound_format.bits_per_sample = 16;
	sound_format.size_in_samples = audio_buffering_in_sample_pairs;
	app_sound_format_set( app, sound_format );

    app_yield( app ); // ensure audio buffer is created before audio thread starts

    audio_thread_context_t audio_thread_context;
    audio_thread_context.exit_flag = 0;
    audio_thread_context.app = app;
    audio_thread_context.audiosys = audiosys;
    audio_thread_context.sound_buffer = sound_buffer;
    audio_thread_context.sound_buffer_sample_pairs_count = audio_buffering_in_sample_pairs;
    thread_ptr_t audio_thread = thread_create( audio_thread_proc, &audio_thread_context, NULL, THREAD_STACK_SIZE_DEFAULT );

	app_sound_start( app );

	//	screens
	static APP_U32 screen_xbgr[ 320 * 200 ];
	static uint8_t screen[ 320 * 200 ];

	// update thread
    update_thread_context_t update_thread_context;
    update_thread_context.exit_flag = 0;
    update_thread_context.app = app;
	update_thread_context.audiosys = audiosys;
	thread_mutex_init( &update_thread_context.audio_mutex );
    update_thread_context.screen = screen;
	thread_mutex_init( &update_thread_context.screen_mutex );
    thread_ptr_t update_thread = thread_create( update_thread_proc, &update_thread_context, NULL, THREAD_STACK_SIZE_DEFAULT );

	// main app loop
	while( app_is_running( app ) )
		{		
		app_yield( app );

		// post process and present screen
		thread_mutex_lock( &update_thread_context.screen_mutex );
		for( int i = 0; i < 320 * 200; ++i ) 
			{
			uint32_t c = screen[ i ];
			screen_xbgr[ i ] = ( c << 16 ) | ( c << 8 ) | c;
			}
		thread_mutex_unlock( &update_thread_context.screen_mutex );
		app_present_xbgr32( app, screen_xbgr, 320, 200, 0xffffff, 0x000000 );

		// update audio
		thread_mutex_lock( &update_thread_context.audio_mutex );
        audiosys_update( audiosys );
		thread_mutex_unlock( &update_thread_context.audio_mutex );		
		}

	// cleanup
	
	app_sound_stop( app );

	update_thread_context.exit_flag = 1;
    thread_join( update_thread );

	audio_thread_context.exit_flag = 1;
    thread_join( audio_thread );

	thread_mutex_term( &update_thread_context.audio_mutex );
	thread_mutex_term( &update_thread_context.screen_mutex );

    audiosys_destroy( audiosys );
	return 0;
	}


#ifndef NDEBUG
	#include <crtdbg.h>
#endif

int main( int argc, char** argv )
	{
    #ifndef NDEBUG
        int flag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG ); // Get current flag
        flag ^= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
        _CrtSetDbgFlag( flag ); // Set flag to the new value
//        _CrtSetBreakAlloc( 0 );
    #endif
	(void) argc, argv;
	return app_run( app_proc, NULL );
	}

// pass-through so the program will build with either /SUBSYSTEM:WINDOWS or /SUBSYSTEN:CONSOLE
extern "C" int __stdcall WinMain( struct HINSTANCE__*, struct HINSTANCE__*, char*, int ) { return main( __argc, __argv ); }

