#version 450

in VsOut
{
	smooth vec3 viewPosition;
	smooth vec3 normal;
	flat vec3	color;
}
fsIn;

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
	viewPositionNormalCompressed.x = packHalf2x16( fsIn.viewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( fsIn.viewPosition.z, fsIn.normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( fsIn.normal.yz );
	viewPositionNormalCompressed.w = 0; // Padding.

	// Output data.
	outViewPositionNormal = viewPositionNormalCompressed;
	outColor			  = vec4( fsIn.color, 32.f ); // w = specular shininess.
}
