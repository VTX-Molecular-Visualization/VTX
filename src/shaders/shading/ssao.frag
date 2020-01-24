#version 450

layout(binding = 0) uniform usampler2D gbColorNormal;
layout(binding = 1) uniform sampler2D gbCamPosition;
layout(binding = 2) uniform sampler2D noise;
layout(binding = 3) uniform sampler2D depth;

layout(location = 0) out float aoFactor;

const float BIAS = 0.025f;
const float SAMPLE_RAD = 1.5f;
const int KERNEL_SIZE = 32;
const float INTENSITY = 1.f;

uniform mat4 uProjMatrix;
uniform vec3 uAoKernel[KERNEL_SIZE];

struct FragmentData
{
	// vec3 color;
	vec3 normal;
	vec3 camPosition;
};

void unpackGBuffers(const in vec2 texPos, out FragmentData fd)
{
	const uvec4 colorNormal = texture(gbColorNormal, texPos, 0);
	const vec4 camPosition = texture(gbCamPosition, texPos, 0);

	const vec2 tmp = unpackHalf2x16(colorNormal.y);

	fd.normal = normalize(vec3(tmp.y, unpackHalf2x16(colorNormal.z)));
	fd.camPosition = camPosition.xyz;
}

void main()
{
	const vec2 texSize = textureSize(gbCamPosition, 0);
	const vec2 noiseScale = texSize / 128.f; // noise tex is 128x128
	const vec2 texPos = gl_FragCoord.xy / texSize;

	FragmentData fd;
	unpackGBuffers(texPos, fd);
	const vec3 p = fd.camPosition.xyz;

	const float rad = abs(p.z) * smoothstep(0.1f, 1.f, abs(p.z) / SAMPLE_RAD);
	vec3 randomVec = normalize(texture(noise, texPos * noiseScale).xyz);
	// Gram-Schmidt process
	vec3 tangent = normalize(randomVec - fd.normal * dot(randomVec, fd.normal));
	vec3 bitangent = cross(fd.normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, fd.normal);

	float ao = 0.f;

	for (int i = 0; i < KERNEL_SIZE; ++i)
	{
		vec3 dir = TBN * uAoKernel[i];

		vec3 samplePos = p + dir * rad;

		vec4 offset = uProjMatrix * vec4(samplePos, 1.f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float sampleDepth = texture(gbCamPosition, offset.xy).z;
		float diffDepth = sampleDepth - samplePos.z;

		if (sampleDepth < 0.f)
		{
			ao += smoothstep(0.f, 1.f, diffDepth - BIAS);
		}
	}

	ao /= float(KERNEL_SIZE);
	aoFactor = ao * INTENSITY;
}
