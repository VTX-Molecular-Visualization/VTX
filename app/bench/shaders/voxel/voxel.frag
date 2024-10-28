#version 450 core

#include "../layout_uniforms_camera.glsl"
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
	vec3 viewPosition = vec3( uniformsCamera.matrixView * vec4( inData.center, 1.f ) );
	vec3 normal		  = normalize( vec3( uniformsCamera.matrixView * -vec4( inData.center, 1.f ) ) );
	
	packData( viewPosition, normal, 0, outDataPacked );
	outColor = vec4( 1.f, 1.f, 1.f, 1.f );
}