#version 450 core

#include "global_uniforms.glsl"
#include "struct_data_packed.glsl"

// In.
layout( binding = 0 ) uniform usampler2D gbViewPositionNormal;
layout( binding = 1 ) uniform sampler2D gbColor;
layout( binding = 2 ) uniform sampler2D gbAmbientOcclusion;

// Out.
out vec4 fragColor;

const uint DIFFUSE = 0;
const uint GLOSSY = 1;
const uint TOON = 2;
const uint FLAT_COLOR = 3;

void main()
{
	const ivec2 texCoord = ivec2( gl_FragCoord.xy );

	UnpackedData data;
	unpackData( gbViewPositionNormal, data, texCoord );

	if ( data.viewPosition.z == 0.f )
	{
		if ( getFogDensity() != 0.f )
		{
			fragColor = vec4( mix( vec3( getBackgroundColor() ),  vec3( getFogColor() ), getFogDensity() ) *  vec3( getLightColor() ), getBackgroundColor().w );
		}
		else
		{
			fragColor = getBackgroundColor();
		}
		return;
	}

	// Lighting (on camera).
	const vec3 lightDir = isCameraPerspective() ? normalize( -data.viewPosition ) : vec3( 0.f, 0.f, 1.f );

	// FLAT_COLOR.
	float lighting = 1.f;
	// DIFFUSE.
	if( getShadingMode() == DIFFUSE )
	{		
		lighting = max( dot( data.normal, lightDir ), 0.f );
	}
	// GLOSSY.
	else if( getShadingMode() == GLOSSY )
	{		
		const float diffuse = 1.f - getSpecularFactor();
		const vec3	viewDir = normalize( -data.viewPosition );
		const vec3	h		= normalize( lightDir + viewDir );
		const float specular = getSpecularFactor() * pow( max( dot( h, data.normal ), 0.f ), texelFetch( gbColor, texCoord, 0 ).w );
		const float cosTheta = max( dot( data.normal, lightDir ), 0.f );
		lighting = ( diffuse + specular ) * cosTheta;
	}
	// TOON.
	else if( getShadingMode() == TOON ) 
	{
		const float intensity = dot( data.normal, lightDir );		

		if ( intensity < 0.25f ) lighting = 0.2f;
		else if ( intensity < 0.5f ) lighting = 0.4f;
		else if ( intensity < 0.75f ) lighting = 0.55f;
		else if ( intensity < 0.95f ) lighting = 0.7f;
	}

	const float ambientOcclusion = texelFetch( gbAmbientOcclusion, texCoord, 0 ).x;

	const float fogFactor = smoothstep( getFogNear(), getFogFar(), -data.viewPosition.z ) * getFogDensity();
	const vec3	color	  = texelFetch( gbColor, texCoord, 0 ).xyz * ambientOcclusion * lighting;

	fragColor = vec4( mix( color, vec3( getFogColor() ), fogFactor ) * vec3( getLightColor() ), 1.f );
}
