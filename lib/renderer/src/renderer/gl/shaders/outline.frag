#version 450 core

#include "global_uniforms.glsl"

// In.
layout( binding = 0 ) uniform sampler2D colorTexture;
layout( binding = 1 ) uniform sampler2D linearDepthTexture;

// Out.
layout( location = 0 ) out vec4 fragColor;

void main()
{
	const vec2 textSize = vec2( textureSize( linearDepthTexture, 0 ) );
	const vec2 texCoord = gl_FragCoord.xy / textSize;

	// Get current pixel depth.
	const float depthCenter = texture( linearDepthTexture, texCoord, 0 ).x;

	//
	float halfThicknessFloor = floor( uniforms.intData.x * 0.5f );
	float halfThicknessCeil	 = ceil(  uniforms.intData.x * 0.5f );

	// Get cross neighbor depth
	const ivec2 offsets[ 4 ] = { ivec2( -halfThicknessFloor, -halfThicknessFloor ),
								 ivec2( -halfThicknessFloor, halfThicknessCeil ),
								 ivec2( halfThicknessCeil, halfThicknessCeil ),
								 ivec2( halfThicknessCeil, -halfThicknessFloor ) };

	const vec4 depthNeighbours = { texture( linearDepthTexture, texCoord + offsets[ 0 ] / textSize ).x,
								   texture( linearDepthTexture, texCoord + offsets[ 1 ] / textSize ).x,
								   texture( linearDepthTexture, texCoord + offsets[ 2 ] / textSize ).x,
								   texture( linearDepthTexture, texCoord + offsets[ 3 ] / textSize ).x };

	// Compute depth difference in cross: no need abs because squared for edge depth.
	const float depthDiff0 = depthNeighbours.x - depthNeighbours.z;
	const float depthDiff1 = depthNeighbours.y - depthNeighbours.w;

	// Roberts cross operator for edge detection.
	const float edgeDepth = sqrt( depthDiff0 * depthDiff0 + depthDiff1 * depthDiff1 ) * 100.f;

	// Compute threshold.
	const float depthThreshold = ( 1.f / uniforms.floatData.y ) * depthCenter; // max( 1.f, depthCenter );

	// Apply outline if edge depth is greater than threshold.
	fragColor = edgeDepth > depthThreshold ? uniforms.outlineColor : texture( colorTexture, texCoord );
}
