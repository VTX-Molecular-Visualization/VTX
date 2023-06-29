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
	vec3 aoKernel[ 512 ];
	int	kernelSize;
	float noiseSize;
} uniforms;

// Out.
layout( location = 0 ) out float outAmbientOcclusion;

const float BIAS = 0.025f;

void main()
{
	const ivec2 texPos = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( inTexturePackedData, data, texPos );
	const vec3 pos = data.viewPosition;

	// Adapt radius wrt depth: the deeper the fragment is, the larger the radius is.
	const float radius = -pos.z;

	const vec3 randomVec = normalize( texture( inTextureNoise, texPos / uniforms.noiseSize ).xyz );
	// Gram-Schmidt process.
	const vec3 tangent	 = normalize( randomVec - data.normal * dot( randomVec, data.normal ) );
	const vec3 bitangent = cross( data.normal, tangent );
	const mat3 TBN		 = mat3( tangent, bitangent, data.normal );

	float ao = 0.f;

	for ( int i = 0; i < uniforms.kernelSize; ++i )
	{
		// Compute sample position.
		const vec3 samplePos = TBN * uniforms.aoKernel[ i ] * radius + pos;

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

	ao				 = 1.f - ( ao / uniforms.kernelSize );
	outAmbientOcclusion = pow( ao, uniforms.intensity );
}
