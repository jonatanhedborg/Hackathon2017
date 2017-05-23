
struct gamestate_intro : gamestate_common
	{	
	bool user_present = false;

	gamestate_intro( object_repo* ctx ) : gamestate_common( ctx )
		{
		}
		
	void update( object_repo* )
		{
		if( !user_present )
			{
			if( tobii->presence.status == TOBII_USER_PRESENCE_STATUS_PRESENT && tobii->head_pose.position_validity == TOBII_VALIDITY_VALID )
				{
				user_present = true;
				play_sound(&resources->sounds[game_resources::SOUNDS_NEW_USER]);		
				switch_state<gamestate_headpose>();
				}
			}
		}
	
	};
	
	