#version 450 core

#include "struct_data_packed.glsl"

// In.
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureColor;

layout ( std140, binding = 2 ) uniform Uniforms
{
	uint size;
	bool background;
} uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );
	const vec2 texSize = textureSize( inTextureColor, 0 );

	UnpackedData data;
	unpackData( inTexturePackedData, data, texCoord );

	if ( uniforms.background == false && data.viewPosition.z == 0.f )
	{
		outFragColor = texture( inTextureColor, texCoord / texSize );
		return;
	}
	else
	{
		float x = int( gl_FragCoord.x ) % uniforms.size;
		float y = int( gl_FragCoord.y ) % uniforms.size;

		x = floor( uniforms.size / 2.0 ) - x;
		y = floor( uniforms.size / 2.0 ) - y;

		x = gl_FragCoord.x + x;
		y = gl_FragCoord.y + y;

		vec2 uv = vec2( x, y ) / texSize;

		outFragColor = texture( inTextureColor, uv );	
	}
}