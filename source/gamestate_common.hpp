
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
		sysfont_8x8_u8( pal_scr->screen, pal_scr->width, pal_scr->height, pal_scr->width - 40, 20, str, 255 );
		}
	
	objrepo::object_repo* objrepo;
	rnd_pcg_t pcg;
	pal_screen* pal_scr;
	uint8_t* screen;
	graph_ns::graph<pal_screen, uint8_t>* graph;
	tobii_t* tobii;
	frametimer_t* timer;
	int fps_counter;
	float fps_time;
	int fps;
	};


