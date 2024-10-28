#version 450 core

#include "../layout_uniforms_camera.glsl"

// In.
layout( location = 0 ) in vec3 inVertexPosition;

// Out.
out 
#include "struct_vertex_shader.glsl"
outData;

void main()
{
	outData.viewPosition = vec3( uniformsCamera.matrixView * vec4( inVertexPosition, 1.f ) );
	outData.normal	     = vec3( 1.f, 1.f, 1.f );//normalize( vec3( uniformsCamera.matrixNormal * vec4( inVertexPosition, 1.f ) ) );
	gl_Position		     =  uniformsCamera.matrixProjection * vec4( outData.viewPosition, 1.f );
}
