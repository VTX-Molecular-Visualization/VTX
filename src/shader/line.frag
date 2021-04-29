#version 450

in VsOut
{
	smooth vec3 viewPosition;
	smooth vec3 normal;
}
vsIn;

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

	// Compress position and normal.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( vsIn.viewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( vsIn.viewPosition.z, vsIn.normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( vsIn.normal.yz );
	viewPositionNormalCompressed.w = 0; // Padding.

	outViewPositionNormal = viewPositionNormalCompressed;
	outColor			  = vec4( 0.9f, 0.9f, 0.9f, 32.f );
}
