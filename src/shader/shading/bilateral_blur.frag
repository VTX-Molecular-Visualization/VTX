#version 450

layout( binding = 0 ) uniform sampler2D inputTexture;
layout( binding = 1 ) uniform sampler2D depthTexture;

layout( location = 0 ) out float ambientOcclusionBlurred;

uniform int	 uBlurSize;
uniform int	 uBlurSharpness;
uniform vec2 uInvTexSize;

// TODO: pass clip info in uniform (zNear, zFar, zDiff = zFar - zNear)
float linearizeDepth( const float zNear, const float zFar, const float depth )
{
	return ( zNear * zFar ) / ( zFar - depth * ( zFar - zNear ) );
}

// TODO: check sharpness utility
void main()
{
	const vec2 texelSize = 1.f / textureSize( inputTexture, 0 );
	const vec2 texCoord	 = gl_FragCoord.xy * texelSize;

	const float aoCenter	= texture( inputTexture, texCoord ).x;
	const float depthCenter = linearizeDepth( 1e-1f, 1e4f, texture( depthTexture, texCoord ).x );
	const float blurSigma	= uBlurSize * 0.5f;
	const float blurFalloff = 1.f / ( 2.f * blurSigma * blurSigma );

	float res	 = aoCenter;
	float weight = 1.f;

	for ( float i = 1.f; i <= uBlurSize; ++i )
	{
		const vec2	uv	  = texCoord + i * uInvTexSize;
		const float ao	  = texture( inputTexture, uv ).x;
		const float depth = linearizeDepth( 1e-1f, 1e4f, texture( depthTexture, uv ).x );

		const float depthDiff = ( depth - depthCenter ) * uBlurSharpness;

		const float w = exp2( fma( -( i * i ), blurFalloff, -depthDiff * depthDiff ) );

		res += ao * w;
		weight += w;
	}
	for ( float i = 1.f; i <= uBlurSize; ++i )
	{
		const vec2	uv	  = texCoord - i * uInvTexSize;
		const float ao	  = texture( inputTexture, uv ).x;
		const float depth = linearizeDepth( 1e-1f, 1e4f, texture( depthTexture, uv ).x );

		const float depthDiff = ( depth - depthCenter ) * uBlurSharpness;

		const float w = exp2( fma( -( i * i ), blurFalloff, -depthDiff * depthDiff ) );

		res += ao * w;
		weight += w;
	}

	ambientOcclusionBlurred = res / weight;
}
