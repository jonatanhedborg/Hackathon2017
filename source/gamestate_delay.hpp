
struct gamestate_delay : gamestate_common
	{	
	int count = 0;

	gamestate_delay( object_repo* ctx ) : gamestate_common( ctx )
		{
		
		}
		
	void update( object_repo* )
		{
		if( tobii->presence.status == TOBII_USER_PRESENCE_STATUS_AWAY || tobii->head_pose.position_validity == TOBII_VALIDITY_INVALID )
			{
			switch_state<gamestate_intro>();
			}
		app_key_t input = get_input();
		if(input == APP_KEY_ESCAPE) signal_exit();			
		}
	
	};
	
	