#version 450

layout( binding = 0 ) uniform sampler2D ambientOcclusionTexture;

layout( location = 0 ) out float ambientOcclusionBlurred;

uniform int uBlurSize;

void main()
{
	const vec2 texelSize = 1.f / textureSize( ambientOcclusionTexture, 0 );
	const vec2 texPos	 = gl_FragCoord.xy * texelSize;

	float res = 0.f;

	const vec2 lim = vec2( fma( -uBlurSize, 0.5f, 0.5f ) );

	for ( int i = 0; i < uBlurSize; ++i )
	{
		for ( int j = 0; j < uBlurSize; ++j )
		{
			vec2 offset = ( lim + vec2( i, j ) ) * texelSize;
			res += texture( ambientOcclusionTexture, texPos + offset ).x;
		}
	}
	ambientOcclusionBlurred = res / float( uBlurSize * uBlurSize );
}
