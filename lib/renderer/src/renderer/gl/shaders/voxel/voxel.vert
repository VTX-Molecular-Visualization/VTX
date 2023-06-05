#version 450 core

#include "../global_uniforms.glsl"

// In.
layout( location = 0 ) in vec3 aVoxelMin;
layout( location = 1 ) in vec3 aVoxelMax;

// Out.
out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	vec3 center = (aVoxelMin + aVoxelMax) / 2.f;
	dataOut.center = center;
	dataOut.voxelSize = aVoxelMax - aVoxelMin;		
	gl_Position =  getMatrixProjection() * getMatrixView() * getMatrixModel() * vec4( center, 1.f );
}
