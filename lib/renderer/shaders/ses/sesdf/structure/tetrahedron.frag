#version 450

in vec3		 position;
in vec3		 normal;
flat in vec3 color;
flat in int  enableShading;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
layout( location = 1 ) out vec4 outColor;

void main()
{
	// fill G-buffers.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( vec2( position.xy ) );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( position.z, normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( vec2( normal.yz ) );
	viewPositionNormalCompressed.w = enableShading;

	// Output data.
	outViewPositionNormal = viewPositionNormalCompressed;
	outColor.rgb = color;
}
