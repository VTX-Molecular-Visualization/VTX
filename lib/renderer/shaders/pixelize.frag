#version 460 core

#include "struct_data_packed.glsl"

// In.
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureColor;

layout ( std140, binding = 2 ) uniform Uniforms
{
	uint size;
	uint background;
} uniforms;

// Out.
layout( location = 0 ) out vec4 outFragColor;

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );
	const vec2 texSize = vec2( textureSize( inTextureColor, 0 ) );

	UnpackedData data;
	unpackData( inTexturePackedData, data, texCoord );

	if ( uniforms.background == 0u && data.viewPosition.z == 0.f )
	{
		outFragColor = texture( inTextureColor, vec2( texCoord ) / texSize );
	}
	else
	{
		const int size = int( uniforms.size );
		const ivec2 pixelCoord = texCoord - ( texCoord % size ) + ivec2( size / 2 );
		outFragColor = texture( inTextureColor, vec2( pixelCoord ) / texSize );
	}
}
