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
	float3 min, max, position;
};

struct gamestate_ingame : gamestate_common {
	array<model_instance_t> models;
	array<obstacle_t> obstacles;

	batch_renderer trench_renderer;
	batch_renderer obstacle_renderer;
	float4x4 projection_matrix;
	camera_t camera;
	
	int hint_count = 60;
	
	int obstacle_min_interval = 7;
	int obstacle_max_interval = 15;
	int segments_to_next_obstacle = 70;
	int wall_color = 0;
	
	float next_segment_position;
	float player_position;

	float velocity[3] = { 0.0f, 0.0f, 0.0f };
	float last_head_pose[3] = { 0.0f, 0.0f, 0.0f };
	
	float target_rot[3] = { 0.0f, 0.0f, 0.0f };
	float current_rot[3] = { 0.0f, 0.0f, 0.0f };
	float velocity_rot[3] = { 0.0f, 0.0f, 0.0f };

	bool init_headpose = false;


	gamestate_ingame( object_repo* ctx ) 
	: gamestate_common( ctx )
	, trench_renderer(graph)
	, obstacle_renderer(graph)
	, next_segment_position(0)
	, player_position(0) {
		projection_matrix = perspective_lh((float)pal_scr->width * 3.0f, (float)pal_scr->height * 3.0f, 0.1f, 1000.0f);
		camera.position = float3(0, 2, 0);
		camera.rotation = float3(0, 0, 0);

		for(int i = 0; i < 3; ++i) {
			generate_segment();
		}
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
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_LEFT], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
					break;	
				case 2:
				case 3:
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_RIGHT], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
					break;
				case 4:
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_LEFT], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_RIGHT], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
					break;
				case 5:
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_HOR_CENTER], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
					break;
				case 6:
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_LEFT], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_HOR_CENTER], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
					break;
				case 7:
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_HOR_CENTER], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
					models.add({&resources->models[game_resources::MODEL_OBSTACLE_RIGHT], float3(0, 0, next_segment_position), float3(0.0f), MATERIAL_RED_GLOW, MATERIAL_LIGHT_RED_GLOW, OBSTACLE});
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
	}

	void update(object_repo*) {
		// Fun stuff
		if (player_position - 200 < next_segment_position) {
			generate_segment();
		}
		if( hint_count > 0 )
		{
			--hint_count;
			if( hint_count == 0 )
				play_sound(&resources->sounds[game_resources::SOUNDS_AVOID_OBSTACLES]);		
		}

		clean_up_segments();

		player_position -= 0.9f;

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

		float delta = get_delta_time();
		camera.position.x += velocity[0] * delta * 1.8f;
		camera.position.y += velocity[1] * delta * 2.4f;
		camera.position.z = player_position;

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
		
		int index = rand(game_resources::SOUNDS_LASER1, game_resources::SOUNDS_LASER6);
		app_key_t input = get_input();
		if (input == APP_KEY_SPACE) play_sound(&resources->sounds[index]);
		else if (input == APP_KEY_ESCAPE) signal_exit();

		update_fps();
	}
	
};
	
