using namespace array_ns;

enum RENDER_PASS {
	TRENCH,
	OBSTACLE
};

struct model_instance_t {
	model_3d* model;
	float3 position;
	float3 rotation;
	int material_fill, material_line;
	RENDER_PASS pass;

};

struct camera_t {
	float3 position;
	float3 rotation;
};

struct obstacle_t {
	float3 min, max;
	float position;
};

static int global_score = 0;

struct gamestate_ingame : gamestate_common {
	array<model_instance_t> models;
	array<obstacle_t> obstacles;

	batch_renderer trench_renderer;
	batch_renderer obstacle_renderer;
	float4x4 projection_matrix;
	camera_t camera;
	float3 prev_pos;
	
	int hint_count = 60;
	int gameover_countdown = 120;
	
	int obstacle_min_interval = 7;
	int obstacle_max_interval = 15;
	int segments_to_next_obstacle = 70;
	int wall_color = 0;
	
	float next_segment_position;

	float velocity[3] = { 0.0f, 0.0f, 0.0f };
	float last_head_pose[3] = { 0.0f, 0.0f, 0.0f };
	
	float target_rot[3] = { 0.0f, 0.0f, 0.0f };
	float current_rot[3] = { 0.0f, 0.0f, 0.0f };
	float velocity_rot[3] = { 0.0f, 0.0f, 0.0f };
	int score = 0;
	int level = 1;
	float player_velocity = 0.90f;

	bool init_headpose = false;


	gamestate_ingame( object_repo* ctx ) 
	: gamestate_common( ctx )
	, trench_renderer(graph)
	, obstacle_renderer(graph)
	, next_segment_position(0)
	, prev_pos(float3(0, 0, 0)) {
		projection_matrix = perspective_lh((float)pal_scr->width * 3.0f, (float)pal_scr->height * 3.0f, 0.1f, 1000.0f);
		camera.position = float3(0, 2, 0);
		camera.rotation = float3(0, 0, 0);

		for(int i = 0; i < 3; ++i) {
			generate_segment();
		}
	}

	~gamestate_ingame()
		{
		global_score = score;
		}

	void add_obstacle(game_resources::model_enum model, material_id background, material_id foreground) {
		models.add({&resources->models[model], float3(0, 0, next_segment_position), float3(0.0f), background, foreground, OBSTACLE});
		obstacles.add({resources->models[model].bounds_min, resources->models[model].bounds_max, next_segment_position});
	}

	void generate_segment() {
		wall_color = wall_color ? 0 : 1;
		models.add({&resources->models[game_resources::MODEL_RIGHT_WALL], float3(0, 0, next_segment_position), float3(0, 0, 0), wall_color ? MATERIAL_GREEN : MATERIAL_LIGHT_GREEN,  wall_color ? MATERIAL_LIGHT_CYAN : MATERIAL_BLACK });
		models.add({&resources->models[game_resources::MODEL_LEFT_WALL], float3(0, 0, next_segment_position), float3(0, 0, 0), wall_color ? MATERIAL_GREEN : MATERIAL_LIGHT_GREEN, wall_color ? MATERIAL_LIGHT_CYAN : MATERIAL_BLACK, TRENCH});
		models.add({&resources->models[game_resources::MODEL_FLOOR], float3(0, 0, next_segment_position), float3(0, 0, 0), wall_color ? MATERIAL_GREEN : MATERIAL_LIGHT_GREEN, wall_color ? MATERIAL_LIGHT_CYAN : MATERIAL_BLACK, TRENCH});		

		--segments_to_next_obstacle;
		if( segments_to_next_obstacle == 0)
		{
			switch( rand( 0, 7 ) )
			{
				case 0:
				case 1:
					add_obstacle(game_resources::MODEL_OBSTACLE_LEFT, MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW);
					
					break;	
				case 2:
				case 3:
					add_obstacle(game_resources::MODEL_OBSTACLE_RIGHT, MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW);
					break;
				case 4:
					add_obstacle(game_resources::MODEL_OBSTACLE_LEFT, MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW);
					add_obstacle(game_resources::MODEL_OBSTACLE_RIGHT, MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW);
					break;
				case 5:
					add_obstacle(game_resources::MODEL_OBSTACLE_HOR_CENTER, MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_LOWGLOW);
					break;
				case 6:
					add_obstacle(game_resources::MODEL_OBSTACLE_BIG_LEFT, MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_LOWGLOW);
					break;
				case 7:
					add_obstacle(game_resources::MODEL_OBSTACLE_BIG_RIGHT, MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_LOWGLOW);
					break;
			}

			segments_to_next_obstacle = rand( obstacle_min_interval, obstacle_max_interval );
		}

		next_segment_position -= 5.0f;
	}

