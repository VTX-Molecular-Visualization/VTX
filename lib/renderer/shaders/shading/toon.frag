#version 460 core

#include "common.glsl"

float computeLighting( const UnpackedData p_data, const vec3 p_lightDirection )
{
	const float intensity = dot( p_data.normal, p_lightDirection );
	float		lighting  = 1.f;

	for ( uint i = 0; i < uniforms.toonSteps; ++i )
	{
		const float range = float( i + 1 ) / float( uniforms.toonSteps );
		if ( intensity < range )
		{
			lighting = range;
			break;
		}
	}
	return lighting;
}

#include "main.glsl"
