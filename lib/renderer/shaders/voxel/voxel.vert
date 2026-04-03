#version 460 core

#include "../layout_uniforms_camera.glsl"
#include "struct_vertex_shader.glsl"

// In.
layout( location = 0 ) in vec3 inVoxelMin;
layout( location = 1 ) in vec3 inVoxelMax;

// Out.
flat out StructVertexShader vsData;

void main()
{
	vec3 center = ( inVoxelMin + inVoxelMax ) / 2.f;
	vsData.center = center;
	vsData.voxelSize = inVoxelMax - inVoxelMin;		
	gl_Position =  uniformsCamera.matrixProjection * uniformsCamera.matrixView * vec4( center, 1.f );
}
