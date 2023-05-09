#version 450

in 
#include "struct_vertex_shader.glsl"
dataIn;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

void main()
{
	// Compress position and normal.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( dataIn.viewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( dataIn.viewPosition.z, dataIn.normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( dataIn.normal.yz );
	viewPositionNormalCompressed.w = 0; // Padding.

	outViewPositionNormal = viewPositionNormalCompressed;
	outColor			  = vec4( 1.f, 1.f, 1.f, 32.f );
}
