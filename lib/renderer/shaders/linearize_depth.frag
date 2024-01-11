#version 450 core

#include "layout_uniforms_camera.glsl"

// In.
layout( binding = 0 ) uniform sampler2D inTextureDepth;

// Out.
layout( location = 0 ) out float outLinearizedDepth;

float linearizeDepth( const vec4 p_clipInfo, const float p_depth )
{
	if ( uniformsCamera.isCameraPerspective == 1 )
	{
		//float ndc = p_depth * 2.0 - 1.0; 
		//return (2.0 * getCameraNear() * getCameraFar()) / (getCameraFar() + getCameraNear() - ndc * (getCameraFar() - getCameraNear()));	

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
	outLinearizedDepth = linearizeDepth( uniformsCamera.cameraClipInfos, texelFetch( inTextureDepth, ivec2( gl_FragCoord.xy ), 0 ).x ); 
}
