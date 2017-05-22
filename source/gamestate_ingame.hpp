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
	array<model_instance_t*> models;
	batch_renderer renderer;
	float4x4 projection_matrix;
	camera_t camera;

	model_3d suzanne_model;
	model_instance_t suzanne_instance;

	gamestate_ingame( object_repo* ctx ) : gamestate_common( ctx ), renderer(graph) {
		projection_matrix = perspective_lh((float)pal_scr->width, (float)pal_scr->height, 0.1f, 1000.0f);
		// projection_matrix = perspective_lh((float)3.2, (float)2.4, 1.0f, 1000.0f);
		camera.position = float3(0, 2, 0);
		camera.rotation = float3(0, 0, 0);

		load_model(assetsys, "/data/standard_wall_left.obj", &suzanne_model);
		suzanne_instance = {&suzanne_model, float3(0, 0, 5), float3(0, 0, 0), 64, 128};
		models.add(&suzanne_instance);
	}

	void update( object_repo* )	{

		// suzanne_instance.rotation.y += 0.01f;
		// suzanne_instance.position.z -= 0.5f;

		float4x4 view_matrix_tmp = rotation_yaw_pitch_roll(camera.rotation.y, camera.rotation.x, camera.rotation.z);
		view_matrix_tmp = mul(view_matrix_tmp, translation(camera.position.x, camera.position.y, camera.position.z));
		float4x4 view_matrix;
		inverse(&view_matrix, 0, view_matrix_tmp);
		float4x4 view_projection_matrix = mul(view_matrix, projection_matrix);

		for(int i = 0; i < models.count(); ++i) {
			float4x4 transformation = translation(0, 0, 0);
			transformation = mul(transformation, rotation_yaw_pitch_roll(models[i]->rotation.y, models[i]->rotation.x, models[i]->rotation.z));
			transformation = mul(transformation, translation(models[i]->position.x, models[i]->position.y, models[i]->position.z));

			transformation = mul(transformation, view_projection_matrix);
			float3* polygons = models[i]->model->vertices.data();
			for (int j = 0; j < models[i]->model->vertices_per_polygon.count(); ++j) {
				renderer.submit(models[i]->material_fill, models[i]->material_line, transformation, models[i]->model->vertices_per_polygon[j], polygons);
				polygons += models[i]->model->vertices_per_polygon[j];
			}
		}

		renderer.render();

		update_fps();
	}
	
};
	
