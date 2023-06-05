#version 450 core

#include "global_uniforms.glsl"
#include "struct_data_packed.glsl"

// In.
layout( binding = 0 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 1 ) uniform sampler2D colorTexture;
layout( binding = 2 ) uniform sampler2D linearDepthTexture;

// Out.
layout( location = 0 ) out vec4 fragColor;

void main()
{
	const ivec2 texPos = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( gbViewPositionNormal, data, texPos );

	const vec2 texCoord = gl_FragCoord.xy / vec2( textureSize( linearDepthTexture, 0 ) );
	if ( data.selected == 0 )
	{
		fragColor = texture( colorTexture, texCoord );
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
		fragColor = edgeDepth > threshold + 0.025 ? getSelectionColor() : texture( colorTexture, texCoord );
	}
}