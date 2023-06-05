#version 450 core

#include "../global_uniforms.glsl"

// In.
layout( location = 0 ) in vec3 inVoxelMin;
layout( location = 1 ) in vec3 inVoxelMax;

// Out.
out 
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	vec3 center = ( inVoxelMin + inVoxelMax ) / 2.f;
	outData.center = center;
	outData.voxelSize = inVoxelMax - inVoxelMin;		
	gl_Position =  getMatrixProjection() * getMatrixView() * getMatrixModel() * vec4( center, 1.f );
}
