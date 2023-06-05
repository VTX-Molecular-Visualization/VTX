#version 450 core

#include "../struct_data_packed.glsl"

// In.
in
#include "struct_vertex_shader.glsl"
dataIn;

// Out.
layout( location = 0 ) out uvec4 outViewPositionNormal;
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

void main()
{
	// Compress position and normal.
	uvec4 data;
	packData( dataIn.viewPosition, dataIn.normal, dataIn.selected, data );

	// Output data.
	outViewPositionNormal = data;
	outColor			  = vec4( dataIn.color, 32.f ); // w = specular shininess.	
	outId				  = uvec2( dataIn.id, 0 );
}
