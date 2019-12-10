#version 450 core

layout(binding = 0) uniform usampler2D gbColorNormal;
layout(binding = 1) uniform sampler2D gbCamPosition;
layout(binding = 2) uniform sampler2D gbAOFactor;

out vec4 fragColor;

struct FragmentData
{
	vec3 color;
	vec3 normal;
	vec3 camPosition;
	float aoFactor;
};

void unpackGBuffers(ivec2 px, out FragmentData fd)
{
	const uvec4 colorNormal = texelFetch(gbColorNormal, px, 0);
	const vec4 camPosition = texelFetch(gbCamPosition, px, 0);
	const float aoFactor = texelFetch(gbAOFactor, px, 0).x;

	const vec2 tmp = unpackHalf2x16(colorNormal.y);

	fd.color = vec3(unpackHalf2x16(colorNormal.x), tmp.x);
	fd.normal = normalize(vec3(tmp.y, unpackHalf2x16(colorNormal.z)));
	fd.camPosition = camPosition.xyz;
	fd.aoFactor = aoFactor;
}

void main()
{
	FragmentData fd;
	unpackGBuffers(ivec2(gl_FragCoord), fd);

	// light is on the camera
	const vec3 lightDir = normalize(-fd.camPosition);
	const float intensity = dot(fd.normal, lightDir);
	float factor = 1.0;

	if (intensity < 0.25)
		factor = 0.2;
	else if (intensity < 0.5)
		factor = 0.4;
	else if (intensity < 0.75)
		factor = 0.55;
	else if (intensity < 0.95)
		factor = 0.7;

	fragColor = vec4((fd.color - fd.aoFactor) * factor, 1.f);
}
