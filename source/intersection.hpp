using namespace vecmath;

bool intersection_test_ray_triangle( float3 ray_origin, float3 ray_direction, float3 v0, float3 v1, float3 v2 );
bool intersection_test_ray_sphere( float3 ray_origin, float3 ray_direction, float4 sphere );
bool intersection_test_ray_aabb( float3 ray_origin, float3 ray_direction, float3 aabb_min, float3 aabb_max );
bool intersection_test_ray_plane( float3 ray_origin, float3 ray_direction, float4 plane );
bool intersection_test_point_sphere(float3 point, float4 sphere);
bool intersection_test_point_aabb(float3 point, float3 aabb_min, float3 aabb_max);
bool intersection_test_sphere_sphere(float4 sphere1, float4 sphere2 );
bool intersection_test_sphere_aabb(float4 sphere, float3 aabb_min, float3 aabb_max);
bool intersection_test_aabb_aabb( float3 aabb1_min, float3 aabb1_max, float3 aabb2_min, float3 aabb2_max );

struct intersection_points_t
	{
	int count;
	float3 points[ 2 ];
	};

intersection_points_t intersection_points_ray_sphere( float3 ray_origin, float3 ray_direction, float4 sphere );
intersection_points_t intersection_points_ray_aabb( float3 ray_origin, float3 ray_direction, 
	float3 aabb_min, float3 aabb_max );
intersection_points_t intersection_points_ray_plane( float3 ray_origin, float3 ray_direction, float4 plane );

// in the resulting point, x and y contains u and v on triangle, z contains distance along ray
intersection_points_t intersection_points_ray_triangle( float3 ray_origin, float3 ray_direction, 
	float3 v0, float3 v1, float3 v2);
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool intersection_test_ray_triangle( float3 ray_origin, float3 ray_direction, float3 v0, float3 v1, float3 v2 )
	{
    float3 edge1 = v1 - v0;
    float3 edge2 = v2 - v0;

    float3 p = cross( ray_direction, edge2 );
	float det = dot( edge1, p );
    float3 t = det > 0 ? ray_origin - v0 : v0 - ray_origin;
	det = abs( det );
    if( det < 0.0001f ) return false;

    float u = dot( t, p );
    if( u < 0.0f || u > det ) return false;

    float3 q = cross( t, edge1 );
    float v = dot( ray_direction, q );
    if( v < 0.0f || u + v > det ) return false;

    return true;
	}


bool intersection_test_ray_sphere( float3 ray_origin, float3 ray_direction, float4 sphere )
	{
	float3 diff = sphere.xyz() - ray_origin;
    float len = lengthsq( ray_direction );
	float t = dot( diff, ray_direction ) / len;
    diff -= ray_direction * t;
    float dist = lengthsq( diff );
	return dist <= sphere.w * sphere.w;
	}

	
bool intersection_test_ray_aabb( float3 ray_origin, float3 ray_direction, float3 aabb_min, float3 aabb_max )
	{
 	float3 bounds[ 2 ] = { aabb_min, aabb_max };

	float3 inv_dir = 1.0f / ray_direction; 
	int sign[ 3 ] = { inv_dir.x < 0 ? 1 : 0, inv_dir.y < 0 ? 1 : 0, inv_dir.z < 0 ? 1 : 0 }; 
	
    float tmin =  ( bounds[     sign[ 0 ] ].x - ray_origin.x ) * inv_dir.x; 
    float tmax =  ( bounds[ 1 - sign[ 0 ] ].x - ray_origin.x ) * inv_dir.x; 
    float tymin = ( bounds[     sign[ 1 ] ].y - ray_origin.y ) * inv_dir.y; 
    float tymax = ( bounds[ 1 - sign[ 1 ] ].y - ray_origin.y ) * inv_dir.y; 
    if( tmin > tymax || tymin > tmax )  return false; 

    if( tymin > tmin ) tmin = tymin; 
    if( tymax < tmax ) tmax = tymax; 
    float tzmin = ( bounds[     sign[ 2 ] ].z - ray_origin.z) * inv_dir.z; 
    float tzmax = ( bounds[ 1 - sign[ 2 ] ].z - ray_origin.z) * inv_dir.z; 
    if( tmin > tzmax || tzmin > tmax ) return false; 
 
    return true; 
	}

	
