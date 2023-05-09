#version 450 core

#include "global_uniforms.glsl"

layout( binding = 1 ) uniform sampler2D depthTexture;

// Out.
layout( location = 0 ) out float linearDepth;

float linearizeDepth( const vec4 clipInfo, const float depth )
{
	if ( bool(uniforms.boolData.x) )
	{
		// Perspective: ( zNear * zFar ) / ( zFar - depth * ( zFar - zNear ) ).	
		return clipInfo[ 0 ] / ( clipInfo[ 1 ] - depth * clipInfo[ 2 ] );
	}
	else
	{
		// Ortho: depth * (zFar - zNear) + zNear.
		return depth * clipInfo[ 2 ] + clipInfo[ 3 ];
	}
}

void main() 
{
	linearDepth = linearizeDepth( uniforms.cameraClipInfos, texelFetch( depthTexture, ivec2( gl_FragCoord.xy ), 0 ).x ); 
}
