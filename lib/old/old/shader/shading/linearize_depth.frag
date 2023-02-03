#version 450

layout( binding = 0 ) uniform sampler2D depthTexture;

layout( location = 0 ) out float linearDepth;

uniform vec4 uClipInfo;

// TODO: implement ortho cam ! if clipInfo[3] == 0
// clipInfo = (zNear * zfar, zFar, zFar - zNear, isPerspective)
float linearizeDepth( const vec4 clipInfo, const float depth )
{
	// ortho:
	// perspective: ( zNear * zFar ) / ( zFar - depth * ( zFar - zNear ) );
	return clipInfo[ 0 ] / ( clipInfo[ 1 ] - depth * clipInfo[ 2 ] );
}

void main() 
{ 
	linearDepth = linearizeDepth( uClipInfo, texelFetch( depthTexture, ivec2( gl_FragCoord.xy ), 0 ).x ); 
}
