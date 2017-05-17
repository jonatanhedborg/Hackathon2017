
struct gamestate_intro : gamestate_common
	{	
	struct star_t
		{
		float x;
		float y;
		float z;
		} stars[ 256 ];	


	gamestate_intro( object_repo* ctx ) : gamestate_common( ctx )
		{
		for( int i = 0; i < sizeof( stars ) / sizeof( *stars ); ++i )
			{
			stars[ i ].x = ( randf() - 0.5f ) * 3000.0f;
			stars[ i ].y = ( randf() - 0.5f ) * 3000.0f;
			stars[ i ].z = ( randf() + 0.000000001f ) * 15.0f;
			}
		
		}
		
		
	void update( object_repo* )
		{
		float delta_time = get_delta_time();
		for( int i = 0; i < sizeof( stars ) / sizeof( *stars ); ++i )
			{
			stars[ i ].z -= 3.0f * delta_time;
			int x = (int)( stars[ i ].x / stars[ i ].z ) + 160;
			int y = (int)( stars[ i ].y / stars[ i ].z ) + 100;
			if( stars[ i ].z <= 0.0f || x < 0 || x >= 320 || y < 0 || y >= 200 )
				{
				stars[ i ].x = ( randf() - 0.5f ) * 3000.0f;
				stars[ i ].y = ( randf() - 0.5f ) * 3000.0f;
				stars[ i ].z = 15.0f;
				continue;
				}
				
			int c = (int)( ( 1.0f - ( stars[ i ].z / 15.0f ) ) * 255.0f );
			if( ( screen[ x + y * 320 ] ) < c )
				screen[ x + y * 320 ] = (uint8_t)c;
			}
		}
	
	};
	
	