#version 420 core

layout(std140) uniform VoxelSettings
{
	mat4 uMVPMatrix;
	vec4 uColor;
};

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

void main()
{
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( vec2( 100000.f) );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( 10000.f, 0.f ) );
	viewPositionNormalCompressed.z = packHalf2x16( vec2(0.f, 0.f) );
	viewPositionNormalCompressed.w = 0; // Padding.
	outViewPositionNormal = viewPositionNormalCompressed;

	outColor = vec4( uColor.rgb, 0.f ); // w = specular shininess.
}
