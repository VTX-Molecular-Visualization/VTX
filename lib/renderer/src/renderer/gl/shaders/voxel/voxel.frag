#version 450 core

#include "../global_uniforms.glsl"
#include "../struct_data_packed.glsl"

// In.
in
#include "struct_geometry_shader.glsl"
inData;

// Out.
layout( location = 0 ) out PackedData outDataPacked;
layout( location = 1 ) out vec4 outColor;

void main()
{
	vec3 viewPosition = vec3( uniforms.matrixView *  uniforms.matrixModel * vec4( inData.center, 1.f ) );
	vec3 normal		  = normalize( vec3( uniforms.matrixNormal * vec4( inData.center, 1.f ) ) );	
	
	packData( viewPosition, normal, 0, outDataPacked );
	outColor = vec4( 0.5f, 0.5f, 0.5f, 32.f );
}
