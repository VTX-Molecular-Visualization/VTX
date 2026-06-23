#version 460 core

#include "common.glsl"

float computeLighting( const UnpackedData p_data, const vec3 p_lightDirection )
{
	const float diffuse		  = 1.f - uniforms.specularFactor;
	const vec3	viewDirection = normalize( -p_data.viewPosition );
	const vec3	halfway		  = normalize( p_lightDirection + viewDirection );
	const float specular
		= uniforms.specularFactor * pow( max( dot( halfway, p_data.normal ), 0.f ), uniforms.shininess );
	const float cosTheta = max( dot( p_data.normal, p_lightDirection ), 0.f );
	return ( diffuse + specular ) * cosTheta;
}

#include "main.glsl"
