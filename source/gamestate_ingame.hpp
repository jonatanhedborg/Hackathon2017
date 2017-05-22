using namespace array_ns;

struct model_instance_t {
	model_3d* model;
	float3 position;
	float3 rotation;
	int material_fill, material_line;
};

struct camera_t {
	float3 position;
	float3 rotation;
};

struct gamestate_ingame : gamestate_common {
	array<model_instance_t> models;
	batch_renderer renderer;
	float4x4 projection_matrix;
	camera_t camera;
	float next_segment_position;
	float player_position;


	gamestate_ingame( object_repo* ctx ) : gamestate_common( ctx ), renderer(graph), next_segment_position(0), player_position(0) {
		projection_matrix = perspective_lh((float)pal_scr->width, (float)pal_scr->height, 0.1f, 1000.0f);
		camera.position = float3(0, 2, 0);
		camera.rotation = float3(0, 0, 0);

		for(int i = 0; i < 2; ++i) {
			generate_segment();
		}
	}

	void generate_segment() {
		models.add({&resources->models[game_resources::MODEL_RIGHT_WALL], float3(0, 0, next_segment_position), float3(0, 0, 0), MATERIAL_GREEN, MATERIAL_LIGHT_GREEN});
		models.add({&resources->models[game_resources::MODEL_LEFT_WALL], float3(0, 0, next_segment_position), float3(0, 0, 0), MATERIAL_GREEN, MATERIAL_LIGHT_GREEN});
		models.add({&resources->models[game_resources::MODEL_FLOOR], float3(0, 0, next_segment_position), float3(0, 0, 0), MATERIAL_GREEN, MATERIAL_LIGHT_GREEN});		
		next_segment_position += 5.0f;
	}

	void clean_up_segments() {
		for (int i = 0; i < models.count(); ++i) {
			if (models[i].position.z < camera.position.z - 5) {
				models.remove(i);
				--i;
			}
		}
	}

	void update( object_repo* )	{
		// Fun stuff
		if (player_position + 5 > next_segment_position) {
			generate_segment();
		}

		clean_up_segments();

		player_position += 0.3f;

		camera.position.z = player_position;

		// Drawing
		float4x4 view_matrix_tmp = rotation_yaw_pitch_roll(camera.rotation.y, camera.rotation.x, camera.rotation.z);
		view_matrix_tmp = mul(view_matrix_tmp, translation(camera.position.x, camera.position.y, camera.position.z));
		float4x4 view_matrix;
		inverse(&view_matrix, 0, view_matrix_tmp);
		float4x4 view_projection_matrix = mul(view_matrix, projection_matrix);

		for(int i = 0; i < models.count(); ++i) {
			float4x4 transformation = translation(0, 0, 0);
			transformation = mul(transformation, rotation_yaw_pitch_roll(models[i].rotation.y, models[i].rotation.x, models[i].rotation.z));
			transformation = mul(transformation, translation(models[i].position.x, models[i].position.y, models[i].position.z));

			transformation = mul(transformation, view_projection_matrix);
			float3* polygons = models[i].model->vertices.data();
			for (int j = 0; j < models[i].model->vertices_per_polygon.count(); ++j) {
				renderer.submit(models[i].material_fill, models[i].material_line, transformation, models[i].model->vertices_per_polygon[j], polygons);
				polygons += models[i].model->vertices_per_polygon[j];
			}
		}

		renderer.render();
		
		int index = rand(game_resources::SOUNDS_LASER1, game_resources::SOUNDS_LASER6);
		app_key_t input = get_input();
		if (input == APP_KEY_SPACE) play_sound(&resources->sounds[index]);
		else if (input == APP_KEY_ESCAPE) signal_exit();

		update_fps();
	}
	
};
	
