#version 450 core

#include "../struct_data_packed.glsl"

// In.
in
#include "struct_vertex_shader.glsl"
inData;

// Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

void main()
{
	packData( inData.viewPosition, inData.normal, inData.selected, outDataPacked );
	outColor = vec4( inData.color, 32.f ); // w = specular shininess.	
	outId    = uvec2( inData.id, 0 );
}
