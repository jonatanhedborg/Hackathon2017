using namespace vecmath;

struct batch_renderer
	{
	batch_renderer( graph_ns::graph<pal_screen, uint8_t>* screen ):
		scr( screen )
		{		
		}
	
	void submit( int material_fill, int material_line, float4x4 xform, int count, float3* vertices )
		{
		polygon_t poly;
		poly.material_fill = material_fill;
		poly.material_line = material_line;
		poly.count = count;
		float z = 0.0f;
		float4 verts[3];
		for( int i = 0; i < count; ++i )
			{
			float4 v = transform(vertices[i], xform);
			if( v.z <= 0.0f ) return;
			v /= v.w;
			if( i < 3 ) verts[ i ] = v;
			poly.verts[ i * 2 + 0 ] = (int) (v.x) + scr->screen->width / 2;
			poly.verts[ i * 2 + 1 ] = -(int) (v.y) + scr->screen->height / 2;
			z += v.z;
			}
		poly.z = z / (float) count;
		
		float3 u = normalize( verts[ 1 ].xyz() - verts[ 0 ].xyz() );
		float3 v = normalize( verts[ 2 ].xyz() - verts[ 0 ].xyz() );
		float3 n = normalize( cross( u, v ) );
		//if( n.z < 0.0f )
			polygons.add( poly );
		}
		
	void render()
		{
		int count = polygons.count();
		polygon_t* poly = polygons.data();
		sort_ns::sort( poly, count );
		for( int i = 0; i < count; ++i )
			{
			int* v = poly->verts;
			scr->polygon_fill<64>( v, poly->count, (uint8_t) poly->material_fill );
			for( int j = 0; j < poly->count + 1; ++j )
				{
				int n = ( j % poly->count ) * 2;
				int m = ( ( j + 1 ) % poly->count ) * 2;
				scr->line( v[ n + 0 ], v[ n + 1 ], v[ m + 0 ], v[ m + 1 ], (uint8_t) poly->material_line );
				}
			++poly;
			}
		polygons.clear();
		}
		
	struct polygon_t
		{
		int material_line;
		int material_fill;
		int count;
		float z;
		int verts[ 128 ];

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