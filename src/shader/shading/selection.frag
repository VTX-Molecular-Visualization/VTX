#version 450

layout( binding = 0 ) uniform sampler2D colorTexture;
layout( binding = 1 ) uniform sampler2D linearDepthTexture;

layout( location = 0 ) out vec4 fragColor;

uniform mat4 uProjMatrix;
uniform vec3 uLineColor;

#define USE_TEXTURE_GATHER

void main()
{
#ifdef USE_TEXTURE_GATHER
	const vec2 texCoord = gl_FragCoord.xy / vec2( textureSize( linearDepthTexture, 0 ) );

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

#else

	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	// Get current pixel depth.
	const float depthCenter = texelFetch( linearDepthTexture, texCoord, 0 ).x;
	// Get cross neighbor depth
	const float depthNW = texelFetchOffset( linearDepthTexture, texCoord, 0, ivec2( -1, -1 ) ).x;
	const float depthNE = texelFetchOffset( linearDepthTexture, texCoord, 0, ivec2( -1, 1 ) ).x;
	const float depthSE = texelFetchOffset( linearDepthTexture, texCoord, 0, ivec2( 1, 1 ) ).x;
	const float depthSW = texelFetchOffset( linearDepthTexture, texCoord, 0, ivec2( 1, -1 ) ).x;

	// Compute threshold wrt depth
	// TODO: allow the user to control it
	const float threshold = 0.05f * max( 1.f, depthCenter );

	// Compute depth difference in cross: no need abs because squared for edge depth.
	const float depthDiff0 = depthNW - depthSE;
	const float depthDiff1 = depthNE - depthSW;

	// Roberts cross operator for edge detection.
	const float edgeDepth = sqrt( depthDiff0 * depthDiff0 + depthDiff1 * depthDiff1 );

	// Apply outline if edge depth is greater than threshold.
	fragColor = vec4( edgeDepth > threshold ? uLineColor : texelFetch( colorTexture, texCoord, 0 ).xyz, 1.f );
#endif
}
