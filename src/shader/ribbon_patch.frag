#version 450

in vec3 te_viewPosition;
in vec3 te_normal;
in vec3 te_color;

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
	viewPositionNormalCompressed.x = packHalf2x16( te_viewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( te_viewPosition.z, te_normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( te_normal.yz );
	viewPositionNormalCompressed.w = 0; // Padding.

	// Output data.
	outViewPositionNormal = viewPositionNormalCompressed;
	outColor			  = vec4( te_color, 32.f ); // w = specular shininess.
}
