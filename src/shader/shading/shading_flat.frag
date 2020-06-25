#version 450 core

layout( binding = 0 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 1 ) uniform sampler2D gbColor;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform vec3  uBackgroundColor;
uniform float uFogNear;
uniform float uFogFar;
uniform float uFogDensity;

out vec4 fragColor;

struct UnpackedData
{
	vec3 viewPosition;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 viewPositionNormal = texelFetch( gbViewPositionNormal, px, 0 );

	const vec2 tmp	  = unpackHalf2x16( viewPositionNormal.y );
	data.viewPosition = vec3( unpackHalf2x16( viewPositionNormal.x ), tmp.x );
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackGBuffers( texCoord, data );

	if ( data.viewPosition.z == 0.f )
	{
		fragColor = vec4( uBackgroundColor, 1.f );
		return;
	}

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	const float fogFactor = smoothstep( uFogNear, uFogFar, -data.viewPosition.z ) * uFogDensity;
	const vec3	color	  = texelFetch( gbColor, texCoord, 0 ).xyz * ambientOcclusion;

	fragColor = vec4( mix( color, uBackgroundColor, fogFactor ), 1.f );
}