bool intersection_test_ray_plane( float3 ray_origin, float3 ray_direction, float4 plane )
	{
	float term1 = ray_origin.x * plane.x + ray_origin.y * plane.y + ray_origin.z * plane.z - plane.w;
	float term2 = ( -ray_direction.x ) * plane.x + ( -ray_direction.y ) * plane.y + ( -ray_direction.z ) * plane.z;
	float u = ( term2 == 0.0f ) ? 0.0f : term1 / term2;
	return u > 0.0f;
	}

	
bool intersection_test_point_sphere(float3 point, float4 sphere)
	{
	return lengthsq( point - sphere.xyz() ) <= sphere.w * sphere.w;
	}

	
bool intersection_test_point_aabb(float3 point, float3 aabb_min, float3 aabb_max)
	{
	return point.x >= aabb_min.x && point.y >= aabb_min.y && point.z >= aabb_min.z &&
		   point.x <= aabb_max.x && point.y <= aabb_max.y && point.z <= aabb_max.z;
	}

	
bool intersection_test_sphere_sphere(float4 sphere1, float4 sphere2 )
	{
	float r = sphere1.w + sphere2.w;
	return lengthsq( sphere1.xyz() - sphere2.xyz() ) < r * r;
	}

	
bool intersection_test_sphere_aabb(float4 sphere, float3 aabb_min, float3 aabb_max)
	{
	float d = 0;
	for( int i = 0; i < 3; ++i ) 
		{
		if( sphere[ i ] < aabb_min[ i ] ) 
			{
			float s = sphere[ i ] - aabb_min[ i ];
			d += s * s; 
			}
		else if( sphere[ i ] > aabb_max[ i ] ) 
			{		
			float s = sphere[ i ] - aabb_max[ i ];     
			d += s * s; 
			}
		}
		
	return d <= sphere.w * sphere.w;
	}

	
bool intersection_test_aabb_aabb( float3 aabb1_min, float3 aabb1_max, float3 aabb2_min, float3 aabb2_max )
	{
	float3 aabb1_extent = ( aabb1_max - aabb1_min ) / 2;
	float3 aabb1_center = aabb1_min + aabb1_extent;
	float3 aabb2_extent = ( aabb2_max - aabb2_min ) / 2;
	float3 aabb2_center = aabb2_min + aabb2_extent;
    if( abs( aabb1_center[ 0 ] - aabb2_center[ 0 ] ) > ( aabb1_extent[ 0 ] + aabb2_extent[ 0 ] ) ) return false;
    if( abs( aabb1_center[ 1 ] - aabb2_center[ 1 ] ) > ( aabb1_extent[ 1 ] + aabb2_extent[ 1 ] ) ) return false;
    if( abs( aabb1_center[ 2 ] - aabb2_center[ 2 ] ) > ( aabb1_extent[ 2 ] + aabb2_extent[ 2 ] ) ) return false;
    return true;
	}

	
intersection_points_t intersection_points_ray_sphere( float3 ray_origin, float3 ray_direction, float4 sphere )
	{
	intersection_points_t result;

    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
	float3 kDiff = ray_origin - sphere.xyz();
    float fA = lengthsq( ray_direction );
	float fB = dot( kDiff, ray_direction );
    float fC = lengthsq( kDiff ) - sphere.w * sphere.w;

    float afT[ 2 ];
    float fDiscr = fB * fB - fA * fC;
    if( fDiscr < 0.0 )
    {
        result.count = 0;
    }
    else if( fDiscr > 0.0 )
    {
        float fRoot = sqrt( fDiscr );
        float fInvA = 1.0f / fA;
        afT[ 0 ] = ( -fB - fRoot ) * fInvA;
        afT[ 1 ] = ( -fB + fRoot ) * fInvA;

        if( afT[ 0 ] >= 0.0f )
            result.count = 2;
        else if( afT[ 1 ] >= 0.0f )
            result.count = 1;
        else
            result.count = 0;
    }
    else
    {
        afT[ 0 ] = -fB / fA;
        result.count = ( afT[ 0 ] >= 0.0 ? 1 : 0 );
    }

    for( int i = 0; i< result.count; ++i ) result.points[ i ] = ray_origin + ray_direction * afT[ i ];

	return result;
	}
	


