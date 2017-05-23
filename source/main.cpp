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
#include <assert.h>


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

enum render_mode_t
	{
	RENDER_MODE_MATERIALS,
	RENDER_MODE_PALETTE,
	};

#include "batch_renderer.hpp"

// palette
APP_U32 palette[ 16 ] = 
	{ 	
	0x000000, 
	0xaa0000, 
	0x006000, 
	0xaaaa00, 
	0x000040, 
	0xaa00aa, 
	0x0055aa, 
	0xaaaaaa,
	0x555555, 
	0xff5555, 
	0x10a010, 
	0xffff55, 
	0x5555ff, 
	0xff55ff, 
	0x55ffff, 
	0xffffff,
	};

struct material_t
	{
	int pal_index;	
	uint8_t glow;
	};

enum material_id
	{
	MATERIAL_SKY,
	MATERIAL_BLACK,
	MATERIAL_BLUE,
	MATERIAL_GREEN,
	MATERIAL_CYAN,
	MATERIAL_RED,
	MATERIAL_RED_GLOW,
	MATERIAL_MAGENTA,
	MATERIAL_BROWN,
	MATERIAL_LIGHT_GRAY,
	MATERIAL_GRAY,
	MATERIAL_LIGHT_BLUE,
	MATERIAL_LIGHT_GREEN,
	MATERIAL_LIGHT_CYAN,
	MATERIAL_LIGHT_RED,
	MATERIAL_LIGHT_RED_GLOW,
	MATERIAL_LIGHT_MAGENTA,
	MATERIAL_YELLOW,
	MATERIAL_WHITE,
	MATERIAL_LIGHT_CYAN_HI_GLOW,
	};
	
