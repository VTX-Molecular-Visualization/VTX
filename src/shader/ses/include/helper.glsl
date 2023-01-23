uint globalInvocationHash( const uvec3 p_position, const uvec3 p_size )
{
	return ( p_position.z * p_size.x * p_size.y ) + ( p_position.y * p_size.x ) + p_position.x;
}