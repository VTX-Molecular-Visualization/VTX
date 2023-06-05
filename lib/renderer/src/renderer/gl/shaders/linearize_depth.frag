#version 450 core

#include "global_uniforms.glsl"

// In.
layout( binding = 0 ) uniform sampler2D depthTexture;

// Out.
layout( location = 0 ) out float linearDepth;

float linearizeDepth( const vec4 p_clipInfo, const float p_depth )
{
	if ( isCameraPerspective() )
	{
		// Perspective: ( zNear * zFar ) / ( zFar - depth * ( zFar - zNear ) ).	
		return p_clipInfo[ 0 ] / ( p_clipInfo[ 1 ] - p_depth * p_clipInfo[ 2 ] );
	}
	else
	{
		// Ortho: depth * (zFar - zNear) + zNear.
		return p_depth * p_clipInfo[ 2 ] + p_clipInfo[ 3 ];
	}
}

void main() 
{
	linearDepth = linearizeDepth( getCameraClipInfos(), texelFetch( depthTexture, ivec2( gl_FragCoord.xy ), 0 ).x ); 
}
