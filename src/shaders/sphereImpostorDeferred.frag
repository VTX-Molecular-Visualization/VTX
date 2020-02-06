#version 450

uniform mat4 uProjMatrix;

smooth in vec3 camImpPos;
flat in vec3 camSpherePos;
flat in vec3 sphereColor;
flat in float sphereRad;
flat in float dotCamSpherePos;

// 3 16 bits for color
// 3 16 bits for normal
// 1 32 bits for padding
layout(location = 0) out uvec4 outColorNormal;
// 3 32 bits for position in cam space
// 1 32 bits for specular
layout(location = 1) out vec4 outCamPosition;

void main()
{
	// (camImpPos - camSpherePos)^2 - sphereRad^2 = 0;
	const float a = dot(camImpPos, camImpPos);
	// b = -2.f * dot(camImpPos, camSpherePos);
	// but '-' is useless since 'b' is squared for 'delta'
	// so for 't', '-' is also removed
	const float b = 2.f * dot(camImpPos, camSpherePos);
	const float c = dotCamSpherePos - sphereRad * sphereRad;
	const float delta = b * b - 4.f * a * c;

	if (delta < 0.f)
	{
		// fragColor = vec4(sphereColor, 0.5f);
		discard;
	}
	else
	{
		// '-' is removed (see 'b')
		const float t = (b - sqrt(delta)) / (2.f * a);

		const vec3 camHit = camImpPos * t;

		const vec3 normal = normalize(camHit - camSpherePos);

		const vec2 clipZW = camHit.z * uProjMatrix[2].zw + uProjMatrix[3].zw;
		const float clipDepth = clipZW.x / clipZW.y;
		gl_FragDepth = ((gl_DepthRange.diff * clipDepth) + gl_DepthRange.near
						+ gl_DepthRange.far)
					   * 0.5f;

		uvec4 colorNormal = uvec4(0);
		vec4 camPosition = vec4(0);
		// fill G-buffers
		colorNormal.x = packHalf2x16(sphereColor.xy);
		colorNormal.y = packHalf2x16(vec2(sphereColor.z, normal.x));
		colorNormal.z = packHalf2x16(normal.yz);
		colorNormal.w = 0; // padding

		camPosition.xyz = camHit;
		camPosition.w = 60.f; // specular

		outColorNormal = colorNormal;
		outCamPosition = camPosition;
	}
}
