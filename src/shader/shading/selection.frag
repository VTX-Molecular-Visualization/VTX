#version 450

layout( binding = 0 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 1 ) uniform sampler2D colorTexture;
layout( binding = 2 ) uniform sampler2D linearDepthTexture;

layout( location = 0 ) out vec4 fragColor;

uniform mat4 uProjMatrix;
uniform vec3 uLineColor;

void main()
{
	const uvec4			 viewPositionNormal = texelFetch( gbViewPositionNormal, ivec2( gl_FragCoord.xy ), 0 );
	const uint selection			= uint( unpackHalf2x16( viewPositionNormal.w ).x );

	const vec2 texCoord = gl_FragCoord.xy / vec2( textureSize( linearDepthTexture, 0 ) );
	if ( selection == 0 )
	{
		fragColor = vec4( texture( colorTexture, texCoord ).xyz, 1.f );
	}
	else
	{
		// Get current pixel depth.
		const float depthCenter = texture( linearDepthTexture, texCoord, 0 ).x;
		// Get cross neighbor depth
		const ivec2 o[ 4 ]			= { ivec2( -1, -1 ), ivec2( -1, 1 ), ivec2( 1, 1 ), ivec2( 1, -1 ) };
		const vec4	depthNeighbours = textureGatherOffsets( linearDepthTexture, texCoord, o );

		// Compute threshold wrt depth
		// TODO: allow the user to control it
		const float threshold = 0.1f * max( 1.f, depthCenter );

		// Compute depth difference in cross: no need abs because squared for edge depth.
		const float depthDiff0 = depthNeighbours.x - depthNeighbours.z;
		const float depthDiff1 = depthNeighbours.y - depthNeighbours.w;

		// Roberts cross operator for edge detection.
		const float edgeDepth = sqrt( depthDiff0 * depthDiff0 + depthDiff1 * depthDiff1 );

		// Apply outline if edge depth is greater than threshold.
		fragColor = vec4( edgeDepth > threshold + 0.025 ? uLineColor : texture( colorTexture, texCoord ).xyz, 1.f );
	}
}
