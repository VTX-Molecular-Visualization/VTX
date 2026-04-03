#version 460 core

#include "../struct_data_packed.glsl"
#include "struct_vertex_shader.glsl"

// In.
smooth in StructVertexShader vsData;

// Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;

void main()
{
	packData( vsData.viewPosition, vsData.normal, 0, outDataPacked );
	outColor = vec4( 1.f, 1.f, 1.f, 1.f );
}
