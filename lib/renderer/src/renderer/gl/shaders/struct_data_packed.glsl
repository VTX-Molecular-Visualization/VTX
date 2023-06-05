struct UnpackedData
{
	vec3 viewPosition;
	vec3 normal;
	uint selected;
};

#define PackedData uvec4

void packData( vec3 p_viewPosition, vec3 p_normal, uint p_selected, out PackedData p_out )
{
	p_out.x = packHalf2x16( p_viewPosition.xy );
	p_out.y = packHalf2x16( vec2( p_viewPosition.z, p_normal.x ) );
	p_out.z = packHalf2x16( p_normal.yz );
	p_out.w = packHalf2x16( vec2( p_selected, 0 ) );
	// TODO: add IDs.
}

void unpackData( usampler2D p_in, out UnpackedData p_out, ivec2 p_px )
{
	const uvec4 data	= texelFetch( p_in, p_px, 0 );
	vec2 tmp			= unpackHalf2x16( data.y );
	p_out.viewPosition  = vec3( unpackHalf2x16( data.x ), tmp.x );
	p_out.normal		= vec3( tmp.y, unpackHalf2x16( data.z ) );
	tmp					= unpackHalf2x16( data.w );
	p_out.selected		= uint( tmp.x );
}