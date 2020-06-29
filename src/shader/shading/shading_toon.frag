#version 450 core

layout( binding = 0 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 1 ) uniform sampler2D gbColor;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform vec3 uBackgroundColor;

uniform float uFogNear;
uniform float uFogFar;
uniform float uFogDensity;
uniform vec3  uFogColor;

uniform vec3 uLightPosition;
uniform vec3 uLightColor;

out vec4 fragColor;

struct UnpackedData
{
	vec3 viewPosition;
	vec3 normal;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 viewPositionNormal = texelFetch( gbViewPositionNormal, px, 0 );

	const vec2 tmp	  = unpackHalf2x16( viewPositionNormal.y );
	data.viewPosition = vec3( unpackHalf2x16( viewPositionNormal.x ), tmp.x );
	data.normal		  = vec3( tmp.y, unpackHalf2x16( viewPositionNormal.z ) );
}

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackGBuffers( texCoord, data );

	if ( data.viewPosition.z == 0.f )
	{
		if ( uFogDensity != 0.f )
			fragColor = vec4( mix( uBackgroundColor, uFogColor, uFogDensity ) * uLightColor, 1.f );
		else
			fragColor = vec4( uBackgroundColor, 1.f );
		return;
	}

	// Light on camera.
	const vec3 lightDir = normalize( uLightPosition - data.viewPosition );

	const float intensity = dot( data.normal, lightDir );
	float		lighting  = 1.f;

	if ( intensity < 0.25f )
		lighting = 0.2f;
	else if ( intensity < 0.5f )
		lighting = 0.4f;
	else if ( intensity < 0.75f )
		lighting = 0.55f;
	else if ( intensity < 0.95f )
		lighting = 0.7f;

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	const float fogFactor = smoothstep( uFogNear, uFogFar, -data.viewPosition.z ) * uFogDensity;
	const vec3	color	  = texelFetch( gbColor, texCoord, 0 ).xyz * lighting * ambientOcclusion;

	fragColor = vec4( mix( color, uBackgroundColor, fogFactor ) * uLightColor, 1.f );
}
