#version 450 core

layout( binding = 0 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 1 ) uniform sampler2D gbColor;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

uniform vec4 uBackgroundColor;
uniform uint uShadingEnum; // DIFFUSE = 0, GLOSSY, TOON, FLAT_COLOR
uniform float uFogNear;
uniform float uFogFar;
uniform float uFogDensity;
uniform vec3  uFogColor;
uniform vec3 uLightColor;
uniform bool uIsPerspective;
uniform float uSpecularFactor = 0.4f;

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
		{
			fragColor = vec4( mix( vec3( uBackgroundColor ), uFogColor, uFogDensity ) * uLightColor, uBackgroundColor.w );
		}
		else
		{
			fragColor = uBackgroundColor;
		}
		return;
	}

	// Lighting (on camera).
	const vec3 lightDir = uIsPerspective ? normalize( -data.viewPosition ) : vec3( 0.f, 0.f, 1.f );

	// FLAT_COLOR.
	float lighting = 1.f;
	// DIFFUSE.
	if( uShadingEnum == 0 )
	{		
		float lighting = max( dot( data.normal, lightDir ), 0.f );
	}
	// GLOSSY.
	else if( uShadingEnum == 1 )
	{		
		const float diffuse = 1.f - uSpecularFactor;
		const vec3	viewDir = normalize( -data.viewPosition );
		const vec3	h		= normalize( lightDir + viewDir );
		const float specular = uSpecularFactor * pow( max( dot( h, data.normal ), 0.f ), texelFetch( gbColor, texCoord, 0 ).w );

		const float cosTheta = max( dot( data.normal, lightDir ), 0.f );
		lighting = ( diffuse + specular ) * cosTheta;
	}
	// TOON.
	else if( uShadingEnum == 2 ) 
	{
		const float intensity = dot( data.normal, lightDir );		

		if ( intensity < 0.25f ) lighting = 0.2f;
		else if ( intensity < 0.5f ) lighting = 0.4f;
		else if ( intensity < 0.75f ) lighting = 0.55f;
		else if ( intensity < 0.95f ) lighting = 0.7f;
	}

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	const float fogFactor = smoothstep( uFogNear, uFogFar, -data.viewPosition.z ) * uFogDensity;
	const vec3	color	  = texelFetch( gbColor, texCoord, 0 ).xyz * ambientOcclusion * lighting;

	fragColor = vec4( mix( color, uFogColor, fogFactor ) * uLightColor, 1.f );
}
