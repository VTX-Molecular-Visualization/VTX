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
	vec3 normal;
};

void unpackGBuffers( ivec2 px, out UnpackedData data )
{
	const uvec4 colorNormal		 = texelFetch( gbColorNormal, px, 0 );

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
	
	const float fogFactor = smoothstep( uFogNear, uFogFar, -viewPosition.z ) * uFogDensity;
	const vec3	color	  = data.color * lighting * ambientOcclusion;

	fragColor = vec4( mix( color, uBackgroundColor, fogFactor ), 1.f );
}
