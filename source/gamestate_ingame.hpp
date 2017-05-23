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

struct gamestate_ingame : gamestate_common {
	array<model_instance_t> models;

	batch_renderer trench_renderer;
	batch_renderer obstacle_renderer;
	float4x4 projection_matrix;
	camera_t camera;
	float next_segment_position;
	float player_position;

	float target_pos[3] = { 0.0f, 0.0f, 0.0f };
	float current_pos[3] = { 0.0f, 0.0f, 0.0f };
	float velocity_pos[3] = { 0.0f, 0.0f, 0.0f };

	bool origin_initialized = false;
	float headpose_origin_pos[3] = { 0.0f, 0.0f, 0.0f };

	float target_rot[3] = { 0.0f, 0.0f, 0.0f };
	float current_rot[3] = { 0.0f, 0.0f, 0.0f };
	float velocity_rot[3] = { 0.0f, 0.0f, 0.0f };


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
		models.add({&resources->models[game_resources::MODEL_RIGHT_WALL], float3(0, 0, next_segment_position), float3(), MATERIAL_GREEN, rand( 0, 1 ) ? MATERIAL_LIGHT_GREEN : MATERIAL_LIGHT_CYAN_HI_GLOW, TRENCH});
		models.add({&resources->models[game_resources::MODEL_LEFT_WALL], float3(0, 0, next_segment_position), float3(), MATERIAL_GREEN, rand( 0, 1 ) ? MATERIAL_LIGHT_GREEN : MATERIAL_LIGHT_CYAN_HI_GLOW, TRENCH});
		models.add({&resources->models[game_resources::MODEL_FLOOR], float3(0, 0, next_segment_position), float3(), MATERIAL_GREEN, rand( 0, 1 ) ? MATERIAL_LIGHT_GREEN : MATERIAL_LIGHT_CYAN_HI_GLOW, TRENCH});		

		if (rand(1, 10) == 1) {
			models.add({&resources->models[game_resources::MODEL_OBSTACLE_LEFT], float3(0, 0, next_segment_position), float3(), MATERIAL_BROWN, MATERIAL_MAGENTA, OBSTACLE});
		}

		if (rand(1, 10) == 1) {
			models.add({&resources->models[game_resources::MODEL_OBSTACLE_RIGHT], float3(0, 0, next_segment_position), float3(), MATERIAL_BROWN, MATERIAL_MAGENTA, OBSTACLE});
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

	void update( object_repo* )	{
		// Fun stuff
		if (player_position - 150 < next_segment_position) {
			generate_segment();
		}

		clean_up_segments();

		player_position -= 0.9f;

		if (!origin_initialized && tobii->head_pose.position_validity == TOBII_VALIDITY_VALID)
		{
			for (int i = 0; i < 3; ++i)
				headpose_origin_pos[i] = tobii->head_pose.position_xyz[i] * 0.35f;
			origin_initialized = true;
		}

		if (tobii->head_pose.position_validity == TOBII_VALIDITY_VALID)
		{
			for (int i = 0; i < 2; ++i)
			{
				target_pos[i] = (tobii->head_pose.position_xyz[i] * 0.25f - headpose_origin_pos[i]) * 0.1f;
				velocity_pos[i] -= velocity_pos[i] * 0.3f;
				velocity_pos[i] += (target_pos[i] - current_pos[i]) * 0.2f;
				current_pos[i] += velocity_pos[i] * 0.2f;
			}
		}

		target_rot[2] = -tobii->head_pose.rotation_xyz[2];
		velocity_rot[2] -= velocity_rot[2] * 0.3f;
		velocity_rot[2] += (target_rot[2] - current_rot[2]) * 0.2f;
		current_rot[2] += velocity_rot[2] * 0.1f;

		camera.position.x = current_pos[0];
		camera.position.y = current_pos[1];
		camera.position.z = player_position;

		if (camera.position.x > 2.0f)
		{
			camera.position.x = 2.0f;
			//target_pos[0] = 2.0f;
			//velocity_pos[0] = 0;
		}
		else if (camera.position.x < -2.0f)
		{
			camera.position.x = -2.0f;
			//target_pos[0] = -2.0f;
			//velocity_pos[0] = 0;
		}
		if (camera.position.y > 4.0f)
		{
			camera.position.y = 4.0f;
			//target_pos[1] = 4.0f;
			//velocity_pos[1] = 0;
		}
		else if (camera.position.y < 0.5f)
		{
			camera.position.y = 0.5f;
			//target_pos[1] = 0.5f;
			//velocity_pos[1] = 0;
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
	
