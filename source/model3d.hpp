#include "libs/array.hpp"
#include "libs/tinyobj_loader_c.h"
#include "libs/assetsys.h"
#include "libs/vecmath.hpp"

using namespace vecmath;

struct model_3d
{
	array_ns::array<float3> vertices;
	array_ns::array<int> vertices_per_polygon;
};

bool load_model(assetsys_t* assetsys, char* name, model_3d* loaded_model)
{
	if (!assetsys) return false;
	if (!name) return false;

	assetsys_file_t file;
	assetsys_file(assetsys, name, &file);
	int size = assetsys_file_size(assetsys, file);
	if (size <= 0) return false;
	char* data = (char*)malloc((size_t)size);
	assetsys_file_load(assetsys, file, data);


	tinyobj_attrib_t attrib;
	tinyobj_attrib_init(&attrib);
	tinyobj_shape_t* shapes;
	size_t num_shapes;
	tinyobj_material_t* materials;
	size_t num_materials;
	int res = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, (char const*)data, (size_t)size, 0);

	if (res == TINYOBJ_SUCCESS)
	{
		int face_offset = 0;
		for (int i = 0; i < attrib.num_face_num_verts; ++i) {
			int vertex_count = attrib.face_num_verts[i];
			for (int j = 0; j < vertex_count; ++j) {
				int vertex_index = attrib.faces[face_offset++].v_idx;
				vertex_index *= 3;
			
				float x = attrib.vertices[vertex_index];
				float y = attrib.vertices[vertex_index+1];
				float z = attrib.vertices[vertex_index+2];
				float3 vertex(x, y, z);

				loaded_model->vertices.add(vertex);
			}
			loaded_model->vertices_per_polygon.add(vertex_count);
		}

		// for (int i = 0; i < attrib.num_vertices / 3; ++i)
		// 	loaded_model->vertices.add( vecmath::float3( attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2] ) );
		// for (int i = 0; i < attrib.num_faces; ++i)
		// 	loaded_model->vertices_per_shape.add(attrib.face_num_verts[i]);

		tinyobj_attrib_free(&attrib);
		tinyobj_shapes_free(shapes, num_shapes);
		tinyobj_materials_free(materials, num_materials);
		free(data);
		return true;
	}
	free(data);
	return false;
}