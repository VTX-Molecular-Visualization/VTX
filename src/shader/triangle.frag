#version 450

smooth in vec3 vViewPosition;
smooth in vec3 vNormal;
flat in vec3   vColor;

// 3 16 bits for color
// 3 16 bits for normal
// 1 32 bits for padding
layout( location = 0 ) out uvec4 outColorNormal;
// 3 32 bits for position in view space
// 1 32 bits for specular
layout( location = 1 ) out vec4 outViewPosition;

void main()
{
	uvec4 colorNormal = uvec4( 0 );

	// Compress color and normal.
	colorNormal.x = packHalf2x16( vColor.xy );
	colorNormal.y = packHalf2x16( vec2( vColor.z, vNormal.x ) );
	colorNormal.z = packHalf2x16( vNormal.yz );
	colorNormal.w = 0; // padding

	// Output data.
	outColorNormal	= colorNormal;
	outViewPosition = vec4( vViewPosition, 32.f ); // w = specular shininess.
}
