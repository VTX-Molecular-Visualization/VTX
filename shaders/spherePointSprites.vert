#version 450

layout(location = 0) in vec3 spherePosition;
layout(location = 1) in vec3 sphereColor;
layout(location = 2) in float sphereRadius;

uniform mat4 uMVMatrix;
uniform mat4 uProjMatrix;
uniform vec2 uScreenSize;

out SphereData
{
	vec4 spherePosition;
	vec3 sphereColor;
	float sphereRadius;
}
outData;

void main()
{
	outData.spherePosition = uMVMatrix * vec4(spherePosition, 1.f);
	outData.sphereColor = sphereColor;
	outData.sphereRadius = sphereRadius;

	vec4 projVoxel = uProjMatrix
					 * vec4(outData.sphereRadius, outData.sphereRadius,
							outData.spherePosition.z, outData.spherePosition.w);
	vec2 projSize = vec2(1280, 720) * projVoxel.xy / projVoxel.w;

	gl_Position = uProjMatrix * outData.spherePosition;
	gl_PointSize = 0.5f * (projSize.x + projSize.y);
}
