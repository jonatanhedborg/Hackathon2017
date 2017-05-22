#include "libs/array.hpp"
#include "libs/tinyobj_loader_c.h"


struct model_3d
{
	array_ns::array<float> vertices;
	array_ns::array<int> vertices_per_shape;
};

bool load_model(char* data, int size, model_3d* loaded_model)
{
	tinyobj_attrib_t attrib;
	tinyobj_attrib_init(&attrib);
	tinyobj_shape_t* shapes;
	size_t num_shapes;
	tinyobj_material_t* materials;
	size_t num_materials;
	int res = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, (char const*)data, (size_t)size, 0);

	if (res == TINYOBJ_SUCCESS)
	{
		for (int i = 0; i < attrib.num_vertices; ++i)
			loaded_model->vertices.add(attrib.vertices[i]);
		for (int i = 0; i < attrib.num_faces; ++i)
			loaded_model->vertices_per_shape.add(attrib.face_num_verts[i]);

		tinyobj_attrib_free(&attrib);
		tinyobj_shapes_free(shapes, num_shapes);
		tinyobj_materials_free(materials, num_materials);
		return true;
	}
	return false;
}