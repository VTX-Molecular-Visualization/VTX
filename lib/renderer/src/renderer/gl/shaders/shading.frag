#version 450 core

#include "global_uniforms.glsl"

// In.
layout( binding = 1 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 2 ) uniform sampler2D gbColor;
layout( binding = 3 ) uniform sampler2D gbAmbientOcclusion;

// Out.
out vec4 fragColor;

const uint DIFFUSE = 0;
const uint GLOSSY = 1;
const uint TOON = 2;
const uint FLAT_COLOR = 3;

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
		if ( uniforms.fog.z != 0.f )
		{
			fragColor = vec4( mix( vec3( uniforms.backgroundColor ),  vec3( uniforms.fogColor ), uniforms.fog.z ) *  vec3( uniforms.lightColor ), uniforms.backgroundColor.w );
		}
		else
		{
			fragColor = uniforms.backgroundColor;
		}
		return;
	}

	// Lighting (on camera).
	const vec3 lightDir = bool(uniforms.boolData.x) ? normalize( -data.viewPosition ) : vec3( 0.f, 0.f, 1.f );

	// FLAT_COLOR.
	float lighting = 1.f;
	// DIFFUSE.
	if( uniforms.uintData.z == 0 )
	{		
		lighting = max( dot( data.normal, lightDir ), 0.f );
	}
	// GLOSSY.
	else if( uniforms.uintData.z == GLOSSY )
	{		
		const float diffuse = 1.f - uniforms.floatData.x;
		const vec3	viewDir = normalize( -data.viewPosition );
		const vec3	h		= normalize( lightDir + viewDir );
		const float specular = uniforms.floatData.x * pow( max( dot( h, data.normal ), 0.f ), texelFetch( gbColor, texCoord, 0 ).w );
		const float cosTheta = max( dot( data.normal, lightDir ), 0.f );
		lighting = ( diffuse + specular ) * cosTheta;
	}
	// TOON.
	else if( uniforms.uintData.z == TOON ) 
	{
		const float intensity = dot( data.normal, lightDir );		

		if ( intensity < 0.25f ) lighting = 0.2f;
		else if ( intensity < 0.5f ) lighting = 0.4f;
		else if ( intensity < 0.75f ) lighting = 0.55f;
		else if ( intensity < 0.95f ) lighting = 0.7f;
	}

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	const float fogFactor = smoothstep( uniforms.fog.x, uniforms.fog.y, -data.viewPosition.z ) * uniforms.fog.z;
	const vec3	color	  = texelFetch( gbColor, texCoord, 0 ).xyz * ambientOcclusion * lighting;

	fragColor = vec4( mix( color, vec3( uniforms.fogColor ), fogFactor ) * vec3( uniforms.lightColor ), 1.f );
}
