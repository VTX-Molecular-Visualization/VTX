#version 450

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

in
#include "struct_geometry_shader.glsl"
dataIn;

void main()
{
	vec3 viewPosition = vec3( u_MVMatrix * vec4( dataIn.center, 1.f ) );
	vec3	normal	   = normalize( vec3( u_normalMatrix * vec4( dataIn.center, 1.f )) );
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( viewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( viewPosition.z, normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( normal.yz );
	viewPositionNormalCompressed.w = 0; // Padding.
	
	outViewPositionNormal = viewPositionNormalCompressed;
	outColor = vec4( 0.5f, 0.5f, 0.5f, 32.f );
}
