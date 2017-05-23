
struct gamestate_common
	{
	gamestate_common( objrepo::object_repo* objrepo_ ) : 
		objrepo( objrepo_ ),
		fps_counter( 0 ),
		fps_time( 0.0f ),
		fps( 0 )
		{
		rnd_pcg_seed(  &pcg , 0 );		
		pal_scr = objrepo->get<pal_screen>();
		screen = pal_scr->screen;
		graph = objrepo->get<graph_ns::graph<pal_screen, uint8_t>>();
		tobii = objrepo->get<tobii_t>();
		timer = objrepo->get<frametimer_t>();
		resources = objrepo->get<game_resources>();
		update_context = objrepo->get<update_thread_context_t>();
		gamestates = objrepo->get< gamestate::game_state_system<object_repo> >();
		render_mode = objrepo->get<render_mode_t>();
		fade_level = objrepo->get<uint8_t>();
		}
		
	float randf() { return rnd_pcg_nextf( &pcg ); }
	int rand( int min, int max ) { return rnd_pcg_range( &pcg, min, max ); }
	float get_delta_time() { return frametimer_delta_time( timer ); }
	
	void update_fps()
		{
		fps_time += get_delta_time();
		++fps_counter;
		if( fps_time >= 1.0f )
			{
			fps_time -= 1.0f;
			fps = fps_counter;
			fps_counter = 0;
			}
		char str[ 16 ];
		sprintf( str, "%d", fps );
		sysfont_8x8_u8( pal_scr->screen, pal_scr->width, pal_scr->height, 40, 20, str, MATERIAL_WHITE );
		}

	void play_sound(audiosys_audio_source_t* source)
	{
		thread_mutex_lock(&update_context->audio_mutex);
		audiosys_sound_play(update_context->audiosys, *source, 0.0f, 0.0f);
		thread_mutex_unlock(&update_context->audio_mutex);
	}

	void play_music(audiosys_audio_source_t* source)
	{
		thread_mutex_lock(&update_context->audio_mutex);
		audiosys_music_play(update_context->audiosys, *source, 0.0f);
		audiosys_music_loop_set(update_context->audiosys, AUDIOSYS_LOOP_ON);
		thread_mutex_unlock(&update_context->audio_mutex);
	}


	void stop_music()
	{
		thread_mutex_lock(&update_context->audio_mutex);
		audiosys_music_stop(update_context->audiosys, 2.0f);
		thread_mutex_unlock(&update_context->audio_mutex);
	}

	app_key_t get_input()
	{
		thread_mutex_lock(&update_context->input_mutex);
		app_input_t input = app_input(update_context->app);
		thread_mutex_unlock(&update_context->input_mutex);
		for (int i = 0; i < input.count; ++i)
			if (input.events[i].type == APP_INPUT_KEY_DOWN) return input.events->data.key;

		return APP_KEY_INVALID;
	}

	void signal_exit()
	{
		thread_mutex_lock(&update_context->signal_mutex);
		update_context->state_signaled_exit = 1;
		thread_mutex_unlock(&update_context->signal_mutex);
	}
	
	template< typename T > void switch_state() 
		{ 
		gamestates->set<T>(); 
		}
	
	objrepo::object_repo* objrepo;
	rnd_pcg_t pcg;
	pal_screen* pal_scr;
	uint8_t* screen;
	gamestate::game_state_system<object_repo>* gamestates;
	graph_ns::graph<pal_screen, uint8_t>* graph;
	game_resources* resources;
	tobii_t* tobii;
	render_mode_t* render_mode;
	uint8_t* fade_level;
	frametimer_t* timer;
	update_thread_context_t* update_context;
	int fps_counter;
	float fps_time;
	int fps;
	};