	void clean_up_segments() {
		for (int i = 0; i < models.count(); ++i) {
			if (models[i].position.z > camera.position.z ) {
				models.remove(i);
				--i;
			}
		}

		for (int i = 0; i < obstacles.count(); ++i) {
			if (obstacles[i].position > camera.position.z ) {
				obstacles.remove(i);
				--i;
			}
		}
	}

	void update(object_repo*) {
		// Fun stuff
		if (camera.position.z - 200 < next_segment_position) {
			generate_segment();
		}
		if( hint_count > 0 )
		{
			--hint_count;
			if( hint_count == 0 )
				play_sound(&resources->sounds[game_resources::SOUNDS_AVOID_OBSTACLES]);		
		}

		if( gameover_countdown < 120 ) 
		{
			--gameover_countdown;
			if( gameover_countdown == 0 ) switch_state<gamestate_game_over>();
			goto render;
		}

		if (tobii->head_pose.position_validity == TOBII_VALIDITY_VALID)
		{
			if (!init_headpose)
			{
				for (int i = 0; i < 2; ++i)
					last_head_pose[i] = tobii->head_pose.position_xyz[i];

				init_headpose = true;
			}
			else
			{
				for (int i = 0; i < 2; ++i)
				{
					velocity[i] = velocity[i] * 0.6f + 0.4f * (tobii->head_pose.position_xyz[i] - last_head_pose[i]);
					last_head_pose[i] = tobii->head_pose.position_xyz[i];
				}
			}
		}
		else
		{
			for (int i = 0; i < 2; ++i)
			{
				last_head_pose[i] = 0;
				velocity[i] = 0;
			}
		}

		target_rot[2] = -tobii->head_pose.rotation_xyz[2];
		velocity_rot[2] -= velocity_rot[2] * 0.3f;
		velocity_rot[2] += (target_rot[2] - current_rot[2]) * 0.2f;
		current_rot[2] += velocity_rot[2] * 0.1f;

		prev_pos = camera.position;

		float delta = get_delta_time();
		camera.position.x += velocity[0] * delta * 1.8f;
		camera.position.y += velocity[1] * delta * 2.4f;
		camera.position.z -= player_velocity;

		if (camera.position.x > 1.85f)
		{
			camera.position.x = 1.85f;
			velocity[0] = 0;
		}
		else if (camera.position.x < -1.85f)
		{
			camera.position.x = -1.85f;
			velocity[0] = 0;
		}
		if (camera.position.y > 4.0f)
		{
			camera.position.y = 4.0f;
			velocity[1] = 0;
		}
		else if (camera.position.y < 0.65f)
		{
			camera.position.y = 0.65f;
			velocity[1] = 0;
		}

		camera.rotation.z = -current_rot[2];
		++score;
		if (score % 1000 == 0 && score != 0)
		{
			++level;
			player_velocity *= 1.25f;
		}


		clean_up_segments();

		bool collided = false;
		for (int i = 0; i < obstacles.count(); ++i) {
			obstacle_t o = obstacles[i];
			float3 p = camera.position;
			float3 min = o.min;
			min.z += o.position;
			float3 max = o.max;
			max.z += o.position;

			float3 ray_dir = p - prev_pos;
			float dist = length(ray_dir);
			float3 norm_ray_dir = ray_dir / dist;

			intersection_points_t ip = intersection_points_ray_aabb( prev_pos, norm_ray_dir, min, max );
			if (ip.count > 0) {
				for(int j = 0; j < ip.count; ++j) {
					if (length(ip.points[j] - p) <= dist) {
						collided = true;
						break;
					}
				}
			}

			if (collided) {
				break;
			}

			// if (p.x > min.x && p.y > min.y && p.z > min.z && p.x < max.x && p.y < max.y && p.z < max.z) {

			// }

		}

		if (collided) {
			stop_music();
			camera.position.z += 1.0f;
			play_sound(&resources->sounds[game_resources::SOUNDS_GAME_OVER]);		
			--gameover_countdown;
		}


render:
		// Drawing
		float4x4 view_matrix_tmp = rotation_yaw_pitch_roll(camera.rotation.y, camera.rotation.x, camera.rotation.z);
		view_matrix_tmp = mul(view_matrix_tmp, translation(camera.position.x, camera.position.y, camera.position.z));
		float4x4 view_matrix = view_matrix_tmp;
		inverse(&view_matrix, 0, view_matrix_tmp);
		float4x4 view_projection_matrix = mul(view_matrix, projection_matrix);

		for(int i = 0; i < models.count(); ++i) {
			float4x4 transformation = translation(0, 0, 0);
			transformation = mul(transformation, rotation_yaw_pitch_roll(models[i].rotation.y, models[i].rotation.x, models[i].rotation.z));
			transformation = mul(transformation, translation(models[i].position.x, models[i].position.y, models[i].position.z));

			transformation = mul(transformation, view_projection_matrix);
			float3* polygons = models[i].model->vertices.data();
			for (int j = 0; j < models[i].model->vertices_per_polygon.count(); ++j) {
				switch(models[i].pass) {
					case TRENCH:
						trench_renderer.submit(models[i].material_fill, models[i].material_line, transformation, models[i].model->vertices_per_polygon[j], polygons);
						break;
			
					case OBSTACLE:
						obstacle_renderer.submit(models[i].material_fill, models[i].material_line, transformation, models[i].model->vertices_per_polygon[j], polygons);
						break;
				}
				polygons += models[i].model->vertices_per_polygon[j];
			}
		}


		trench_renderer.render();
		obstacle_renderer.render();

		//Draw HUD stuff
		if ((score % 1000) < 100)
		{
			if (level == 1)
			{
				char *level_one = "Initiating Level 1.";
				sysfont_8x8_u8(pal_scr->screen, pal_scr->width, pal_scr->height, (pal_scr->width / 2) - ((int)strlen(level_one) * 8) / 2, 60, level_one, MATERIAL_WHITE);
				char *gl = "Good luck.";
				sysfont_8x8_u8(pal_scr->screen, pal_scr->width, pal_scr->height, (pal_scr->width / 2) - ((int)strlen(gl) * 8) / 2, 100, gl, MATERIAL_WHITE);

			}
			else
			{
				if (score % 33 < 16)
				{
					char* level_up = "Level up!";
					sysfont_9x16_u8(pal_scr->screen, pal_scr->width, pal_scr->height, (pal_scr->width / 2) - ((int)strlen(level_up) * 9) / 2, 60, level_up, MATERIAL_WHITE);
				}
				char lvl[18];
				sprintf(lvl, "Level %i.", level);
				sysfont_8x8_u8(pal_scr->screen, pal_scr->width, pal_scr->height, (pal_scr->width / 2) - ((int)strlen(lvl) * 8) / 2, 100, lvl, MATERIAL_WHITE);
			}
		}
		char str[16];
		sprintf(str, "%d", score);
		sysfont_9x16_u8(pal_scr->screen, pal_scr->width, pal_scr->height, pal_scr->width / 2 - ( (int)strlen( str ) * 9 )/ 2, 20, str, MATERIAL_WHITE);

		int index = rand(game_resources::SOUNDS_LASER1, game_resources::SOUNDS_LASER6);
		app_key_t input = get_input();
		if (input == APP_KEY_SPACE) play_sound(&resources->sounds[index]);
		else if (input == APP_KEY_ESCAPE) signal_exit();

#ifdef _DEBUG
		update_fps();
#endif
	}
	
};
	
