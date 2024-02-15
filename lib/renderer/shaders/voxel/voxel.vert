#version 450 core

#include "../layout_uniforms_camera.glsl"

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
	gl_Position =  uniformsCamera.matrixProjection * uniformsCamera.matrixView * vec4( center, 1.f );
}