struct materials_t
	{
	materials_t()
		{
		memset( mtls, 0, sizeof( mtls ) );
		set( MATERIAL_SKY, -1, 255 );
		set( MATERIAL_BLACK, 0 );
		set( MATERIAL_BLUE, 1 );
		set( MATERIAL_GREEN, 2 );
		set( MATERIAL_CYAN, 3 );
		set( MATERIAL_RED, 4 );
		set( MATERIAL_RED_GLOW, 4, 128 );
		set( MATERIAL_MAGENTA, 5 );
		set( MATERIAL_BROWN, 6 );
		set( MATERIAL_LIGHT_GRAY, 7 );
		set( MATERIAL_GRAY, 8 );
		set( MATERIAL_LIGHT_BLUE, 9 );
		set( MATERIAL_LIGHT_GREEN, 10 );
		set( MATERIAL_LIGHT_CYAN, 11 );
		set( MATERIAL_LIGHT_RED, 12 );
		set( MATERIAL_LIGHT_RED_GLOW, 12, 192 );
		set( MATERIAL_LIGHT_MAGENTA, 13 );
		set( MATERIAL_YELLOW, 14 );
		set( MATERIAL_WHITE, 15 );
		set( MATERIAL_LIGHT_CYAN_HI_GLOW, 11, 128 );
		}
		
	void set( material_id id, int pal_index, uint8_t glow = 0 )
		{
		material_t& m = mtls[ (int) id ];
		m.pal_index = pal_index;
		m.glow = glow;
		}
	
	material_t mtls[ 256 ];
	
	material_t operator[]( material_id id ) { return mtls[ (int) id ]; }
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

struct game_resources
{
	enum model_enum
	{
		MODEL_SUZANNE,
		MODEL_LEFT_WALL,
		MODEL_RIGHT_WALL,
		MODEL_FLOOR,
		MODEL_RIGHT_WALL1,
		MODEL_OBSTACLE_LEFT,
		MODEL_OBSTACLE_RIGHT,
		MODEL_OBSTACLE_HOR_CENTER,
		MODEL_COUNT,
	};
	enum sounds_enum
	{
		SOUNDS_MUSIC,
		SOUNDS_PICKUP,
		SOUNDS_LASER1,
		SOUNDS_LASER2,
		SOUNDS_LASER3,
		SOUNDS_LASER4,
		SOUNDS_LASER5,
		SOUNDS_LASER6,
		SOUNDS_NEW_USER,
		SOUNDS_GET_READY,
		SOUNDS_INGAME_MUSIC,
		SOUNDS_COUNT,
	};

	model_3d models[MODEL_COUNT];
	audiosys_audio_source_t sounds[SOUNDS_COUNT];
};

void load_resources(game_resources* resources)
{
	// Mount current working folder's "data" folder as a virtual "/data" path
	assetsys_t* assetsys = assetsys_create(0);
	assetsys_mount(assetsys, "./data", "/data");
	//sounds
	if(!load_sound(assetsys, "/data/music.ogg", &resources->sounds[game_resources::SOUNDS_MUSIC])) assert(false);
	if(!load_sound(assetsys, "/data/pickup.ogg", &resources->sounds[game_resources::SOUNDS_PICKUP])) assert(false);
	if (!load_sound(assetsys, "/data/laser1.ogg", &resources->sounds[game_resources::SOUNDS_LASER1])) assert(false);
	if (!load_sound(assetsys, "/data/laser2.ogg", &resources->sounds[game_resources::SOUNDS_LASER2])) assert(false);
	if (!load_sound(assetsys, "/data/laser3.ogg", &resources->sounds[game_resources::SOUNDS_LASER3])) assert(false);
	if (!load_sound(assetsys, "/data/laser4.ogg", &resources->sounds[game_resources::SOUNDS_LASER4])) assert(false);
	if (!load_sound(assetsys, "/data/laser5.ogg", &resources->sounds[game_resources::SOUNDS_LASER5])) assert(false);
	if (!load_sound(assetsys, "/data/laser6.ogg", &resources->sounds[game_resources::SOUNDS_LASER6])) assert(false);
	if (!load_sound(assetsys, "/data/new_user.ogg", &resources->sounds[game_resources::SOUNDS_NEW_USER])) assert(false);
	if (!load_sound(assetsys, "/data/get_ready.ogg", &resources->sounds[game_resources::SOUNDS_GET_READY])) assert(false);
	if (!load_sound(assetsys, "/data/ingame_music.ogg", &resources->sounds[game_resources::SOUNDS_INGAME_MUSIC])) assert(false);

	//models
	if(!load_model(assetsys, "/data/suzanne.obj", &resources->models[game_resources::MODEL_SUZANNE])) assert(false);
	if(!load_model(assetsys, "/data/standard_wall_left.obj", &resources->models[game_resources::MODEL_LEFT_WALL])) assert(false);
	if(!load_model(assetsys, "/data/standard_wall_right.obj", &resources->models[game_resources::MODEL_RIGHT_WALL])) assert(false);
	if(!load_model(assetsys, "/data/standard_floor.obj", &resources->models[game_resources::MODEL_FLOOR])) assert(false);
	if(!load_model(assetsys, "/data/shaped_wall_right_1.obj", &resources->models[game_resources::MODEL_RIGHT_WALL1])) assert(false);
	if(!load_model(assetsys, "/data/obstacle_left.obj", &resources->models[game_resources::MODEL_OBSTACLE_LEFT])) assert(false);
	if(!load_model(assetsys, "/data/obstacle_right.obj", &resources->models[game_resources::MODEL_OBSTACLE_RIGHT])) assert(false);
	if(!load_model(assetsys, "/data/obstacle_hor_center.obj", &resources->models[game_resources::MODEL_OBSTACLE_HOR_CENTER])) assert(false);

	assetsys_destroy(assetsys);
}

void free_resources( game_resources* resources )
{
	for (int i = 0; i < game_resources::SOUNDS_COUNT; ++i)
		free_sound(&resources->sounds[i]);
}


// update thread	

struct update_thread_context_t
{
	int exit_flag;
	app_t* app;
	audiosys_t* audiosys;
	thread_mutex_t audio_mutex;
	uint8_t* screen;
	render_mode_t* render_mode;
	uint8_t* fade_level;
	thread_mutex_t screen_mutex;
	thread_mutex_t input_mutex;
	thread_mutex_t signal_mutex;
	int state_signaled_exit;
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

		
	game_resources resources;
	load_resources(&resources);

	//	screens/graphics
	static uint8_t screen[ 320 * 200 ];
	memset( screen, 0, sizeof( screen ) );
	pal_screen scr_pixels = { screen, 320, 200 };
	graph_ns::graph<pal_screen, uint8_t> graph_pixels( &scr_pixels );
	
	// frame time
	frametimer_t* frametimer = frametimer_init( 0 );
	frametimer_lock_rate( frametimer, 60 );

	render_mode_t render_mode = RENDER_MODE_MATERIALS;
	uint8_t fade_level = 255;

	// "global" systems
	object_repo objrepo;
	gamestate::game_state_system<object_repo> gamestates( &objrepo );
	objrepo.add( &render_mode );
	objrepo.add( &fade_level );
	objrepo.add( &scr_pixels );
	objrepo.add( &graph_pixels );
	objrepo.add( &tobii );
	objrepo.add( &gamestates );
	objrepo.add( frametimer );
	objrepo.add( &resources );
	objrepo.add( context );

	// init gamestates
	init_gamestates( &gamestates );
	
	// update loop
    while( !context->exit_flag )
        {
		// clear screen
		memset(screen, MATERIAL_SKY, sizeof( screen ) );

		// update frame
		float delta_time = frametimer_update( frametimer );
		tobii_process_callbacks( device );
		gamestates.update( delta_time );

		thread_mutex_lock( &context->screen_mutex );
		*context->render_mode = render_mode;
		*context->fade_level = fade_level;
		memcpy( context->screen, screen, sizeof( screen ) );
		thread_mutex_unlock( &context->screen_mutex );		
        }

    frametimer_term( frametimer );
	free_resources( &resources );
    return 0;
    }
	
static APP_U32 pal_green[] = { 0xff000000,0xff010100,0xff020300,0xff030600,0xff050800,0xff060b00,0xff090f00,0xff0b1300,0xff0d1700,0xff0f1b00,0xff121f00,0xff142400,0xff172800,0xff192c00,0xff1b3000,0xff1e3400,0xff1f3700,0xff1f3800,0xff203900,0xff213a00,0xff223c00,0xff223d00,0xff223f00,0xff234000,0xff234100,0xff244200,0xff254400,0xff254500,0xff264600,0xff264700,0xff264900,0xff274a00,0xff284b00,0xff284c00,0xff294d00,0xff294f00,0xff295000,0xff2a5100,0xff2b5200,0xff2b5400,0xff2b5500,0xff2c5600,0xff2c5800,0xff2c5900,0xff2d5b00,0xff2d5c00,0xff2d5d00,0xff2d5e00,0xff2d6000,0xff2e6100,0xff2e6300,0xff2e6300,0xff2e6400,0xff2e6600,0xff2f6800,0xff2e6900,0xff2e6900,0xff2f6b00,0xff2f6c00,0xff2f6e00,0xff2f6f00,0xff2f7001,0xff2f7201,0xff2f7300,0xff2f7400,0xff2f7600,0xff2e7800,0xff2e7900,0xff2e7a00,0xff2e7c00,0xff2d7d00,0xff2d7f00,0xff2c8100,0xff2c8300,0xff2c8500,0xff2b8600,0xff2b8700,0xff2a8900,0xff298a00,0xff288d00,0xff288f00,0xff289000,0xff279300,0xff279400,0xff259500,0xff259700,0xff259900,0xff249b00,0xff239c00,0xff229e00,0xff21a000,0xff20a100,0xff20a300,0xff1fa500,0xff1ea700,0xff1da900,0xff1caa00,0xff1bac00,0xff1bae00,0xff1ab000,0xff19b100,0xff18b400,0xff17b500,0xff16b700,0xff16b900,0xff15bb00,0xff14bc00,0xff13be00,0xff12c000,0xff11c100,0xff10c100,0xff10c300,0xff10c400,0xff10c500,0xff0fc600,0xff0ec800,0xff0dc900,0xff0dcb00,0xff0dcc00,0xff0ccc00,0xff0ccd00,0xff0ccf00,0xff0bd000,0xff0ad200,0xff09d300,0xff09d400,0xff08d500,0xff08d600,0xff08d800,0xff07d800,0xff06d900,0xff06db00,0xff06dc00,0xff05dd00,0xff05df00,0xff05df00,0xff04e000,0xff03e100,0xff03e200,0xff02e300,0xff02e400,0xff02e400,0xff02e600,0xff02e700,0xff01e800,0xff01e900,0xff01eb00,0xff00eb00,0xff00ec00,0xff00ed00,0xff00ee01,0xff00ee01,0xff00ef01,0xff00f002,0xff00f102,0xff00f203,0xff00f303,0xff00f404,0xff00f404,0xff00f504,0xff00f605,0xff00f705,0xff00f706,0xff00f806,0xff00f906,0xff00f907,0xff00fa08,0xff00fb08,0xff00fc09,0xff00fd09,0xff00fd09,0xff00fe0a,0xff00fe0b,0xff00fe0c,0xff00ff0c,0xff00ff0d,0xff01ff0f,0xff02ff10,0xff02ff12,0xff03ff13,0xff04ff15,0xff06ff17,0xff06ff19,0xff07ff1a,0xff09ff1c,0xff0aff1e,0xff0bff20,0xff0cff22,0xff0eff24,0xff0fff27,0xff10ff29,0xff13ff2b,0xff14ff2e,0xff15ff30,0xff17ff32,0xff19ff35,0xff1bff37,0xff1cff39,0xff1dff3c,0xff1fff3f,0xff21ff42,0xff23ff45,0xff25ff47,0xff26ff49,0xff29ff4d,0xff2bff50,0xff2dff52,0xff2fff55,0xff31ff57,0xff33ff5a,0xff35ff5d,0xff37ff5f,0xff39ff62,0xff3bff66,0xff3dff68,0xff3fff6a,0xff41ff6d,0xff44ff71,0xff47ff73,0xff48ff75,0xff4bff78,0xff4cff7b,0xff4fff7d,0xff51ff80,0xff53ff83,0xff55ff86,0xff58ff89,0xff5aff8a,0xff5cff8d,0xff5eff8f,0xff61ff92,0xff62ff94,0xff64ff96,0xff66ff99,0xff68ff9c,0xff6aff9e,0xff6dffa0,0xff6fffa1,0xff71ffa4,0xff72ffa5,0xff75ffa8,0xff77ffaa,0xff79ffad,0xff7fffb2,0xff84ffb6,0xff8affba,0xff91ffbf,0xff97ffc4,0xff9dffc9,0xffa4ffcd,0xffaaffd1,0xffb1ffd5,0xffb8ffd8,0xffbeffdc,0xffc5ffe0,0xffcaffe3, };

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
	static APP_U32 post1_xbgr[ 320 * 200 ];
	static APP_U32 post2_xbgr[ 320 * 200 ];
	static uint8_t screen[ 320 * 200 ];

	render_mode_t render_mode = RENDER_MODE_MATERIALS;
	uint8_t fade_level = 255;

	// update thread
    update_thread_context_t update_thread_context;
    update_thread_context.exit_flag = 0;
	update_thread_context.state_signaled_exit = 0;
    update_thread_context.app = app;
	update_thread_context.audiosys = audiosys;
	update_thread_context.render_mode = &render_mode;
	update_thread_context.fade_level = &fade_level;
	thread_mutex_init( &update_thread_context.audio_mutex );
    update_thread_context.screen = screen;
	thread_mutex_init( &update_thread_context.screen_mutex );
	thread_mutex_init(&update_thread_context.input_mutex);
	thread_mutex_init(&update_thread_context.signal_mutex);
    thread_ptr_t update_thread = thread_create( update_thread_proc, &update_thread_context, NULL, THREAD_STACK_SIZE_DEFAULT );

	materials_t materials;

	rnd_pcg_t pcg;
	rnd_pcg_seed(  &pcg , 0 );		

	struct star_t
		{
		float x;
		float y;
		float z;
		} stars[ 256 ];	
	
	for( int i = 0; i < sizeof( stars ) / sizeof( *stars ); ++i )
		{
		stars[ i ].x = ( rnd_pcg_nextf( &pcg ) - 0.5f ) * 3000.0f;
		stars[ i ].y = ( rnd_pcg_nextf( &pcg ) - 0.5f ) * 3000.0f;
		stars[ i ].z = ( rnd_pcg_nextf( &pcg ) + 0.000000001f ) * 15.0f;
		}

	// main app loop
	while( app_is_running( app ) )
		{		
		app_yield( app );
		
		// stars
		float delta_time = 1.0f / 60.0f;
		memset( screen_xbgr, 0, sizeof( screen_xbgr ) );
		for( int i = 0; i < sizeof( stars ) / sizeof( *stars ); ++i )
			{
			stars[ i ].z -= 3.0f * delta_time;
			int x = (int)( stars[ i ].x / stars[ i ].z ) + 160;
			int y = (int)( stars[ i ].y / stars[ i ].z ) + 100;
			if( stars[ i ].z <= 0.0f || x < 0 || x >= 320 || y < 0 || y >= 200 )
				{
				stars[ i ].x = ( rnd_pcg_nextf( &pcg ) - 0.5f ) * 3000.0f;
				stars[ i ].y = ( rnd_pcg_nextf( &pcg ) - 0.5f ) * 3000.0f;
				stars[ i ].z = 15.0f;
				continue;
				}
				
			int c = (int)( ( 1.0f - ( stars[ i ].z / 15.0f ) ) * 255.0f );
			if( ( screen_xbgr[ x + y * 320 ] & 0xff ) < c )
				screen_xbgr[ x + y * 320 ] = (uint32_t)( ( c << 16 ) | ( c << 8 ) | c );
			}

		if( render_mode == RENDER_MODE_MATERIALS )
			{
			// post process and present screen
			thread_mutex_lock( &update_thread_context.screen_mutex );
			for( int i = 0; i < 320 * 200; ++i ) 
				{
				material_id id = (material_id) screen[ i ];
				if( id == MATERIAL_SKY ) continue;
				
				material_t m = materials[ id ];
				screen_xbgr[ i ] = palette[ m.pal_index ] | ( id << 24 );			
				}
			thread_mutex_unlock( &update_thread_context.screen_mutex );
			
			uint32_t blur_kernel[ 11 ] = 
				{ 
				(uint32_t) ( 65536.0f * 0.000003 ),
				(uint32_t) ( 65536.0f * 0.000229 ),
				(uint32_t) ( 65536.0f * 0.005977 ),
				(uint32_t) ( 65536.0f * 0.060598 ),
				(uint32_t) ( 65536.0f * 0.241730 ),
				(uint32_t) ( 65536.0f * 0.382925 ),
				(uint32_t) ( 65536.0f * 0.241730 ),
				(uint32_t) ( 65536.0f * 0.060598 ),
				(uint32_t) ( 65536.0f * 0.005977 ),
				(uint32_t) ( 65536.0f * 0.000229 ),
				(uint32_t) ( 65536.0f * 0.000003 ),
				};


			memset( post1_xbgr, 0, sizeof( post1_xbgr ) );
			for( int y = 5; y < 200 - 5; ++y ) 
				{
				for( int x = 5; x < 320 - 5; ++x ) 
					{
					uint32_t acc_r = 0;
					uint32_t acc_g = 0;
					uint32_t acc_b = 0;
					for( int i = -5; i <= 5; ++i )
						{
						uint32_t c = screen_xbgr[ x + i + y * 320 ];
						uint32_t id = ( c >> 24 ) & 0xff;
						uint32_t glow = materials[ (material_id) id ].glow;
						uint32_t r = c & 0xff;
						uint32_t g = ( c >> 8 ) & 0xff;
						uint32_t b = ( c >> 16 ) & 0xff;
						acc_r += blur_kernel[ 5 + i ] * r * glow;
						acc_g += blur_kernel[ 5 + i ] * g * glow;
						acc_b += blur_kernel[ 5 + i ] * b * glow;
						}
					acc_r >>= 23;
					acc_g >>= 23;
					acc_b >>= 23;
					acc_r = acc_r > 255 ? 255 : acc_r;
					acc_g = acc_g > 255 ? 255 : acc_g;
					acc_b = acc_b > 255 ? 255 : acc_b;
					uint32_t c = acc_r | ( acc_g << 8 ) | ( acc_b << 16 ) | ( screen_xbgr[ x + y * 320 ] & 0xff000000 );
					post1_xbgr[ x + y * 320 ] = c;
					}
				}
					
			memset( post2_xbgr, 0, sizeof( post2_xbgr ) );
			for( int y = 5; y < 200 - 5; ++y ) 
				{
				for( int x = 5; x < 320 - 5; ++x ) 
					{
					uint32_t acc_r = 0;
					uint32_t acc_g = 0;
					uint32_t acc_b = 0;
					for( int i = -5; i <= 5; ++i )
						{
						uint32_t c = post1_xbgr[ x + ( y + i ) * 320 ];
						uint32_t id = ( c >> 24 ) & 0xff;
						uint32_t glow = materials[ (material_id) id ].glow;
						uint32_t r = c & 0xff;
						uint32_t g = ( c >> 8 ) & 0xff;
						uint32_t b = ( c >> 16 ) & 0xff;
						acc_r += blur_kernel[ 5 + i ] * r * glow;
						acc_g += blur_kernel[ 5 + i ] * g * glow;
						acc_b += blur_kernel[ 5 + i ] * b * glow;
						}
					acc_r >>= 23;
					acc_g >>= 23;
					acc_b >>= 23;
					acc_r = acc_r > 255 ? 255 : acc_r;
					acc_g = acc_g > 255 ? 255 : acc_g;
					acc_b = acc_b > 255 ? 255 : acc_b;
					uint32_t c = acc_r | ( acc_g << 8 ) | ( acc_b << 16 );
					post2_xbgr[ x + y * 320 ] = c;
					}
				}

			for( int y = 0; y < 200; ++y ) 
				{
				for( int x = 0; x < 320; ++x ) 
					{
					uint32_t c1 = post2_xbgr[ x + y * 320 ];
					uint32_t r1 = c1 & 0xff;
					uint32_t g1 = ( c1 >> 8 ) & 0xff;
					uint32_t b1 = ( c1 >> 16 ) & 0xff;
					uint32_t c2 = screen_xbgr[ x + y * 320 ];
					uint32_t r2 = c2 & 0xff;
					uint32_t g2 = ( c2 >> 8 ) & 0xff;
					uint32_t b2 = ( c2 >> 16 ) & 0xff;
					uint32_t r = r1 + r2;
					uint32_t g = g1 + g2;
					uint32_t b = b1 + b2;
					r = r > 255 ? 255 : r;
					g = g > 255 ? 255 : g;
					b = b > 255 ? 255 : b;
					uint32_t c = r | ( g << 8 ) | ( b << 16 ) | ( post1_xbgr[ x + y * 320 ] & 0xff000000 );
					screen_xbgr[ x + y * 320 ] = c;
					}
				}
			}
		else if( render_mode == RENDER_MODE_PALETTE )
			{
			thread_mutex_lock( &update_thread_context.screen_mutex );
			for( int i = 0; i < 320 * 200; ++i ) 
				{
				if( screen[ i ] == 0 ) continue;
				uint32_t c1 = screen_xbgr[ i ];
				uint32_t r1 = c1 & 0xff;
				uint32_t g1 = ( c1 >> 8 ) & 0xff;
				uint32_t b1 = ( c1 >> 16 ) & 0xff;
				uint32_t c2 = pal_green[ screen[ i ] ];
				uint32_t r2 = c2 & 0xff;
				uint32_t g2 = ( c2 >> 8 ) & 0xff;
				uint32_t b2 = ( c2 >> 16 ) & 0xff;
				uint32_t fl = (uint32_t) fade_level;
				uint32_t inv_fl = 255 - fl;
				uint32_t r = ( r1 * inv_fl + r2 * fl ) >> 8;
				uint32_t g = ( g1 * inv_fl + g2 * fl ) >> 8;
				uint32_t b = ( b1 * inv_fl + b2 * fl ) >> 8;
				uint32_t c = r | ( g << 8 ) | ( b << 16 );
				screen_xbgr[ i ] = c;
				}
			thread_mutex_unlock( &update_thread_context.screen_mutex );
			}

		app_present_xbgr32( app, screen_xbgr, 320, 200, 0xffffff, 0x000000 );

		// update audio
		thread_mutex_lock( &update_thread_context.audio_mutex );
        audiosys_update( audiosys );
		thread_mutex_unlock( &update_thread_context.audio_mutex );

		//check for state signal exit
		thread_mutex_lock(&update_thread_context.signal_mutex);
		if (update_thread_context.state_signaled_exit)
		{
			thread_mutex_unlock(&update_thread_context.signal_mutex);
			break;
		}
		else
			thread_mutex_unlock(&update_thread_context.signal_mutex);
		}

	// cleanup
	
	app_sound_stop( app );

	update_thread_context.exit_flag = 1;
    thread_join( update_thread );

	audio_thread_context.exit_flag = 1;
    thread_join( audio_thread );

	thread_mutex_term( &update_thread_context.audio_mutex );
	thread_mutex_term( &update_thread_context.screen_mutex );
	thread_mutex_term(&update_thread_context.input_mutex);
	thread_mutex_term(&update_thread_context.signal_mutex);

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

