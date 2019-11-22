#version 450 core

layout(binding = 0) uniform usampler2D gbColorNormal;
layout(binding = 1) uniform sampler2D gbCamPosition;
layout(binding = 2) uniform sampler2D gbAOFactor;

// layout(location = 0) out vec4 image;

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

	const vec3 lightDir = normalize(-fd.camPosition);
	const float diffuse = dot(fd.normal, lightDir);
	const vec3 ambient = fd.color * vec3(fd.aoFactor);
	fragColor = vec4((fd.color - fd.aoFactor) * diffuse, 1.f);
}
