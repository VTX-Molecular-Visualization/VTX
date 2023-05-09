#version 450 core

layout( location = 0 ) in vec3 aVoxelMin;
layout( location = 1 ) in vec3 aVoxelMax;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	vec3 center = (aVoxelMin + aVoxelMax) / 2.;
	dataOut.center = center;
	dataOut.voxelSize = aVoxelMax - aVoxelMin;		
	gl_Position = u_projMatrix * u_MVMatrix * vec4(center, 1.f);
}
