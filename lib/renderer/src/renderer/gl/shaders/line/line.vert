#version 450 core

layout( location = 0 ) in vec3 aVertexPosition;

uniform mat4 u_MVMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_normalMatrix;

out 
#include "struct_vertex_shader.glsl"
dataOut;

void main()
{
	dataOut.viewPosition = vec3( u_MVMatrix * vec4( aVertexPosition, 1.f ) );
	dataOut.normal	   = normalize( vec3( u_normalMatrix * vec4( aVertexPosition, 1.f ) ) );
	gl_Position		   = u_projMatrix * vec4( dataOut.viewPosition, 1.f );
}
