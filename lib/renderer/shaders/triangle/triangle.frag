#version 460 core

#include "../struct_data_packed.glsl"

// In.
flat out StructVertexShaderFlat vsDataFlat; 
smooth out StructVertexShaderSmooth vsDataSmooth;

// Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;
layout( location = 2 ) out uvec2 outId;

void main()
{
	packData( vsDataSmooth.viewPosition, vsDataSmooth.normal, vsDataFlat.selected, outDataPacked );
	outColor = vsDataSmooth.color;
	outId    = uvec2( vsDataFlat.id, 0 );
}
