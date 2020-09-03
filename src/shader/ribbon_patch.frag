#version 450

in vec3 vViewPositionTE;
in vec3 vNormaleTE;
in vec3 vColorTE;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

void main()
{
	uvec4 colorNormal = uvec4( 0 );

	// Compress color and normal.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( vViewPositionTE.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( vViewPositionTE.z, vNormaleTE.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( vNormaleTE.yz );
	viewPositionNormalCompressed.w = 0; // Padding.

	// Output data.
	outViewPositionNormal = viewPositionNormalCompressed;
	outColor			  = vec4( vColorTE, 32.f ); // w = specular shininess.
}
