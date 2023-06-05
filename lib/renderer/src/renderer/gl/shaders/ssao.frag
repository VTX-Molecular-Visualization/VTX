#version 450 core

#include "global_uniforms.glsl"
#include "struct_data_packed.glsl"

// Crytek (Crysis) like SSAO

// In.
layout( binding = 0 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 1 ) uniform sampler2D noise;
layout( binding = 2 ) uniform sampler2D linearDepth;

uniform vec3  uAoKernel[ 512 ];
uniform int	  uKernelSize;
uniform float uNoiseSize;

// Out.
layout( location = 0 ) out float ambientOcclusion;

const float BIAS = 0.025f;

void main()
{
	const ivec2 texPos = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( gbViewPositionNormal, data, texPos );
	const vec3 pos = data.viewPosition;

	// Adapt radius wrt depth: the deeper the fragment is, the larger the radius is.
	const float radius = -pos.z;

	const vec3 randomVec = normalize( texture( noise, texPos / uNoiseSize ).xyz );
	// Gram-Schmidt process.
	const vec3 tangent	 = normalize( randomVec - data.normal * dot( randomVec, data.normal ) );
	const vec3 bitangent = cross( data.normal, tangent );
	const mat3 TBN		 = mat3( tangent, bitangent, data.normal );

	float ao = 0.f;

	for ( int i = 0; i < uKernelSize; ++i )
	{
		// Compute sample position.
		const vec3 samplePos = TBN * uAoKernel[ i ] * radius + pos;

		// Project sample position.
		vec4 offset = getMatrixProjection() * vec4( samplePos, 1.f );
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5f + 0.5f;

		// Get sample depth.
		float sampleDepth = -texture( linearDepth, offset.xy ).x;

		// Range check: ignore background.
		const float rangeCheck = sampleDepth == 0.f ? 0.f : smoothstep( 0.f, 1.f, radius / abs( pos.z - sampleDepth ) );
		ao += ( sampleDepth >= samplePos.z + BIAS ? 1.f : 0.f ) * rangeCheck;
	}

	ao				 = 1.f - ( ao / uKernelSize );
	ambientOcclusion = pow( ao, getSSAOIntensity() );
}
