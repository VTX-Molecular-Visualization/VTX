#version 450 core

#include "../layout_uniforms_camera.glsl"
#include "struct_vertex_shader.glsl"

// In.
layout( location = 0 ) in vec3 inVertexPosition;

// Out.
smooth out StructVertexShader vsData; 

void main()
{
	vsData.viewPosition = vec3( uniformsCamera.matrixView * vec4( inVertexPosition, 1.f ) );
	vsData.normal	    = vec3( 1.f, 1.f, 1.f );//normalize( vec3( uniformsCamera.matrixNormal * vec4( inVertexPosition, 1.f ) ) );
	gl_Position		    =  uniformsCamera.matrixProjection * vec4( vsData.viewPosition, 1.f );
}
