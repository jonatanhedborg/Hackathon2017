struct batch_renderer
	{
	batch_renderer( graph_ns::graph<pal_screen, uint8_t>* screen ):
		scr( screen )
		{		
		}
	
	void submit( int material_fill, int material_line, float4x4 transform, int count, float3* vertices )
		{
		polygon_t& poly = polygons.add();
		poly.material_fill = material_fill;
		poly.material_line = material_line;
		poly.count = count;
		float z = 0.0f;
		for( int i = 0; i < count; ++i )
			{
			float3 v = transform_coord( vertices[ i ], transform );
			poly.verts[ i * 2 + 0 ] = (int) v.x;
			poly.verts[ i * 2 + 1 ] = (int) v.y;
			z += v.z;
			}
		poly.z = z / (float) count;
		}
		
	void render()
		{
		sort_ns::sort( polygons.data(), polygons.count() );
		polygon_t* poly;
		for( int i = 0; i < polygons.count(); ++i )
			{
			int* v = poly->verts;
			scr->polygon_fill<8>( v, poly->count, (uint8_t) poly->material_fill );
			for( int j = 0; j < poly->count; ++j )
				scr->line( v[ j + 0 ], v[ j + 1 ], v[ j + 2 ], v[ j + 3 ], (uint8_t) poly->material_line );
			++poly;
			}
		}
		
	struct polygon_t
		{
		int material_line;
		int material_fill;
		int count;
		float z;
		int verts[ 16 ];

		bool operator<( polygon_t const& other ) const
			{
			return z < other.z;
			}
			
		bool operator>( polygon_t const& other ) const
			{
			return z > other.z;
			}
		};
		

	graph_ns::graph<pal_screen, uint8_t>* scr;
	array<polygon_t> polygons;	
	};