#version 450 core

#include "layout_uniforms_camera.glsl"
#include "struct_data_packed.glsl"

// Crytek (Crysis) like SSAO

// In.
layout( binding = 0 ) uniform usampler2D inTexturePackedData;
layout( binding = 1 ) uniform sampler2D inTextureNoise;
layout( binding = 2 ) uniform sampler2D inTextureDepth;

layout ( std140, binding = 3 ) uniform Uniforms
{	
	float intensity;
	float radius;
	int kernelSize;
} uniforms;


const int horizRatio = 2;
const int vertiRatio = 2;
// Out.
layout( location = 0 ) out float outAmbientOcclusion;

const float BIAS = 0.025f;
const vec3[16] aoKernel = vec3[16](
    vec3(-0.00033631022, 0.0010243914, 0.002448698),
    vec3(0.0034475655, 0.0014735218, 0.012867554),
    vec3(0.01098434, -0.0017593445, 0.016593954),
    vec3(-0.021057466, 0.0018550212, 0.025669338),
    vec3(0.018788554, 0.004227499, 0.0294004),
    vec3(-0.0067719393, 0.009231464, 0.013406272),
    vec3(0.08500118, 0.00941014, 0.04617175),
    vec3(0.055248156, -0.076622084, 0.0072870906),
    vec3(0.026444966, 0.005659814, 0.05082282),
    vec3(-0.025307063, -0.07054181, 0.24715291),
    vec3(-0.10994154, -0.04454086, 0.27159324),
    vec3(-0.22478794, -0.06995961, 0.13159482),
    vec3(-0.24298586, 0.046245884, 0.2124926),
    vec3(0.1463565, 0.306015, 0.4069092),
    vec3(0.18017945, -0.15394086, 0.4013861),
    vec3(-0.444832, 0.36629602, 0.65063953)
);
const int kernelSize = aoKernel.length();
const int noiseTextureSize = 64;

void main()
{
	const ivec2 texPos = ivec2(gl_FragCoord.xy );

	const ivec2 smallerTexPos = ivec2(texPos.x*horizRatio, texPos.y*vertiRatio);

	UnpackedData data;
	unpackData( inTexturePackedData, data, smallerTexPos );
	const vec3 pos = data.viewPosition;


	// Adapt radius wrt depth: the deeper the fragment is, the larger the radius is.
	const float radius = -pos.z;

	const vec3 randomVec = normalize( texture( inTextureNoise, smallerTexPos / float( noiseTextureSize ) ).xyz );
	// Gram-Schmidt process.
	const vec3 tangent	 = normalize( randomVec - data.normal * dot( randomVec, data.normal ) );
	const vec3 bitangent = cross( data.normal, tangent );
	const mat3 TBN		 = mat3( tangent, bitangent, data.normal );

	float ao = 0.f;

	for ( int i = 0; i < kernelSize; ++i )
	{
		// Compute sample position.
		const vec3 samplePos = TBN * aoKernel[i] * radius + pos;

		// Project sample position.
		vec4 offset = uniformsCamera.matrixProjection * vec4( samplePos, 1.f );
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5f + 0.5f;

		// Get sample depth.
		float sampleDepth = -texture( inTextureDepth, offset.xy ).x;

		// Range check: ignore background.
		const float rangeCheck = sampleDepth == 0.f ? 0.f : smoothstep( 0.f, 1.f, radius / abs( pos.z - sampleDepth ) );
		ao += ( sampleDepth >= samplePos.z + BIAS ? 1.f : 0.f ) * rangeCheck;
	}

	ao				 = 1.f - ( ao / kernelSize );
	outAmbientOcclusion = pow( ao, uniforms.intensity );
	
}
