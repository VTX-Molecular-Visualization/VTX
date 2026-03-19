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
	}
	else
	{
		const vec2 xy = floor( uniforms.size / 2.f ) - ( texCoord % uniforms.size ) + texCoord;
		outFragColor = texture( inTextureColor, xy / texSize );	
	}
}