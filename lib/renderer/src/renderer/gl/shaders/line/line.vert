#version 450 core

#include "../global_uniforms.glsl"

// In.
layout( location = 0 ) in vec3 inVertexPosition;

// Out.
out 
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	outData.viewPosition = vec3( getMatrixView() * getMatrixModel() * vec4( inVertexPosition, 1.f ) );
	outData.normal	     = normalize( vec3( getMatrixNormal() * vec4( inVertexPosition, 1.f ) ) );
	gl_Position		     = getMatrixProjection() * vec4( outData.viewPosition, 1.f );
}
