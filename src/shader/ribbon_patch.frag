#version 450

smooth in vec3 vViewPosition;
// smooth in vec3 vNormal;
// flat in vec3   vColor;

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

	vec3 vNormal = vec3( 0.f, 0.f, 0.f );
	vec3 vColor	 = vec3( 1.f, 1.f, 1.f );

	// Compress color and normal.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( vViewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( vViewPosition.z, vNormal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( vNormal.yz );
	viewPositionNormalCompressed.w = 0; // Padding.

	// Output data.
	outViewPositionNormal = viewPositionNormalCompressed;
	outColor			  = vec4( vColor, 32.f ); // w = specular shininess.
}
