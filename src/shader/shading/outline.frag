#version 450

layout( binding = 0 ) uniform sampler2D colorTexture;
layout( binding = 1 ) uniform sampler2D linearDepthTexture;

layout( location = 0 ) out vec4 fragColor;

uniform mat4  uProjMatrix;
uniform vec3 uLineColor;

void main()
{
	const vec2 texelSize = 1.f / textureSize( colorTexture, 0 );
	const vec2 texCoord	 = gl_FragCoord.xy * texelSize;

	const float depthCenter = texture( linearDepthTexture, texCoord ).x;
	
	const float thickness = 4.f / min(1.f, abs(depthCenter));
	const float threshold = 0.5f;
	const vec2	lim		  = vec2( -thickness * 0.5f );
	float weight = 0.f;
	for ( float i = 1.f; i <= thickness; ++i )
	{
		for ( float j = 1.f; j <= thickness; ++j )
		{
			const vec3 v = vec3(lim + vec2( i, j ),1.f);
			vec4 offset = uProjMatrix * vec4( v, 1.f );
			offset.xy /= offset.w;
			offset.xy = offset.xy * 0.5f + 0.5f;

			const vec2	uv	  = fma( offset.xy, texelSize, texCoord );
			const float depth = texture( linearDepthTexture, uv ).x;

			const float depthDiff = depthCenter - depth;
			
			if ( depthDiff > threshold )
			{
				fragColor = vec4( uLineColor, 1.f );
				return;
			}
		}
	}
	fragColor = vec4( texture( colorTexture, texCoord ).xyz, 1.f );
}
