#version 450 core

layout( binding = 0 ) uniform usampler2D gbColorNormal;
layout( binding = 1 ) uniform sampler2D gbViewPosition;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform vec3  uBackgroundColor;
uniform float uFogNear;
uniform float uFogFar;
uniform float uFogDensity;

out vec4 fragColor;

struct UnpackedData
{
	vec3 color;
	vec3 normal;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 colorNormal = texelFetch( gbColorNormal, px, 0 );

	const vec2 tmp = unpackHalf2x16( colorNormal.y );
	data.color	   = vec3( unpackHalf2x16( colorNormal.x ), tmp.x );
	data.normal	   = vec3( tmp.y, unpackHalf2x16( colorNormal.z ) );
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

	// Light on camera.
	const vec3 lightDir = normalize( -viewPosition );

	const float cosTheta = max( dot( data.normal, lightDir ), 0.f );

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	const float fogFactor = smoothstep( uFogNear, uFogFar, -viewPosition.z ) * uFogDensity;
	const vec3	color	  = data.color * cosTheta * ambientOcclusion;

	fragColor = vec4( mix( color, uBackgroundColor, fogFactor ), 1.f );
}
