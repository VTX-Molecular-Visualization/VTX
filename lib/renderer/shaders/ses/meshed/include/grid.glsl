uvec3 gridPosition( const vec3 p_worldPosition, const vec3 p_worldOrigin, const vec3 p_cellSize )
{	
	return uvec3( floor( ( p_worldPosition - p_worldOrigin ) / p_cellSize) );
}

uvec3 gridPosition( const uint p_hash, const uvec3 p_size )
{
	uint z = p_hash / ( p_size.x * p_size.y );
	uint r = p_hash % ( p_size.x * p_size.y );
	return uvec3( r % p_size.x , r / p_size.x, z );
}

vec3 worldPosition( const uvec3 p_gridPosition, const vec3 p_worldOrigin, const vec3 p_cellSize )
{
	return p_worldOrigin +  ( ( vec3( p_gridPosition ) + 0.5f ) * p_cellSize );
}

vec3 worldPosition( const uint p_hash, const uvec3 p_size, const vec3 p_worldOrigin, const vec3 p_cellSize )
{ 
	return worldPosition( gridPosition( p_hash, p_size ), p_worldOrigin, p_cellSize );
}

uint gridHash( const uvec3 p_gridPosition, const uvec3 p_size )
{
	return ( p_gridPosition.z * p_size.x * p_size.y ) + ( p_gridPosition.y * p_size.x ) + p_gridPosition.x;
}

uint gridHash( const vec3 p_worldPosition, const uvec3 p_size, const vec3 p_worldOrigin, const vec3 p_cellSize )
{
	return gridHash( gridPosition( p_worldPosition, p_worldOrigin, p_cellSize ), p_size );
}