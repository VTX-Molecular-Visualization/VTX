#version 450

#include "global_uniforms.glsl"

// In.
layout( binding = 1 ) uniform sampler2D inputTexture;
layout( binding = 2 ) uniform sampler2D linearDepthTexture;

uniform ivec2 uDirection;

// Out.
layout( location = 0 ) out float blurred;

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	const float inputCenter = texelFetch( inputTexture, texCoord, 0 ).x;
	const float depthCenter = texelFetch( linearDepthTexture, texCoord, 0 ).x;
	const float blurSigma	= uniforms.uintData.y * 0.5f;
	const float blurFalloff = 1.f / ( 2.f * blurSigma * blurSigma );

	// Adapt sharpness wrt depth: the deeper the fragment is, the weaker the sharpness is.
	const float sharpness = max( 1.f, 64.f / depthCenter );

	float res	 = inputCenter;
	float weight = 1.f;

	// Compute blur contribution on each side in the given direction.
	for ( int i = 1; i <= uniforms.uintData.y; ++i )
	{
		const ivec2 uv			 = texCoord + i * uDirection;
		const float inputCurrent = texelFetch( inputTexture, uv, 0 ).x;
		const float depthCurrent = texelFetch( linearDepthTexture, uv, 0 ).x;

		const float depthDiff = ( depthCurrent - depthCenter ) * sharpness;

		const float w = exp2( fma( -( i * i ), blurFalloff, -depthDiff * depthDiff ) );

		res += inputCurrent * w;
		weight += w;
	}
	for ( int i = 1; i <= uniforms.uintData.y; ++i )
	{
		const ivec2 uv			 = texCoord - i * uDirection;
		const float inputCurrent = texelFetch( inputTexture, uv, 0 ).x;
		const float depthCurrent = texelFetch( linearDepthTexture, uv, 0 ).x;

		const float depthDiff = ( depthCurrent - depthCenter ) * sharpness;

		const float w = exp2( fma( -( i * i ), blurFalloff, -depthDiff * depthDiff ) );

		res += inputCurrent * w;
		weight += w;
	}

	// Apply blur.
	blurred = res / weight;
}
