#version 450 core

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform vec3 uBackgroundColor;

out vec4 fragColor;

struct UnpackedData
{
	vec3 color;
	vec3 normal;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 colorNormal = texelFetch( gbColorNormal, px, 0 );

	const vec2 tmp = unpackHalf2x16( colorNormal.y );
	data.color	   = vec3( unpackHalf2x16( colorNormal.x ), tmp.x );
	data.normal	   = vec3( tmp.y, unpackHalf2x16( colorNormal.z ) );
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackGBuffers( texCoord, data );

	if ( data.normal.x == 0.f && data.normal.y == 0.f && data.normal.z == 0.f )
	{
		fragColor = vec4( uBackgroundColor, 1.f );
		return;
	}

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	fragColor = vec4( data.color * ambientOcclusion, 1.f );
}
