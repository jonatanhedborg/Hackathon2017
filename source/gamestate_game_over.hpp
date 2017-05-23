
struct gamestate_game_over : gamestate_common
	{	
	batch_renderer renderer;
	float4x4 projection_matrix;
	camera_t camera;
	array<model_instance_t> models;
	float c = 0.0f;

	gamestate_game_over( object_repo* ctx ) : gamestate_common( ctx ), renderer(graph)
		{
		stop_music();
		projection_matrix = perspective_lh((float)pal_scr->width * 3.0f, (float)pal_scr->height * 3.0f, 0.1f, 1000.0f);
		camera.position = float3(0, 2, 0);
		camera.rotation = float3(0, 0, 0);
		models.add({&resources->models[game_resources::MODEL_GAME_OVER_BODY], float3(0, 2, -10), float3(1.0f, 0.2f, 0.0f ), MATERIAL_BROWN, MATERIAL_BROWN, OBSTACLE});
		models.add({&resources->models[game_resources::MODEL_GAME_OVER_FACE], float3(0, 2, -10), float3(1.0f, 0.2f, 0.0f ), MATERIAL_YELLOW, MATERIAL_YELLOW, OBSTACLE});
		}
		
	void update( object_repo* )
		{
		// Drawing
		float4x4 view_matrix_tmp = rotation_yaw_pitch_roll(camera.rotation.y, camera.rotation.x, camera.rotation.z);
		view_matrix_tmp = mul(view_matrix_tmp, translation(camera.position.x, camera.position.y, camera.position.z));
		float4x4 view_matrix = view_matrix_tmp;
		inverse(&view_matrix, 0, view_matrix_tmp);
		float4x4 view_projection_matrix = mul(view_matrix, projection_matrix);

		c += 0.01f;
		models[ 0 ].rotation.z = sin( c ) * 0.1f;
		models[ 1 ].rotation.z = sin( c ) * 0.1f;
		models[ 0 ].rotation.x = 1 + cos( c ) * 0.1f;
		models[ 1 ].rotation.x = 1 + cos( c ) * 0.1f;
		if( c > 3.0f ) switch_state<gamestate_intro>();
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
		}
	
	};
	
	