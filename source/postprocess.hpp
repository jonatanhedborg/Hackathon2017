void post_process( APP_U32 screen_xbgr[ 320 * 200 ], materials_t materials )
{		
	static APP_U32 post1_xbgr[ 320 * 200 ];
	static APP_U32 post2_xbgr[ 320 * 200 ];

	// uint32_t blur_kernel[ 11 ] = 
		// { 
		// (uint32_t) ( 65536.0f * 0.000003 ),
		// (uint32_t) ( 65536.0f * 0.000229 ),
		// (uint32_t) ( 65536.0f * 0.005977 ),
		// (uint32_t) ( 65536.0f * 0.060598 ),
		// (uint32_t) ( 65536.0f * 0.241730 ),
		// (uint32_t) ( 65536.0f * 0.382925 ),
		// (uint32_t) ( 65536.0f * 0.241730 ),
		// (uint32_t) ( 65536.0f * 0.060598 ),
		// (uint32_t) ( 65536.0f * 0.005977 ),
		// (uint32_t) ( 65536.0f * 0.000229 ),
		// (uint32_t) ( 65536.0f * 0.000003 ),
		// };

	uint32_t blur_kernel[ 11 ] = 
		{ 
		(uint32_t) ( 65536.0f * 0.02 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.06 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.15 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.24 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.32 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.42 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.32 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.24 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.15 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.06 * 0.5 ),
		(uint32_t) ( 65536.0f * 0.02 * 0.5 ),
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
				if( materials[ (material_id) id ].dynamic_glow ) glow = ( ( r + g + b ) * glow ) / ( 3 * 255 );
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
				if( materials[ (material_id) id ].dynamic_glow ) glow = ( ( r + g + b ) * glow ) / ( 3 * 255 );
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
			uint32_t r = 2 * r1 + r2;
			uint32_t g = 2 * g1 + g2;
			uint32_t b = 2 * b1 + b2;
			r = r > 255 ? 255 : r;
			g = g > 255 ? 255 : g;
			b = b > 255 ? 255 : b;
			uint32_t c = r | ( g << 8 ) | ( b << 16 ) | ( post1_xbgr[ x + y * 320 ] & 0xff000000 );
			screen_xbgr[ x + y * 320 ] = c;
			}
		}
}				