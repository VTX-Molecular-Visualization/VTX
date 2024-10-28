#version 450 core

#include "../struct_data_packed.glsl"

// In.
in
#include "struct_vertex_shader.glsl"
inData;

// Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;

void main()
{
	packData( inData.viewPosition, inData.normal, 0, outDataPacked );
	outColor			  = vec4( 1.f, 1.f, 1.f, 1.f );
}
