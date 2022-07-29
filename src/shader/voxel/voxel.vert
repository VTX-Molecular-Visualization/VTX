#version 450

#define VOXEL_MIN 0
#define VOXEL_MAX 1

layout( location = VOXEL_MIN ) in vec3 aVoxelMin;
layout( location = VOXEL_MAX ) in vec3 aVoxelMax;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

out VsOut
{
	flat vec3 voxelSize;
	flat vec3 center;
}
vsOut;

void main()
{
	vec3 center = (aVoxelMin + aVoxelMax) / 2.;
	vsOut.center = center;
	vsOut.voxelSize = aVoxelMax - aVoxelMin;		
	gl_Position = u_projMatrix * u_MVMatrix * vec4(center, 1.f);
}
