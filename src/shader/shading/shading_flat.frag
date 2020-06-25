#version 450 core

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 1 ) uniform sampler2D gbViewPosition;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform vec3 uBackgroundColor;
uniform float uFogNear;
uniform float uFogFar;
uniform float uFogDensity;

out vec4 fragColor;

struct UnpackedData
{
	vec3 color;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 colorNormal = texelFetch( gbColorNormal, px, 0 );

	const vec2 tmp = unpackHalf2x16( colorNormal.y );
	data.color	   = vec3( unpackHalf2x16( colorNormal.x ), tmp.x );
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackGBuffers( texCoord, data );
	const vec3 viewPosition = texelFetch( gbViewPosition, texCoord, 0 ).xyz;

	if ( viewPosition.z == 0.f )
	{
		fragColor = vec4( uBackgroundColor, 1.f );
		return;
	}

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;
	

	const float fogFactor = smoothstep( uFogNear, uFogFar, -viewPosition.z ) * uFogDensity;
	const vec3	color	  = data.color * ambientOcclusion;

	fragColor = vec4( mix( color, uBackgroundColor, fogFactor ), 1.f );
}
