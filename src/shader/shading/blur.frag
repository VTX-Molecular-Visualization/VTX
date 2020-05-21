#version 450

layout( binding = 0 ) uniform sampler2D ambientOcclusionTexture;
layout( binding = 1 ) uniform sampler2D depthTexture;

layout( location = 0 ) out float ambientOcclusionBlurred;

uniform int uBlurSize;

void main()
{
	const vec2 texelSize = 1.f / textureSize( ambientOcclusionTexture, 0 );
	const vec2 texCoord	 = gl_FragCoord.xy * texelSize;

	const float aoCenter	= texture( ambientOcclusionTexture, texCoord ).x;
	const float depthCenter = texture( depthTexture, texCoord ).x;
	const float blurSigma	= uBlurSize * 0.5f;
	const float blurFalloff = 1.f / ( 2.f * blurSigma * blurSigma );

	float res	 = aoCenter;
	float weight = 1.f;

	const vec2 lim = vec2( fma( -uBlurSize, 0.5f, 0.5f ) );

	for ( float i = 1.f; i <= uBlurSize; ++i )
	{
		for ( float j = 1.f; j <= uBlurSize; ++j )
		{
			const vec2	uv	  = fma( ( lim + vec2( i, j ) ), texelSize, texCoord );
			const float ao	  = texture( ambientOcclusionTexture, uv ).x;
			const float depth = texture( depthTexture, uv ).x;

			const float depthDiff = ( depth - depthCenter ) * 1e4f;
			const float w		  = exp2( fma(-( i * j ), blurFalloff, - depthDiff * depthDiff ));

			res += ao * w;
			weight += w;
		}
	}
	ambientOcclusionBlurred = res / weight;
}
