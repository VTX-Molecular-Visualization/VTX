#version 450

#include "global_uniforms.glsl"

// Crytek (Crysis) like SSAO

// In.
layout( binding = 1 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 2 ) uniform sampler2D noise;
layout( binding = 3 ) uniform sampler2D linearDepth;

uniform vec3  uAoKernel[ 512 ];
uniform int	  uKernelSize;
uniform float uNoiseSize;

// Out.
layout( location = 0 ) out float ambientOcclusion;

const float BIAS = 0.025f;

struct UnpackedData
{
	vec3 viewPosition;
	vec3 normal;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 viewPositionNormal = texelFetch( gbViewPositionNormal, px, 0 );

	const vec2 tmp	  = unpackHalf2x16( viewPositionNormal.y );
	data.viewPosition = vec3( unpackHalf2x16( viewPositionNormal.x ), tmp.x );
	data.normal		  = vec3( tmp.y, unpackHalf2x16( viewPositionNormal.z ) );
}

void main()
{
	const ivec2 texPos = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackGBuffers( texPos, data );
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
		vec4 offset = uniforms.matrixProjection * vec4( samplePos, 1.f );
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5f + 0.5f;

		// Get sample depth.
		float sampleDepth = -texture( linearDepth, offset.xy ).x;

		// Range check: ignore background.
		const float rangeCheck = sampleDepth == 0.f ? 0.f : smoothstep( 0.f, 1.f, radius / abs( pos.z - sampleDepth ) );
		ao += ( sampleDepth >= samplePos.z + BIAS ? 1.f : 0.f ) * rangeCheck;
	}

	ao				 = 1.f - ( ao / uKernelSize );
	ambientOcclusion = pow( ao, uniforms.uintData.x );
}
