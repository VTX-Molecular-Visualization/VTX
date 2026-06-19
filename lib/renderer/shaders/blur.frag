#version 460 core

// In.
layout( binding = 0 ) uniform sampler2D inTextureColor;
layout( binding = 1 ) uniform sampler2D inTextureDepth;

layout( std140, binding = 2 ) uniform Uniforms
{
	ivec2 direction;
	float size;
	float scale;
}

uniforms;

// Out.
layout( location = 0 ) out float outBlur;

ivec2 depthTexCoord( const ivec2 p_texCoord )
{
	const ivec2 depthTextureSize = textureSize( inTextureDepth, 0 );
	const ivec2 texCoord		 = ivec2( ( vec2( p_texCoord ) + 0.5f ) * uniforms.scale );
	return clamp( texCoord, ivec2( 0 ), depthTextureSize - 1 );
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	const float inputCenter = texelFetch( inTextureColor, texCoord, 0 ).x;
	const float depthCenter = texelFetch( inTextureDepth, depthTexCoord( texCoord ), 0 ).x;
	const float blurSigma	= uniforms.size * 0.5f;
	const float blurFalloff = 1.f / ( 2.f * blurSigma * blurSigma );

	// Adapt sharpness wrt depth: the deeper the fragment is, the weaker the sharpness is.
	const float sharpness = max( 1.f, 64.f / depthCenter );

	float res	 = inputCenter;
	float weight = 1.f;

	// Compute blur contribution on each side in the given direction.
	for ( int i = 1; i <= uniforms.size; ++i )
	{
		const ivec2 uv			 = texCoord + i * uniforms.direction;
		const float inputCurrent = texelFetch( inTextureColor, uv, 0 ).x;
		const float depthCurrent = texelFetch( inTextureDepth, depthTexCoord( uv ), 0 ).x;

		const float depthDiff = ( depthCurrent - depthCenter ) * sharpness;

		const float w = exp2( fma( -( i * i ), blurFalloff, -depthDiff * depthDiff ) );

		res += inputCurrent * w;
		weight += w;
	}
	for ( int i = 1; i <= uniforms.size; ++i )
	{
		const ivec2 uv			 = texCoord - i * uniforms.direction;
		const float inputCurrent = texelFetch( inTextureColor, uv, 0 ).x;
		const float depthCurrent = texelFetch( inTextureDepth, depthTexCoord( uv ), 0 ).x;

		const float depthDiff = ( depthCurrent - depthCenter ) * sharpness;

		const float w = exp2( fma( -( i * i ), blurFalloff, -depthDiff * depthDiff ) );

		res += inputCurrent * w;
		weight += w;
	}

	// Apply blur.
	outBlur = res / weight;
}
