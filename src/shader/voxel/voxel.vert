#version 420

layout( location = 0 ) in vec3 aVoxelMin;
layout( location = 1 ) in vec3 aVoxelMax;

flat out vec3 vVoxelSize;

layout(std140) uniform VoxelSettings
{
	mat4 uMVPMatrix;
	vec4 uColor;
};

void main()
{
	vec3 center = (aVoxelMin + aVoxelMax) / 2.;
	vVoxelSize = center - aVoxelMin;
	gl_Position = uMVPMatrix * vec4(center, 1.);
}
