#version 450 core

in
#include "struct_vertex_shader.glsl"
dataIn;

// 3 16 bits for position.
// 3 16 bits for normal.
// 1 32 bits for padding.
//layout( location = 0 ) out uvec4 outViewPositionNormal;
// 3 32 bits for color.
// 1 32 bits for specular.
//layout( location = 1 ) out vec4 outColor;
//layout( location = 2 ) out uvec2 outId;

out vec4 fragColor;

void main()
{
	// Compress position and normal.
	uvec4 viewPositionNormalCompressed;
	viewPositionNormalCompressed.x = packHalf2x16( dataIn.viewPosition.xy );
	viewPositionNormalCompressed.y = packHalf2x16( vec2( dataIn.viewPosition.z, dataIn.normal.x ) );
	viewPositionNormalCompressed.z = packHalf2x16( dataIn.normal.yz );
	viewPositionNormalCompressed.w = packHalf2x16( vec2( dataIn.selected, 0 ) );

	// Output data.
	//outViewPositionNormal = viewPositionNormalCompressed;
	//outColor			  = vec4( dataIn.color, 32.f ); // w = specular shininess.	
	//outId				  = uvec2( dataIn.id, 0 );

	fragColor = vec4(dataIn.color, 1.f);
}