intersection_points_t intersection_points_ray_aabb( float3 ray_origin, float3 ray_direction, 
	float3 aabb_min, float3 aabb_max )
	{
	intersection_points_t result;
	result.count = 0;

 	float3 bounds[ 2 ] = { aabb_min, aabb_max };

	float3 inv_dir = 1.0f / ray_direction; 
	int sign[ 3 ] = { inv_dir.x < 0 ? 1 : 0, inv_dir.y < 0 ? 1 : 0, inv_dir.z < 0 ? 1 : 0 }; 
	
    float tmin =  ( bounds[     sign[ 0 ] ].x - ray_origin.x ) * inv_dir.x; 
    float tmax =  ( bounds[ 1 - sign[ 0 ] ].x - ray_origin.x ) * inv_dir.x; 
    float tymin = ( bounds[     sign[ 1 ] ].y - ray_origin.y ) * inv_dir.y; 
    float tymax = ( bounds[ 1 - sign[ 1 ] ].y - ray_origin.y ) * inv_dir.y; 
    if( tmin > tymax || tymin > tmax ) return result; 

    if( tymin > tmin ) tmin = tymin; 
    if( tymax < tmax ) tmax = tymax; 
    float tzmin = ( bounds[     sign[ 2 ] ].z - ray_origin.z) * inv_dir.z; 
    float tzmax = ( bounds[ 1 - sign[ 2 ] ].z - ray_origin.z) * inv_dir.z; 
    if( tmin > tzmax || tzmin > tmax ) return result; 
 
	if (tzmin > tmin) tmin = tzmin;
	if (tzmax < tmax) tmax = tzmax;
  
	result.count = 2;
	result.points[ 0 ] = ray_origin + ray_direction * tmin;
	result.points[ 1 ] = ray_origin + ray_direction * tmax;	
	return result;
	}
	

intersection_points_t intersection_points_ray_plane( float3 ray_origin, float3 ray_direction, float4 plane )
	{
	intersection_points_t result;
	result.count = 0;

	float term1 = ray_origin.x * plane.x + ray_origin.y * plane.y + ray_origin.z * plane.z - plane.w;
	float term2 = ( -ray_direction.x ) * plane.x + ( -ray_direction.y ) * plane.y + ( -ray_direction.z ) * plane.z;

	float u = (term2 == 0.0f) ? 0.0f : term1 / term2;

	if ( u > 0.0f )
	{
		result.points[ 0 ] = (ray_direction * u) + ray_origin;
		result.count = 1;
		return result;
	}

	return result;
	}
	
	
intersection_points_t intersection_point_ray_triangle( float3 ray_origin, float3 ray_direction, 
	float3 v0, float3 v1, float3 v2 )
	{
	intersection_points_t result;
	result.count = 0;

    float3 edge1 = v1 - v0;
    float3 edge2 = v2 - v0;

    float3 p = cross( ray_direction, edge2 );
	float det = dot( edge1, p );
    float3 t = det > 0 ? ray_origin - v0 : v0 - ray_origin;
	det = abs( det );
    if( det < 0.0001f ) return result;

    float u = dot( t, p );
    if( u < 0.0f || u > det ) return result;

    float3 q = cross( t, edge1 );
    float v = dot( ray_direction, q );
    if( v < 0.0f || u + v > det ) return result;
	
    float d = dot( edge2, q );
    float inv_det = 1.0f / det;

	result.count = 1;
	result.points[ 0 ].x = u * inv_det;
	result.points[ 0 ].y = v * inv_det;
	result.points[ 1 ].z = d * inv_det;
	return result;
	}
	
