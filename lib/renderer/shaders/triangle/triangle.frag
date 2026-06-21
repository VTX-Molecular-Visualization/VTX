#version 460 core

#include "../struct_data_packed.glsl"
#include "struct_vertex_shader.glsl"

// In.
smooth in StructVertexShaderSmooth vsDataSmooth;

// Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

void main()
{
	packData( vsDataSmooth.viewPosition, vsDataSmooth.normal, 0u, outDataPacked );
	outColor = vec4( 0.8f, 0.8f, 0.8f, 1.f );
	outId	 = uvec2( 0u );
}
